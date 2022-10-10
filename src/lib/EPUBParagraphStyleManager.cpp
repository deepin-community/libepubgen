/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "EPUBParagraphStyleManager.h"

#include <sstream>

#include <librevenge/librevenge.h>

#include "libepubgen_utils.h"
#include "EPUBCSSContent.h"

namespace libepubgen
{

using librevenge::RVNGPropertyList;
using librevenge::RVNGString;

std::string EPUBParagraphStyleManager::getClass(RVNGPropertyList const &pList)
{
  if (pList["librevenge:paragraph-id"])
  {
    int id=pList["librevenge:paragraph-id"]->getInt();
    if (m_idNameMap.find(id)!=m_idNameMap.end())
      return m_idNameMap.find(id)->second;
  }
  EPUBCSSProperties content;
  extractProperties(pList, false, content);
  ContentNameMap_t::const_iterator it=m_contentNameMap.find(content);
  if (it != m_contentNameMap.end())
    return it->second;
  std::stringstream s;
  s << "para" << m_contentNameMap.size();
  m_contentNameMap[content]=s.str();
  return s.str();
}

std::string EPUBParagraphStyleManager::getStyle(RVNGPropertyList const &pList)
{
  EPUBCSSProperties content;
  extractProperties(pList, false, content);

  std::stringstream s;
  for (const auto &property : content)
    s << property.first << ": " << property.second << "; ";
  return s.str();
}

void EPUBParagraphStyleManager::defineParagraph(RVNGPropertyList const &propList)
{
  if (!propList["librevenge:paragraph-id"])
  {
    EPUBGEN_DEBUG_MSG(("EPUBParagraphStyleManager::defineStyle: can not find the paragraph id\n"));
    return;
  }
  int id=propList["librevenge:paragraph-id"]->getInt();
  RVNGPropertyList pList(propList);
  pList.remove("librevenge:paragraph-id");
  m_idNameMap[id]=getClass(pList);
}

void EPUBParagraphStyleManager::send(EPUBCSSContent &out)
{
  for (ContentNameMap_t::const_iterator it=m_contentNameMap.begin(); m_contentNameMap.end() != it; ++it)
  {
    RVNGPropertyList props;
    fillPropertyList(it->first, props);
    out.insertRule(("." + it->second).c_str(), props);
  }
}

void EPUBParagraphStyleManager::extractProperties(RVNGPropertyList const &pList, bool isList, EPUBCSSProperties &cssProps) const
{
  if (pList["fo:text-align"])
  {
    if (pList["fo:text-align"]->getStr() == librevenge::RVNGString("end")) // stupid OOo convention..
      cssProps["text-align"] = "right";
    else
      cssProps["text-align"] = pList["fo:text-align"]->getStr().cstr();
  }

  {
    // the margins
    std::ostringstream s;
    if (pList["fo:margin-top"])
      s << " " << pList["fo:margin-top"]->getStr().cstr();
    else
      s << " 0px";
    if (pList["fo:margin-right"])
      s << " " << pList["fo:margin-right"]->getStr().cstr();
    else
      s << " 0px";
    if (pList["fo:margin-bottom"])
      s << " " << pList["fo:margin-bottom"]->getStr().cstr();
    else
      s << " 0px";
    if (isList)
    {
      double val=0;
      if (pList["fo:margin-left"])
      {
        librevenge::RVNGUnit unit=pList["fo:margin-left"]->getUnit();
        if (unit==librevenge::RVNG_POINT) val=pList["fo:margin-left"]->getDouble();
        else if (unit==librevenge::RVNG_INCH) val=pList["fo:margin-left"]->getDouble()*72.;
        else if (unit==librevenge::RVNG_TWIP) val=pList["fo:margin-left"]->getDouble()*20.;
      }
      if (pList["fo:text-indent"])
      {
        librevenge::RVNGUnit unit=pList["fo:text-indent"]->getUnit();
        if (unit==librevenge::RVNG_POINT) val+=pList["fo:text-indent"]->getDouble();
        else if (unit==librevenge::RVNG_INCH) val+=pList["fo:text-indent"]->getDouble()*72.;
        else if (unit==librevenge::RVNG_TWIP) val+=pList["fo:text-indent"]->getDouble()*20.;
      }
      val -= 10; // checkme: seems to big, so decrease it
      s << " " << val << "px";
    }
    else if (pList["fo:margin-left"])
      s << " " << pList["fo:margin-left"]->getStr().cstr();
    else
      s << " 0px";

    cssProps["margin"] = s.str();
  }

  if (pList["fo:text-indent"])
  {
    cssProps["text-indent"] = pList["fo:text-indent"]->getStr().cstr();
    if (isList && pList["fo:text-indent"]->getStr().cstr()[0]=='-')
      cssProps["padding-left"] = pList["fo:text-indent"]->getStr().cstr()+1;
  }
  // line height
  if (pList["fo:line-height"] && (pList["fo:line-height"]->getDouble()<0.999||pList["fo:line-height"]->getDouble()>1.001))
    cssProps["line-height"] = pList["fo:line-height"]->getStr().cstr();
  if (pList["style:line-height-at-least"] && (pList["style:line-height-at-least"]->getDouble()<0.999||pList["style:line-height-at-least"]->getDouble()>1.001))
    cssProps["min-height"] = pList["style:line-height-at-least"]->getStr().cstr();
  // other: background, border
  if (pList["fo:background-color"])
    cssProps["background-color"] = pList["fo:background-color"]->getStr().cstr();

  extractBorders(pList, cssProps);
}

void EPUBParagraphStyleManager::extractBorders(RVNGPropertyList const &pList, EPUBCSSProperties &cssProps) const
{
  static char const *type[] = {"border", "border-left", "border-top", "border-right", "border-bottom" };
  for (int i = 0; i < 5; i++)
  {
    std::string field("fo:");
    field+=type[i];
    if (!pList[field.c_str()])
      continue;
    cssProps[type[i]] =  pList[field.c_str()]->getStr().cstr();
    // does not seems to works with negative text-indent, so add a padding
    if (i<=1 && pList["fo:text-indent"] && pList["fo:text-indent"]->getDouble()<0 &&
        pList["fo:text-indent"]->getStr().cstr()[0]=='-')
      cssProps["padding-left"] = pList["fo:text-indent"]->getStr().cstr()+1;
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
