/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_EPUBIMAGEMANAGER_H
#define INCLUDED_EPUBIMAGEMANAGER_H

#include <string>
#include <unordered_map>
#include <unordered_set>

#include <boost/functional/hash.hpp>

#include <librevenge/librevenge.h>

#include "EPUBCSSProperties.h"
#include "EPUBCounter.h"
#include "EPUBPath.h"

namespace libepubgen
{

class EPUBManifest;
class EPUBPackage;
class EPUBCSSContent;

class EPUBImageManager
{
  // disable copying
  EPUBImageManager(const EPUBImageManager &);
  EPUBImageManager &operator=(const EPUBImageManager &);

  struct BinaryDataHash // : public std::unary_function<librevenge::RVNGBinaryData, std::size_t>
  {
    std::size_t operator()(const librevenge::RVNGBinaryData &data) const;
  };

  struct BinaryDataEqual // : public std::binary_function<librevenge::RVNGBinaryData, librevenge::RVNGBinaryData, bool>
  {
    bool operator()(const librevenge::RVNGBinaryData &left, const librevenge::RVNGBinaryData &right) const;
  };

  typedef std::unordered_map<librevenge::RVNGBinaryData, EPUBPath, BinaryDataHash, BinaryDataEqual> MapType_t;
  typedef std::unordered_map<EPUBCSSProperties, std::string, boost::hash<EPUBCSSProperties>> ContentNameMap_t;

public:
  explicit EPUBImageManager(EPUBManifest &manifest);

  const EPUBPath &insert(const librevenge::RVNGBinaryData &data, const librevenge::RVNGString &mimetype, const librevenge::RVNGString &properties="");

  void writeTo(EPUBPackage &package);

  //! returns the class name corresponding to a propertylist
  std::string getFrameClass(librevenge::RVNGPropertyList const &pList);
  //! returns the style string corresponding to a propertylist
  std::string getFrameStyle(librevenge::RVNGPropertyList const &pList);
  //! returns the style for a follow-up <br> element, based on wrapping properties.
  static std::string getWrapStyle(librevenge::RVNGPropertyList const &pList);
  //! send the data to the sink
  void send(EPUBCSSContent &out);

private:
  //! convert a property list into a CSS property map
  void extractImageProperties(librevenge::RVNGPropertyList const &pList, EPUBCSSProperties &cssProps) const;

  EPUBManifest &m_manifest;
  MapType_t m_map;
  EPUBCounter m_number;
  //! a map image content -> name
  ContentNameMap_t m_imageContentNameMap;
};

}

#endif // INCLUDED_EPUBIMAGEMANAGER

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
