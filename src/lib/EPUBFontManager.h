/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_EPUBFONTMANAGER_H
#define INCLUDED_EPUBFONTMANAGER_H

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

/// Manages embedded fonts.
class EPUBFontManager
{
  // disable copying
  EPUBFontManager(const EPUBFontManager &);
  EPUBFontManager &operator=(const EPUBFontManager &);

  struct BinaryDataHash // : public std::unary_function<librevenge::RVNGBinaryData, std::size_t>
  {
    std::size_t operator()(const librevenge::RVNGBinaryData &data) const;
  };

  struct BinaryDataEqual // : public std::binary_function<librevenge::RVNGBinaryData, librevenge::RVNGBinaryData, bool>
  {
    bool operator()(const librevenge::RVNGBinaryData &left, const librevenge::RVNGBinaryData &right) const;
  };

  typedef std::unordered_map<librevenge::RVNGBinaryData, EPUBPath, BinaryDataHash, BinaryDataEqual> MapType_t;
  typedef std::unordered_set<EPUBCSSProperties, boost::hash<EPUBCSSProperties>> SetType_t;

public:
  explicit EPUBFontManager(EPUBManifest &manifest);

  void insert(const librevenge::RVNGPropertyList &propertyList, const EPUBPath &path);

  void writeTo(EPUBPackage &package);

  //! send the data to the sink
  void send(EPUBCSSContent &out);

private:
  //! convert a property list into a CSS property map
  void extractFontProperties(librevenge::RVNGPropertyList const &pList, EPUBCSSProperties &cssProps) const;

  EPUBManifest &m_manifest;
  /// Font filename -> content map.
  MapType_t m_map;
  EPUBCounter m_number;
  /// Set of font properties.
  SetType_t m_set;
};

}

#endif // INCLUDED_EPUBFONTMANAGER

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
