/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_EPUBHTMLMANAGER_H
#define INCLUDED_EPUBHTMLMANAGER_H

#include <memory>
#include <string>
#include <vector>

#include <librevenge/librevenge.h>

#include <libepubgen/libepubgen-decls.h>

#include "EPUBCounter.h"
#include "EPUBPath.h"
#include "EPUBXMLContent.h"

namespace libepubgen
{

class EPUBFontManager;
class EPUBHTMLGenerator;
class EPUBImageManager;
class EPUBListStyleManager;
class EPUBParagraphStyleManager;
class EPUBSpanStyleManager;
class EPUBTableStyleManager;
class EPUBManifest;
class EPUBPackage;

class EPUBHTMLManager
{
  // disable copying
  EPUBHTMLManager(const EPUBHTMLManager &);
  EPUBHTMLManager &operator=(const EPUBHTMLManager &);

public:
  explicit EPUBHTMLManager(EPUBManifest &manifest);

  const std::shared_ptr<EPUBHTMLGenerator> create(EPUBImageManager &imageManager, EPUBFontManager &fontManager, EPUBListStyleManager &listStyleManager, EPUBParagraphStyleManager &paragraphStyleManager, EPUBSpanStyleManager &spanStyleManager, EPUBSpanStyleManager &bodyStyleManager, EPUBTableStyleManager &tableStyleManager, const EPUBPath &stylesheetPath, EPUBStylesMethod stylesMethod, EPUBLayoutMethod layoutMethod, int version);

  void writeTo(EPUBPackage &package);

  void writeSpineTo(EPUBXMLContent &xml);
  void writeTocTo(EPUBXMLContent &xml, const EPUBPath &tocPath, int version, EPUBLayoutMethod layout);

  /// Appends text to the title of the current heading.
  void insertHeadingText(const std::string &text);

  /// Registers a chapter name for the current page (fixed layout case).
  void addChapterName(const std::string &text);

  /// If the current heading has a title.
  bool hasHeadingText() const;

private:
  EPUBManifest &m_manifest;
  std::vector<EPUBPath> m_paths;
  std::vector<EPUBXMLContent> m_contents;
  std::vector<std::string> m_ids;
  EPUBCounter m_number;
};

}

#endif // INCLUDED_EPUBHTMLMANAGER

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
