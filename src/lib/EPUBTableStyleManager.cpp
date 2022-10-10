/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "EPUBTableStyleManager.h"

#include <numeric>
#include <sstream>

#include <librevenge/librevenge.h>

#include "libepubgen_utils.h"
#include "EPUBCSSContent.h"

namespace libepubgen
{

using librevenge::RVNGPropertyList;
using librevenge::RVNGPropertyListVector;
using librevenge::RVNGString;

void EPUBTableStyleManager::openTable(RVNGPropertyList const &propList)
{
  const librevenge::RVNGPropertyListVector *columns = propList.child("librevenge:table-columns");
  if (columns)
  {
    std::vector<double> colWidths;
    std::vector<double> relColWidths;
    for (unsigned long i = 0; i < columns->count(); i++)
    {
      RVNGPropertyList const &prop=(*columns)[i];
      double width=0;
      if (prop["style:column-width"])
      {
        librevenge::RVNGUnit unit=prop["style:column-width"]->getUnit();
        if (unit==librevenge::RVNG_POINT)
          width=prop["style:column-width"]->getDouble()/72.;
        else if (unit==librevenge::RVNG_INCH)
          width=prop["style:column-width"]->getDouble();
        else if (unit==librevenge::RVNG_TWIP)
          width=prop["style:column-width"]->getDouble()/1440.;
      }
      colWidths.push_back(width);

      if (prop["style:rel-column-width"])
      {
        width = prop["style:rel-column-width"]->getDouble();
        relColWidths.push_back(width);
      }
    }
    m_columnWidthsStack.push_back(colWidths);
    m_relColumnWidthsStack.push_back(relColWidths);
  }
}

void EPUBTableStyleManager::closeTable()
{
  if (!m_columnWidthsStack.size())
  {
    EPUBGEN_DEBUG_MSG(("EPUBTableStyleManager::closeTable: can not find the columns width\n"));
    return;
  }
  m_columnWidthsStack.pop_back();
}

namespace
{
bool extractColumnsWidth(const std::vector< std::vector<double> > &columnWidthsStack, int col, int numSpanned, bool relative, double &w)
{
  if (!columnWidthsStack.size())
    return false;
  std::vector<double> const &widths=columnWidthsStack.back();
  double total = std::accumulate(widths.begin(), widths.end(), static_cast<double>(0));
  if (col < 0 || size_t(col+numSpanned-1) >= widths.size())
  {
    if (!relative)
    {
      EPUBGEN_DEBUG_MSG(("EPUBTableStyleManager::getColumnsWidth: can not compute the columns width\n"));
    }
    return false;
  }
  bool fixed = true;
  w = 0;
  for (auto i=size_t(col); i < size_t(col+numSpanned); i++)
  {
    if (widths[i] < 0)
    {
      w += -widths[i];
      fixed = false;
    }
    else if (widths[i] > 0)
      w += widths[i];
    else
    {
      w = 0;
      return true;
    }
  }
  if (!fixed) w = -w;
  if (relative)
    // Expected unit is percents.
    w = w * 100 / total;
  return true;
}
}

bool EPUBTableStyleManager::getColumnsWidth(int col, int numSpanned, double &w) const
{
  return extractColumnsWidth(m_columnWidthsStack, col, numSpanned, false, w);
}

bool EPUBTableStyleManager::getRelColumnsWidth(int col, int numSpanned, double &w) const
{
  return extractColumnsWidth(m_relColumnWidthsStack, col, numSpanned, true, w);
}

std::string EPUBTableStyleManager::getCellClass(RVNGPropertyList const &pList)
{
  EPUBCSSProperties content;
  extractCellProperties(pList, content);
  ContentNameMap_t::const_iterator it=m_cellContentNameMap.find(content);
  if (it != m_cellContentNameMap.end())
    return it->second;
  std::stringstream s;
  s << "cellTable" << m_cellContentNameMap.size();
  m_cellContentNameMap[content]=s.str();
  return s.str();
}

std::string EPUBTableStyleManager::getCellStyle(RVNGPropertyList const &pList)
{
  EPUBCSSProperties content;
  extractCellProperties(pList, content);

  std::stringstream s;
  for (const auto &property : content)
    s << property.first << ": " << property.second << "; ";
  return s.str();
}

std::string EPUBTableStyleManager::getRowClass(RVNGPropertyList const &pList)
{
  EPUBCSSProperties content;
  extractRowProperties(pList, content);
  ContentNameMap_t::const_iterator it=m_rowContentNameMap.find(content);
  if (it != m_rowContentNameMap.end())
    return it->second;
  std::stringstream s;
  s << "rowTable" << m_rowContentNameMap.size();
  m_rowContentNameMap[content]=s.str();
  return s.str();
}

std::string EPUBTableStyleManager::getRowStyle(RVNGPropertyList const &pList)
{
  EPUBCSSProperties content;
  extractRowProperties(pList, content);

  std::stringstream s;
  for (const auto &property : content)
    s << property.first << ": " << property.second << "; ";
  return s.str();
}

std::string EPUBTableStyleManager::getTableClass(RVNGPropertyList const &pList)
{
  EPUBCSSProperties content;
  extractTableProperties(pList, content);
  ContentNameMap_t::const_iterator it=m_tableContentNameMap.find(content);
  if (it != m_tableContentNameMap.end())
    return it->second;
  std::stringstream s;
  s << "table" << m_tableContentNameMap.size();
  m_tableContentNameMap[content]=s.str();
  return s.str();
}

std::string EPUBTableStyleManager::getTableStyle(RVNGPropertyList const &pList)
{
  EPUBCSSProperties content;
  extractTableProperties(pList, content);

  std::stringstream s;
  for (const auto &property : content)
    s << property.first << ": " << property.second << "; ";
  return s.str();
}

void EPUBTableStyleManager::send(EPUBCSSContent &out)
{
  for (ContentNameMap_t::const_iterator it=m_cellContentNameMap.begin(); m_cellContentNameMap.end() != it; ++it)
  {
    RVNGPropertyList props;
    fillPropertyList(it->first, props);
    out.insertRule(("." + it->second).c_str(), props);
  }

  for (ContentNameMap_t::const_iterator it=m_rowContentNameMap.begin(); m_rowContentNameMap.end() != it; ++it)
  {
    RVNGPropertyList props;
    fillPropertyList(it->first, props);
    out.insertRule(("." + it->second).c_str(), props);
  }

  for (ContentNameMap_t::const_iterator it=m_tableContentNameMap.begin(); m_tableContentNameMap.end() != it; ++it)
  {
    RVNGPropertyList props;
    fillPropertyList(it->first, props);
    out.insertRule(("." + it->second).c_str(), props);
  }
}

void EPUBTableStyleManager::extractCellProperties(RVNGPropertyList const &pList, EPUBCSSProperties &cssProps) const
{
  // try to get the cell width
  if (pList["librevenge:column"])
  {
    int c=pList["librevenge:column"]->getInt();
    int span=1;
    if (pList["table:number-columns-spanned"])
      span = pList["table:number-columns-spanned"]->getInt();
    double w;
    if (!getColumnsWidth(c,span,w))
    {
      EPUBGEN_DEBUG_MSG(("EPUBTableStyleManager::getCellContent: can not find columns witdth for %d[%d]\n", c, span));
    }
    else if (w > 0)
    {
      std::ostringstream width;
      width << w << "in";
      cssProps["width"] = width.str();
    }
    else if (w < 0)
    {
      std::ostringstream width;
      width << -w << "in";
      cssProps["min-width"] = width.str();
    }

    if (getRelColumnsWidth(c, span, w))
    {
      std::ostringstream width;
      width << w << "%";
      cssProps["width"] = width.str();
    }
  }
  if (pList["fo:text-align"])
  {
    if (pList["fo:text-align"]->getStr() == RVNGString("end")) // stupid OOo convention..
      cssProps["text-align"] = "right";
    else
      cssProps["text-align"] = pList["fo:text-align"]->getStr().cstr();
  }
  const librevenge::RVNGProperty *verticalAlign = pList["style:vertical-align"];
  if (verticalAlign && !verticalAlign->getStr().empty())
    cssProps["vertical-align"] = verticalAlign->getStr().cstr();
  else
    cssProps["vertical-align"] = "top";
  if (pList["fo:background-color"])
    cssProps["background-color"] = pList["fo:background-color"]->getStr().cstr();

  static char const *type[] = {"border", "border-left", "border-top", "border-right", "border-bottom" };
  for (auto &i : type)
  {
    std::string field("fo:");
    field+=i;
    if (!pList[field.c_str()])
      continue;
    cssProps[i] = pList[field.c_str()]->getStr().cstr();
  }
}

void EPUBTableStyleManager::extractRowProperties(RVNGPropertyList const &pList, EPUBCSSProperties &cssProps) const
{
  if (pList["style:min-row-height"])
    cssProps["min-height"] = pList["style:min-row-height"]->getStr().cstr();
  else if (pList["style:row-height"])
    cssProps["height"] = pList["style:row-height"]->getStr().cstr();
}

void EPUBTableStyleManager::extractTableProperties(RVNGPropertyList const &pList, EPUBCSSProperties &cssProps) const
{
  if (pList["style:rel-width"])
    cssProps["width"] = pList["style:rel-width"]->getStr().cstr();
  else if (pList["style:width"])
    cssProps["width"] = pList["style:width"]->getStr().cstr();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
