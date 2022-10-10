/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_EPUBTABLESTYLEMANAGER_H
#define INCLUDED_EPUBTABLESTYLEMANAGER_H

#include <librevenge/librevenge.h>

#include <string>
#include <unordered_map>
#include <vector>

#include <boost/functional/hash.hpp>

#include "EPUBCSSProperties.h"

namespace libepubgen
{

class EPUBCSSContent;

/** Small class to manage the tables style */
class EPUBTableStyleManager
{
  typedef std::unordered_map<EPUBCSSProperties, std::string, boost::hash<EPUBCSSProperties>> ContentNameMap_t;

public:
  //! constructor
  EPUBTableStyleManager() : m_cellContentNameMap(), m_rowContentNameMap(), m_tableContentNameMap(), m_columnWidthsStack(), m_relColumnWidthsStack()
  {
  }
  //! destructor
  ~EPUBTableStyleManager()
  {
  }
  //! open a table
  void openTable(librevenge::RVNGPropertyList const &propList);
  //! close a table
  void closeTable();
  //! returns the class name corresponding to a propertylist
  std::string getCellClass(librevenge::RVNGPropertyList const &pList);
  //! returns the style string corresponding to a propertylist
  std::string getCellStyle(librevenge::RVNGPropertyList const &pList);
  //! returns the class name corresponding to a propertylist
  std::string getRowClass(librevenge::RVNGPropertyList const &pList);
  //! returns the style string corresponding to a propertylist
  std::string getRowStyle(librevenge::RVNGPropertyList const &pList);
  //! returns the class name corresponding to a propertylist
  std::string getTableClass(librevenge::RVNGPropertyList const &pList);
  //! returns the style string corresponding to a propertylist
  std::string getTableStyle(librevenge::RVNGPropertyList const &pList);
  //! send the data to the sink
  void send(EPUBCSSContent &out);
private:
  //! convert a property list into a CSS property map
  void extractCellProperties(librevenge::RVNGPropertyList const &pList, EPUBCSSProperties &cssProps) const;
  //! convert a property list into a CSS property map
  void extractRowProperties(librevenge::RVNGPropertyList const &pList, EPUBCSSProperties &cssProps) const;
  //! convert a property list into a CSS property map
  void extractTableProperties(librevenge::RVNGPropertyList const &pList, EPUBCSSProperties &cssProps) const;
  //! try to return the col width
  bool getColumnsWidth(int i, int numSpanned, double &w) const;
  //! try to return the relative col width
  bool getRelColumnsWidth(int i, int numSpanned, double &w) const;
  //! a map cell content -> name
  ContentNameMap_t m_cellContentNameMap;
  //! a map row content -> name
  ContentNameMap_t m_rowContentNameMap;
  //! a map table content -> name
  ContentNameMap_t m_tableContentNameMap;
  //! a stack of column width (in inches )
  std::vector<std::vector<double> > m_columnWidthsStack;
  //! a stack of relative column width (in percents )
  std::vector<std::vector<double> > m_relColumnWidthsStack;

  EPUBTableStyleManager(EPUBTableStyleManager const &orig);
  EPUBTableStyleManager operator=(EPUBTableStyleManager const &orig);
};

}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
