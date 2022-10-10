/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "EPUBSpanStyleManager.h"

#include <cstring>
#include <sstream>

#include <librevenge/librevenge.h>

#include "libepubgen_utils.h"
#include "EPUBCSSContent.h"

namespace libepubgen
{

using librevenge::RVNGPropertyList;

std::string EPUBSpanStyleManager::getClass(RVNGPropertyList const &pList)
{
  if (pList["librevenge:span-id"])
  {
    int id=pList["librevenge:span-id"]->getInt();
    if (m_idNameMap.find(id)!=m_idNameMap.end())
      return m_idNameMap.find(id)->second;
  }

  EPUBCSSProperties content;

  extractProperties(pList, content);

  ContentNameMap_t::const_iterator it = m_contentNameMap.find(content);
  if (it != m_contentNameMap.end())
    return it->second;

  std::stringstream s;
  s << m_classNamePrefix << m_contentNameMap.size();

  m_contentNameMap[content]=s.str();
  return s.str();
}

std::string EPUBSpanStyleManager::getStyle(RVNGPropertyList const &pList)
{
  EPUBCSSProperties content;
  extractProperties(pList, content);

  std::stringstream s;
  for (const auto &property : content)
    s << property.first << ": " << property.second << "; ";
  return s.str();
}

void EPUBSpanStyleManager::defineSpan(RVNGPropertyList const &propList)
{
  if (!propList["librevenge:span-id"])
  {
    EPUBGEN_DEBUG_MSG(("EPUBSpanStyleManager::defineStyle: can not find the span id\n"));
    return;

  }
  int id=propList["librevenge:span-id"]->getInt();

  RVNGPropertyList pList(propList);
  pList.remove("librevenge:span-id");
  m_idNameMap[id]=getClass(pList);
}

void EPUBSpanStyleManager::send(EPUBCSSContent &out)
{
  for (ContentNameMap_t::const_iterator it=m_contentNameMap.begin(); m_contentNameMap.end() != it; ++it)
  {
    RVNGPropertyList props;
    fillPropertyList(it->first, props);
    out.insertRule(("." + it->second).c_str(), props);
  }
}

void EPUBSpanStyleManager::extractProperties(RVNGPropertyList const &pList, EPUBCSSProperties &cssProps) const
{
  if (pList["fo:background-color"])
    cssProps["background-color"] = pList["fo:background-color"]->getStr().cstr();
  if (pList["fo:color"])
    cssProps["color"] = pList["fo:color"]->getStr().cstr();
  if (pList["fo:font-size"])
    cssProps["font-size"] = pList["fo:font-size"]->getStr().cstr();
  if (pList["fo:font-style"])
    cssProps["font-style"] = pList["fo:font-style"]->getStr().cstr();
  if (pList["fo:font-variant"])
    cssProps["font-variant"] = pList["fo:font-variant"]->getStr().cstr();
  if (pList["fo:font-weight"])
    cssProps["font-weight"] = pList["fo:font-weight"]->getStr().cstr();
  if (pList["fo:letter-spacing"])
    cssProps["letter-spacing"] = pList["fo:letter-spacing"]->getStr().cstr();
  if (pList["fo:text-shadow"])
    cssProps["text-shadow"] = "1px 1px 1px #666666";
  if (pList["fo:text-transform"])
    cssProps["text-transform"] = pList["fo:text-transform"]->getStr().cstr();

  if (pList["style:font-name"])
  {
    std::ostringstream name;
    name << '\'' << pList["style:font-name"]->getStr().cstr() << '\'';
    cssProps["font-family"] = name.str();
  }
  if (pList["style:text-blinking"])
    cssProps["text-decoration"] = "blink";
  extractDecorations(pList, cssProps);
  if (pList["style:text-position"])
    extractTextPosition(pList["style:text-position"]->getStr().cstr(), cssProps);

  if (pList["text:display"])
    cssProps["display"] = pList["text:display"]->getStr().cstr();

  // checkme not working with Safari 6.02...
  if (pList["style:font-relief"] && pList["style:font-relief"]->getStr().cstr())
  {
    if (strcmp(pList["style:font-relief"]->getStr().cstr(),"embossed")==0)
      cssProps["font-effect"] = "emboss";
    else if (strcmp(pList["style:font-relief"]->getStr().cstr(),"engraved")==0)
      cssProps["font-effect"] = "engrave";
  }
  if (pList["style:text-outline"])
    cssProps["font-effect"] = "outline";

  if (pList["style:text-scale"])
  {
    if (pList["style:text-scale"]->getDouble() < 0.2)
      cssProps["font-stretch"] = "ultra-condensed";
    else if (pList["style:text-scale"]->getDouble() < 0.4)
      cssProps["font-stretch"] = "extra-condensed";
    else if (pList["style:text-scale"]->getDouble() < 0.6)
      cssProps["font-stretch"] = "condensed";
    else if (pList["style:text-scale"]->getDouble() < 0.8)
      cssProps["font-stretch"] = "semi-condensed";
    else if (pList["style:text-scale"]->getDouble() > 2.0)
      cssProps["font-stretch"] = "ultra-expanded";
    else if (pList["style:text-scale"]->getDouble() > 1.6)
      cssProps["font-stretch"] = "extra-expanded";
    else if (pList["style:text-scale"]->getDouble() > 1.4)
      cssProps["font-stretch"] = "expanded";
    else if (pList["style:text-scale"]->getDouble() > 1.2)
      cssProps["font-stretch"] = "semi-expanded";
  }
}

void EPUBSpanStyleManager::extractDecorations(RVNGPropertyList const &pList, EPUBCSSProperties &cssProps) const
{
  // replaceme by text-decoration-line when its implementation will appear in browser
  std::stringstream s;

  // line-though style or type 'none' is not line-though, everything else is.
  const librevenge::RVNGProperty *textLineThoughStyle = pList["style:text-line-through-style"];
  bool lineThough = textLineThoughStyle && textLineThoughStyle->getStr() != "none";
  if (!lineThough)
  {
    const librevenge::RVNGProperty *textLineThoughType = pList["style:text-line-through-type"];
    lineThough = textLineThoughType && textLineThoughType->getStr() != "none";
  }
  if (lineThough)
    s << " line-through";

  if (pList["style:text-overline-style"] || pList["style:text-overline-type"])
    s << " overline";
  const librevenge::RVNGProperty *textUnderlineStyle = pList["style:text-underline-style"];
  bool underline = textUnderlineStyle && textUnderlineStyle->getStr() != "none";
  if (!underline)
  {
    const librevenge::RVNGProperty *textUnderlineType = pList["style:text-underline-type"];
    underline = textUnderlineType && textUnderlineType->getStr() != "none";
  }
  if (underline)
    s << " underline";
  if (s.str().length())
    cssProps["text-decoration"] = s.str();
}

void EPUBSpanStyleManager::extractTextPosition(char const *value, EPUBCSSProperties &cssProps) const
{
  if (!value) return;
  // first try to retrieve the position
  std::stringstream s("");
  double pos=0;
  if (strncmp(value,"super", 5)==0)
  {
    pos=33;
    s << value+5;
  }
  else if (strncmp(value,"sub", 3)==0)
  {
    pos=-33;
    s << value+3;
  }
  else
  {
    s << value;
    s >> pos;
    if (!s.good())
    {
      EPUBGEN_DEBUG_MSG(("EPUBSpanStyleManager::parseTextPosition: can not read %s\n", value));
      return;
    }
    if (s.peek()=='%')
    {
      char c;
      s >> c;
    }
  }
  double percent;
  s >> percent;
  if (!s.good() || percent <= 0 || percent > 100)
  {
    EPUBGEN_DEBUG_MSG(("EPUBSpanStyleManager::parseTextPosition: can not read percent: find %s\n", s.str().c_str()));
    percent=100;
  }
  if (pos <= 0 && pos >= 0) return;
  cssProps["vertical-align"] = "baseline";
  cssProps["position"] = "relative";

  std::ostringstream top;
  top << (-pos/100.) << "em";
  cssProps["top"] = top.str();
  if (percent < 100)
  {
    std::ostringstream fontSize;
    fontSize << (percent/100.) << "em";
    cssProps["font-size"] = fontSize.str();
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
