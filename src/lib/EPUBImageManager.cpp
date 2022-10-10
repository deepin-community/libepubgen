/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "EPUBImageManager.h"

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <sstream>

#include <boost/functional/hash.hpp>

#include "EPUBBinaryContent.h"
#include "EPUBCSSContent.h"
#include "EPUBManifest.h"
#include "EPUBPath.h"

namespace libepubgen
{

using librevenge::RVNGBinaryData;

using std::string;

namespace
{

string getExtension(const string &mimetype)
{
  static const std::unordered_map<string, string> extensionMap =
  {
    {"image/gif", "gif"},
    {"image/jpeg", "jpg"},
    {"image/png", "png"},
    {"image/svg+xml", "svg"},
  };

  const auto it = extensionMap.find(mimetype);
  return (extensionMap.end() == it) ? string("img") : it->second;
}

}

std::size_t EPUBImageManager::BinaryDataHash::operator()(const librevenge::RVNGBinaryData &data) const
{
  size_t seed = 0;

  const unsigned char *const buf = data.getDataBuffer();
  for (size_t i = 0; data.size() != i; ++i)
    boost::hash_combine(seed, buf[i]);

  return seed;
}

bool EPUBImageManager::BinaryDataEqual::operator()(const librevenge::RVNGBinaryData &left, const librevenge::RVNGBinaryData &right) const
{
  if (left.empty() && right.empty())
    return true;
  if (left.size() != right.size())
    return false;
  const unsigned char *const leftData = left.getDataBuffer();
  return std::equal(leftData, leftData + left.size(), right.getDataBuffer());
}

EPUBImageManager::EPUBImageManager(EPUBManifest &manifest)
  : m_manifest(manifest)
  , m_map()
  , m_number()
  , m_imageContentNameMap()
{
}

const EPUBPath &EPUBImageManager::insert(const librevenge::RVNGBinaryData &data, const librevenge::RVNGString &mimetype, const librevenge::RVNGString &properties)
{
  MapType_t::const_iterator it = m_map.find(data);
  if (m_map.end() == it)
  {
    const string mime(mimetype.cstr());

    std::ostringstream nameBuf;
    nameBuf << "image" << std::setw(4) << std::setfill('0') << m_number.next();
    const string id = nameBuf.str();

    nameBuf << "." << getExtension(mime);

    const EPUBPath path(EPUBPath("OEBPS/images") / nameBuf.str());

    m_manifest.insert(path, mime, id, properties.cstr());
    it = m_map.insert(MapType_t::value_type(data, path)).first;
  }

  assert(m_map.end() != it); // the image must be present at this point

  return it->second;
}

void EPUBImageManager::writeTo(EPUBPackage &package)
{
  for (MapType_t::const_iterator it = m_map.begin(); m_map.end() != it; ++it)
  {
    EPUBBinaryContent image;
    image.insertBinaryData(it->first);
    image.writeTo(package, it->second.str().c_str());
  }
}

std::string EPUBImageManager::getFrameClass(librevenge::RVNGPropertyList const &pList)
{
  EPUBCSSProperties content;
  extractImageProperties(pList, content);
  ContentNameMap_t::const_iterator it=m_imageContentNameMap.find(content);
  if (it != m_imageContentNameMap.end())
    return it->second;
  std::stringstream s;
  s << "frame" << m_imageContentNameMap.size();
  m_imageContentNameMap[content]=s.str();
  return s.str();
}

std::string EPUBImageManager::getFrameStyle(librevenge::RVNGPropertyList const &pList)
{
  EPUBCSSProperties content;
  extractImageProperties(pList, content);

  std::stringstream s;
  for (const auto &property : content)
    s << property.first << ": " << property.second << "; ";
  return s.str();
}

void EPUBImageManager::extractImageProperties(librevenge::RVNGPropertyList const &pList, EPUBCSSProperties &cssProps) const
{
  // Positioning.
  librevenge::RVNGString anchorType;
  if (pList["text:anchor-type"])
    anchorType = pList["text:anchor-type"]->getStr();
  if (anchorType != "as-char")
  {
    // Horizontal position.
    librevenge::RVNGString horizontalPos;
    if (pList["style:horizontal-pos"])
      horizontalPos = pList["style:horizontal-pos"]->getStr();

    if (horizontalPos == "right")
      cssProps["float"] = "right";
    else
      cssProps["float"] = "left";
  }

  // Extract borders.
  static char const *type[] = {"border", "border-left", "border-top", "border-right", "border-bottom" };
  for (int i = 0; i < 5; i++)
  {
    std::string field("fo:");
    field+=type[i];
    if (!pList[field.c_str()])
      continue;
    cssProps[type[i]] =  pList[field.c_str()]->getStr().cstr();
  }

  // Extract size.
  if (auto pRelWidth = pList["style:rel-width"])
    cssProps["width"] = pRelWidth->getStr().cstr();
  else if (auto pWidth = pList["svg:width"])
    cssProps["width"] = pWidth->getStr().cstr();
}

std::string EPUBImageManager::getWrapStyle(librevenge::RVNGPropertyList const &pList)
{
  librevenge::RVNGString wrap;
  librevenge::RVNGString anchorType;
  std::string ret;

  if (pList["style:wrap"])
    wrap = pList["style:wrap"]->getStr();
  if (pList["text:anchor-type"])
    anchorType = pList["text:anchor-type"]->getStr();

  if (anchorType == "as-char")
    return ret;

  // Emulate wrap type with a break after the image.
  if (wrap == "none")
    ret = "clear: both;";
  else if (wrap == "left")
    // We want content on the left side, space on the right side, so the next
    // element should clear on its left.
    ret = "clear: left;";
  else if (wrap == "right")
    ret = "clear: right;";
  else if (wrap == "parallel")
    ret = "clear: none;";

  return ret;
}

void EPUBImageManager::send(EPUBCSSContent &out)
{
  for (auto it = m_imageContentNameMap.begin(); m_imageContentNameMap.end() != it; ++it)
  {
    librevenge::RVNGPropertyList props;
    fillPropertyList(it->first, props);
    out.insertRule(("." + it->second).c_str(), props);
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
