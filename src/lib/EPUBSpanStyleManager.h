/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_EPUBSPANSTYLEMANAGER_H
#define INCLUDED_EPUBSPANSTYLEMANAGER_H

#include <map>
#include <string>
#include <unordered_map>

#include <boost/functional/hash.hpp>

#include <librevenge/librevenge.h>

#include "EPUBCSSProperties.h"
#include "EPUBCounter.h"

namespace libepubgen
{

class EPUBCSSContent;

/** Small class to manage the span style */
class EPUBSpanStyleManager
{
  typedef std::unordered_map<EPUBCSSProperties, std::string, boost::hash<EPUBCSSProperties>> ContentNameMap_t;

public:
  //! constructor
  EPUBSpanStyleManager(std::string classNamePrefix) : m_contentNameMap(), m_idNameMap(), m_classNamePrefix(classNamePrefix)
  {
  }
  //! destructor
  virtual ~EPUBSpanStyleManager()
  {
  }
  //! define a span style
  void defineSpan(librevenge::RVNGPropertyList const &pList);
  //! returns the class name corresponding to a propertylist
  std::string getClass(librevenge::RVNGPropertyList const &pList);
  //! returns the style string corresponding to a propertylist
  std::string getStyle(librevenge::RVNGPropertyList const &pList);
  //! send the data to the sink
  void send(EPUBCSSContent &out);
protected:
  //! convert a property list into a CSS property map
  virtual void extractProperties(librevenge::RVNGPropertyList const &pList, EPUBCSSProperties &cssProps) const;

private:
  //! add data corresponding to a text position into the map
  void extractTextPosition(char const *value, EPUBCSSProperties &cssProps) const;
  //! add data corresponding to the line decoration into the map
  void extractDecorations(librevenge::RVNGPropertyList const &pList, EPUBCSSProperties &cssProps) const;
protected:
  //! a map content -> name
  ContentNameMap_t m_contentNameMap;
  //! a map id -> name
  std::map<int, std::string> m_idNameMap;

  std::string m_classNamePrefix;

private:
  EPUBSpanStyleManager(EPUBSpanStyleManager const &orig);
  EPUBSpanStyleManager operator=(EPUBSpanStyleManager const &orig);
};

}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
