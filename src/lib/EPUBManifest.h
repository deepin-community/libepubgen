/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_EPUBMANIFEST_H
#define INCLUDED_EPUBMANIFEST_H

#include <string>
#include <tuple>
#include <unordered_map>

namespace libepubgen
{

class EPUBPath;
class EPUBXMLContent;

class EPUBManifest
{
  // disable copying
  EPUBManifest(const EPUBManifest &);
  EPUBManifest &operator=(const EPUBManifest &);

  // media-type, id, properties
  typedef std::tuple<std::string, std::string, std::string> ValueType_t;
  typedef std::unordered_map<std::string, ValueType_t> MapType_t;

public:
  EPUBManifest();

  void insert(const EPUBPath &path, const std::string &mimetype, const std::string &id, const std::string &properties);

  void writeTo(EPUBXMLContent &xml);

private:
  MapType_t m_map;
};

}

#endif // INCLUDED_EPUBMANIFEST_H

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
