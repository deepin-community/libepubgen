/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_EPUBPARAGRAPHSTYLEMANAGER_H
#define INCLUDED_EPUBPARAGRAPHSTYLEMANAGER_H

#include <string>
#include <map>
#include <unordered_map>

#include <boost/functional/hash.hpp>

#include <librevenge/librevenge.h>

#include "EPUBCSSProperties.h"

namespace libepubgen
{

class EPUBCSSContent;

/** Small class to manage the paragraph style */
class EPUBParagraphStyleManager
{
  typedef std::unordered_map<EPUBCSSProperties, std::string, boost::hash<EPUBCSSProperties>> ContentNameMap_t;

public:
  //! constructor
  EPUBParagraphStyleManager() : m_contentNameMap(), m_idNameMap()
  {
  }
  //! destructor
  virtual ~EPUBParagraphStyleManager()
  {
  }
  //! define a paragraph style
  void defineParagraph(librevenge::RVNGPropertyList const &pList);
  //! returns the class name corresponding to a propertylist
  std::string getClass(librevenge::RVNGPropertyList const &pList);
  //! returns the style string corresponding to a propertylist
  std::string getStyle(librevenge::RVNGPropertyList const &pList);
  //! send the data to the sink
  void send(EPUBCSSContent &out);
protected:
  //! convert a property list into a CSS property map
  void extractProperties(librevenge::RVNGPropertyList const &pList, bool isList, EPUBCSSProperties &cssProps) const;
  //! a map content -> name
  ContentNameMap_t m_contentNameMap;
  //! a map id -> name
  std::map<int, std::string> m_idNameMap;
  //! add data corresponding to the border
  void extractBorders(librevenge::RVNGPropertyList const &pList, EPUBCSSProperties &cssProps) const;
private:
  EPUBParagraphStyleManager(EPUBParagraphStyleManager const &orig);
  EPUBParagraphStyleManager operator=(EPUBParagraphStyleManager const &orig);
};

}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
