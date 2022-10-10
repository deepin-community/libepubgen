/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "EPUBManifest.h"

#include <cassert>

#include "EPUBPath.h"
#include "EPUBXMLContent.h"

namespace libepubgen
{

EPUBManifest::EPUBManifest()
  : m_map()
{
}

void EPUBManifest::insert(const EPUBPath &path, const std::string &mimetype, const std::string &id, const std::string &properties)
{
  if (!m_map.insert(MapType_t::value_type(path.relativeTo(EPUBPath("OEBPS/content.opf")).str(), ValueType_t(mimetype, id, properties))).second)
  {
    assert(!"duplicate entry!");
  }
}

void EPUBManifest::writeTo(EPUBXMLContent &xml)
{
  for (MapType_t::const_iterator it = m_map.begin(); m_map.end() != it; ++it)
  {
    librevenge::RVNGPropertyList attrs;
    attrs.insert("href", it->first.c_str());
    attrs.insert("media-type", std::get<0>(it->second).c_str());
    attrs.insert("id", std::get<1>(it->second).c_str());
    const std::string &properties = std::get<2>(it->second);
    if (!properties.empty())
      attrs.insert("properties", properties.c_str());
    xml.insertEmptyElement("item", attrs);
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
