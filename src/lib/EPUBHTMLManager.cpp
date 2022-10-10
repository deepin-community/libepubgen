/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "EPUBHTMLManager.h"

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <sstream>

#include "EPUBHTMLGenerator.h"
#include "EPUBManifest.h"

namespace libepubgen
{

namespace
{

/// Extracts a title string from a path and provides a fallback if it would be empty.
void getPathTitle(std::ostringstream &label, const EPUBPath &path, EPUBLayoutMethod layout, std::vector<EPUBPath>::size_type index)
{
  if (path.getTitle().empty())
  {
    if (layout == EPUB_LAYOUT_METHOD_FIXED)
      label << "Page ";
    else
      label << "Section ";
    label << (index + 1);
  }
  else
    label << path.getTitle();
}

}

EPUBHTMLManager::EPUBHTMLManager(EPUBManifest &manifest)
  : m_manifest(manifest)
  , m_paths()
  , m_contents()
  , m_ids()
  , m_number()
{
}

const std::shared_ptr<EPUBHTMLGenerator> EPUBHTMLManager::create(EPUBImageManager &imageManager, EPUBFontManager &fontManager, EPUBListStyleManager &listStyleManager, EPUBParagraphStyleManager &paragraphStyleManager, EPUBSpanStyleManager &spanStyleManager, EPUBSpanStyleManager &bodyStyleManager, EPUBTableStyleManager &tableStyleManager, const EPUBPath &stylesheetPath, EPUBStylesMethod stylesMethod, EPUBLayoutMethod layoutMethod, int version)
{
  std::ostringstream nameBuf;
  nameBuf << "section" << std::setw(4) << std::setfill('0') << m_number.next();
  m_ids.push_back(nameBuf.str());

  nameBuf << ".xhtml";
  m_paths.push_back(EPUBPath("OEBPS/sections") / nameBuf.str());

  m_manifest.insert(m_paths.back(), "application/xhtml+xml", m_ids.back(), "");

  m_contents.push_back(EPUBXMLContent());

  const std::shared_ptr<EPUBHTMLGenerator> gen(
    new EPUBHTMLGenerator(m_contents.back(), imageManager, fontManager, listStyleManager, paragraphStyleManager, spanStyleManager, bodyStyleManager, tableStyleManager, m_paths.back(), stylesheetPath, stylesMethod, layoutMethod, version));

  return gen;
}

void EPUBHTMLManager::writeTo(EPUBPackage &package)
{
  assert(m_contents.size() == m_paths.size());

  std::vector<EPUBPath>::const_iterator pathIt = m_paths.begin();
  auto contentIt = m_contents.begin();

  for (; (m_paths.end() != pathIt) && (m_contents.end() != contentIt); ++pathIt, ++contentIt)
    contentIt->writeTo(package, pathIt->str().c_str());
}

void EPUBHTMLManager::writeSpineTo(EPUBXMLContent &xml)
{
  for (std::vector<std::string>::const_iterator it = m_ids.begin(); m_ids.end() != it; ++it)
  {
    librevenge::RVNGPropertyList itemrefAttrs;
    itemrefAttrs.insert("idref", it->c_str());
    xml.insertEmptyElement("itemref", itemrefAttrs);
  }
}

void EPUBHTMLManager::writeTocTo(EPUBXMLContent &xml, const EPUBPath &tocPath, int version, EPUBLayoutMethod layout)
{
  if (version >= 30)
  {
    bool hasChapterNames = std::find_if(m_paths.begin(), m_paths.end(), [](const EPUBPath &path)
    {
      return !path.getChapters().empty();
    }) != m_paths.end();
    for (std::vector<EPUBPath>::size_type i = 0; m_paths.size() != i; ++i)
    {
      const std::vector<std::string> &chapters = m_paths[i].getChapters();
      for (const auto &chapter : chapters)
      {
        xml.openElement("li");
        librevenge::RVNGPropertyList anchorAttrs;
        anchorAttrs.insert("href", m_paths[i].relativeTo(tocPath).str().c_str());
        xml.openElement("a", anchorAttrs);
        std::ostringstream label;
        xml.insertCharacters(chapter.c_str());
        xml.closeElement("a");
        xml.closeElement("li");
      }
      if (hasChapterNames)
        // Chapter names are provided for this document, so never write Page
        // <N> entries.
        continue;

      xml.openElement("li");
      librevenge::RVNGPropertyList anchorAttrs;
      anchorAttrs.insert("href", m_paths[i].relativeTo(tocPath).str().c_str());
      xml.openElement("a", anchorAttrs);
      std::ostringstream label;
      getPathTitle(label, m_paths[i], layout, i);
      xml.insertCharacters(label.str().c_str());
      xml.closeElement("a");
      xml.closeElement("li");
    }

    return;
  }

  librevenge::RVNGPropertyList navPointAttrs;
  for (std::vector<EPUBPath>::size_type i = 0; m_paths.size() != i; ++i)
  {
    std::ostringstream id;
    id << "section" << (i + 1);
    navPointAttrs.insert("id", id.str().c_str());
    navPointAttrs.insert("class", "document");
    std::ostringstream playOrder;
    playOrder << (i + 1);
    navPointAttrs.insert("playOrder", playOrder.str().c_str());
    xml.openElement("navPoint", navPointAttrs);
    xml.openElement("navLabel");
    xml.openElement("text");
    std::ostringstream label;
    getPathTitle(label, m_paths[i], layout, i);
    xml.insertCharacters(label.str().c_str());
    xml.closeElement("text");
    xml.closeElement("navLabel");
    librevenge::RVNGPropertyList contentAttrs;
    contentAttrs.insert("src", m_paths[i].relativeTo(tocPath).str().c_str());
    xml.insertEmptyElement("content", contentAttrs);
    xml.closeElement("navPoint");
  }
}

void EPUBHTMLManager::insertHeadingText(const std::string &text)
{
  if (m_paths.empty())
    return;

  m_paths.back().appendTitle(text);
}

void EPUBHTMLManager::addChapterName(const std::string &text)
{
  if (m_paths.empty())
    return;

  m_paths.back().addChapter(text);
}

bool EPUBHTMLManager::hasHeadingText() const
{
  if (m_paths.empty())
    return false;

  return !m_paths.back().getTitle().empty();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
