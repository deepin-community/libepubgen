/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "EPUBFontManager.h"

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

std::string getFontExtension(const std::string &mimetype)
{
  static const std::unordered_map<std::string, std::string> extensionMap =
  {
    {"application/vnd.ms-opentype", "otf"},
  };

  const auto it = extensionMap.find(mimetype);
  return it == extensionMap.end() ? std::string("ttf") : it->second;
}

}

std::size_t EPUBFontManager::BinaryDataHash::operator()(const librevenge::RVNGBinaryData &data) const
{
  size_t seed = 0;

  const unsigned char *const buf = data.getDataBuffer();
  for (size_t i = 0; data.size() != i; ++i)
    boost::hash_combine(seed, buf[i]);

  return seed;
}

bool EPUBFontManager::BinaryDataEqual::operator()(const librevenge::RVNGBinaryData &left, const librevenge::RVNGBinaryData &right) const
{
  if (left.empty() && right.empty())
    return true;
  if (left.size() != right.size())
    return false;
  const unsigned char *const leftData = left.getDataBuffer();
  return std::equal(leftData, leftData + left.size(), right.getDataBuffer());
}

EPUBFontManager::EPUBFontManager(EPUBManifest &manifest)
  : m_manifest(manifest)
  , m_map()
  , m_number()
  , m_set()
{
}

void EPUBFontManager::insert(const librevenge::RVNGPropertyList &propertyList, const EPUBPath &base)
{
  librevenge::RVNGBinaryData data(propertyList["office:binary-data"]->getStr());
  librevenge::RVNGString mimetype(propertyList["librevenge:mime-type"]->getStr());
  if (mimetype == "truetype")
    // librevenge's truetype is EPUB's opentype.
    mimetype = "application/vnd.ms-opentype";

  MapType_t::const_iterator it = m_map.find(data);
  if (m_map.end() == it)
  {
    const std::string mime(mimetype.cstr());

    std::ostringstream nameBuf;
    nameBuf << "font" << std::setw(4) << std::setfill('0') << m_number.next();
    const std::string id = nameBuf.str();

    nameBuf << "." << getFontExtension(mime);

    const EPUBPath path(EPUBPath("OEBPS/fonts") / nameBuf.str());

    m_manifest.insert(path, mime, id, "");
    it = m_map.insert(MapType_t::value_type(data, path)).first;
  }

  assert(m_map.end() != it); // the font must be present at this point

  // Now collect CSS properties.
  EPUBCSSProperties content;
  extractFontProperties(propertyList, content);
  std::stringstream ss;
  ss << "url(";
  ss << it->second.relativeTo(base).str();
  ss << ")";
  content["src"] = ss.str();
  SetType_t::const_iterator contentIt = m_set.find(content);
  if (contentIt != m_set.end())
    return;

  m_set.insert(content);
}

void EPUBFontManager::extractFontProperties(librevenge::RVNGPropertyList const &pList, EPUBCSSProperties &cssProps) const
{
  if (pList["librevenge:name"])
  {
    std::ostringstream name;
    name << '\'' << pList["librevenge:name"]->getStr().cstr() << '\'';
    cssProps["font-family"] = name.str();
  }

  if (pList["librevenge:font-style"])
    cssProps["font-style"] = pList["librevenge:font-style"]->getStr().cstr();

  if (pList["librevenge:font-weight"])
    cssProps["font-weight"] = pList["librevenge:font-weight"]->getStr().cstr();
}

void EPUBFontManager::writeTo(EPUBPackage &package)
{
  for (MapType_t::const_iterator it = m_map.begin(); m_map.end() != it; ++it)
  {
    EPUBBinaryContent font;
    font.insertBinaryData(it->first);
    font.writeTo(package, it->second.str().c_str());
  }
}

void EPUBFontManager::send(EPUBCSSContent &out)
{
  for (const auto &fontProperties : m_set)
  {
    librevenge::RVNGPropertyList props;
    fillPropertyList(fontProperties, props);
    out.insertRule("@font-face", props);
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
