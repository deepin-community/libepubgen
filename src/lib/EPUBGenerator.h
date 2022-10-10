/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_EPUBGENERATOR_H
#define INCLUDED_EPUBGENERATOR_H

#include <memory>

#include "EPUBFontManager.h"
#include "EPUBHTMLManager.h"
#include "EPUBImageManager.h"
#include "EPUBListStyleManager.h"
#include "EPUBManifest.h"
#include "EPUBParagraphStyleManager.h"
#include "EPUBPath.h"
#include "EPUBSpanStyleManager.h"
#include "EPUBBodyStyleManager.h"
#include "EPUBSplitGuard.h"
#include "EPUBTableStyleManager.h"

namespace libepubgen
{

class EPUBHTMLGenerator;
class EPUBPackage;

class EPUBGenerator
{
  // disable copying
  EPUBGenerator(const EPUBGenerator &);
  EPUBGenerator &operator=(const EPUBGenerator &);

public:
  EPUBGenerator(EPUBPackage *package, int version);
  virtual ~EPUBGenerator();

  void startDocument(const librevenge::RVNGPropertyList &props);
  void endDocument();

  void setDocumentMetaData(const librevenge::RVNGPropertyList &props);

  void startNewHtmlFile();

  const std::shared_ptr<EPUBHTMLGenerator> &getHtml() const;

  EPUBHTMLManager &getHtmlManager();

  const EPUBSplitGuard &getSplitGuard() const;
  EPUBSplitGuard &getSplitGuard();
  int getVersion() const;

  void setSplitMethod(EPUBSplitMethod splitMethod);

  void setStylesMethod(EPUBStylesMethod stylesMethod);

  void setLayoutMethod(EPUBLayoutMethod layoutMethod);

private:
  virtual void startHtmlFile() = 0;
  virtual void endHtmlFile() = 0;

private:
  void writeContainer();
  void writeNavigation();
  void writeStylesheet();
  void writeRoot();

private:
  EPUBPackage *m_package;
  EPUBManifest m_manifest;
  EPUBHTMLManager m_htmlManager;
  EPUBImageManager m_imageManager;
  EPUBFontManager m_fontManager;
  EPUBListStyleManager m_listStyleManager;
  EPUBParagraphStyleManager m_paragraphStyleManager;
  EPUBSpanStyleManager m_spanStyleManager;
  EPUBBodyStyleManager m_bodyStyleManager;
  EPUBTableStyleManager m_tableStyleManager;
  const EPUBPath m_stylesheetPath;

  librevenge::RVNGPropertyList m_documentProps;
  librevenge::RVNGPropertyList m_metadata;

  std::shared_ptr<EPUBHTMLGenerator> m_currentHtml;

  EPUBSplitGuard m_splitGuard;

  int m_version;
  EPUBStylesMethod m_stylesMethod;
  EPUBLayoutMethod m_layoutMethod;
};

}

#endif // INCLUDED_EPUBGENERATOR

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
