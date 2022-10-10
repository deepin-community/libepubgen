/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "EPUBListStyleManager.h"

#include <cstring>
#include <sstream>

#include <librevenge/librevenge.h>

#include "libepubgen_utils.h"
#include "EPUBCSSContent.h"

namespace libepubgen
{

using librevenge::RVNGPropertyList;

void EPUBListStyleManager::List::setLevel(int lvl, librevenge::RVNGPropertyList const &pList, bool ordered)
{
  if (lvl < 0 || lvl > 30)
  {
    EPUBGEN_DEBUG_MSG(("EPUBListStyleManager::List::setLevel: level is %d\n", lvl));
    return;
  }
  if (size_t(lvl) >= m_contentsList.size())
    m_contentsList.resize(size_t(lvl)+1);

  EPUBCSSProperties cssProps;

  // fixme: read text:space-before, text:min-label-width, text:min-label-distance
  if (!ordered)
  {
    // fixme read text:bullet-char
  }
  else
  {
    // fixme read style:num-prefix, style:num-suffix, text:start-value
    if (pList["style:num-format"])
    {
      if (strcmp(pList["style:num-format"]->getStr().cstr(),"a")==0)
        cssProps["list-style-type"] = "lower-alpha";
      else if (strcmp(pList["style:num-format"]->getStr().cstr(),"A")==0)
        cssProps["list-style-type"] = "upper-alpha";
      else if (strcmp(pList["style:num-format"]->getStr().cstr(),"i")==0)
        cssProps["list-style-type"] = "lower-roman";
      else if (strcmp(pList["style:num-format"]->getStr().cstr(),"I")==0)
        cssProps["list-style-type"] = "upper-roman";
      else if (strcmp(pList["style:num-format"]->getStr().cstr(),"1")==0)
        cssProps["list-style-type"] = "decimal";
      else
      {
        EPUBGEN_DEBUG_MSG(("EPUBListStyleManager::List::setLevel: unknown format %s\n",
                           pList["style:num-format"]->getStr().cstr()));
      }
    }
  }
  m_contentsList[size_t(lvl)] = cssProps;
}

const EPUBCSSProperties &EPUBListStyleManager::List::getCurrentProperties() const
{
  if (m_level < 0 || size_t(m_level) >= m_contentsList.size())
  {
    EPUBGEN_DEBUG_MSG(("EPUBListStyleManager::List::str: level %d is not set\n", m_level));
    static EPUBCSSProperties dummy;
    return dummy;
  }
  return m_contentsList[size_t(m_level)];
}

void EPUBListStyleManager::send(EPUBCSSContent &out)
{
  EPUBParagraphStyleManager::send(out);

  for (List::ContentNameMap_t::const_iterator it=m_levelNameMap.begin(); m_levelNameMap.end() != it; ++it)
  {
    RVNGPropertyList props;
    fillPropertyList(it->first, props);
    out.insertRule(("." + it->second).c_str(), props);
  }
}

std::string EPUBListStyleManager::getClass(RVNGPropertyList const &pList)
{
  EPUBCSSProperties content;
  extractProperties(pList, true, content);
  List::ContentNameMap_t::const_iterator it = m_contentNameMap.find(content);
  if (it != m_contentNameMap.end())
    return it->second;
  std::stringstream s;
  s << "listElt" << m_contentNameMap.size();
  m_contentNameMap[content]=s.str();
  return s.str();
}

void EPUBListStyleManager::defineLevel(RVNGPropertyList const &pList, bool ordered)
{
  int id=-1;
  if (pList["librevenge:list-id"])
    id=pList["librevenge:list-id"]->getInt();
  if (m_idListMap.find(id)==m_idListMap.end())
    m_idListMap[id]=List();
  if (!pList["librevenge:level"])
  {
    EPUBGEN_DEBUG_MSG(("EPUBListStyleManager::defineLevel: can not find list level\n"));
    return;
  }
  m_idListMap.find(id)->second.setLevel(pList["librevenge:level"]->getInt(), pList, ordered);
}

std::string EPUBListStyleManager::openLevel(RVNGPropertyList const &pList, bool ordered)
{
  int id = -1;
  if (pList["librevenge:list-id"])
    id=pList["librevenge:list-id"]->getInt();
  else // anomynous list
    defineLevel(pList, ordered);
  m_actualIdStack.push_back(id);

  EPUBCSSProperties content;
  extractProperties(pList, true, content);
  if (id >= 0 && m_idListMap.find(id)!=m_idListMap.end())
  {
    m_idListMap.find(id)->second.openLevel();
    const EPUBCSSProperties &levelProps = m_idListMap.find(id)->second.getCurrentProperties();
    content.insert(levelProps.begin(), levelProps.end());
  }
  List::ContentNameMap_t::const_iterator it = m_levelNameMap.find(content);
  if (it != m_levelNameMap.end())
    return it->second;
  std::stringstream s;
  s << "listLevel" << m_levelNameMap.size();
  m_levelNameMap[content]=s.str();

  return s.str();
}

void EPUBListStyleManager::closeLevel()
{
  if (!m_actualIdStack.size())
  {
    EPUBGEN_DEBUG_MSG(("EPUBListStyleManager::closeLevel: can not find level\n"));
    return;
  }
  int id=m_actualIdStack.back();
  if (id >= 0 && m_idListMap.find(id)!=m_idListMap.end())
    m_idListMap.find(id)->second.closeLevel();

  m_actualIdStack.pop_back();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
