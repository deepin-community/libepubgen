/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_EPUBHTMLGENERATOR_H
#define INCLUDED_EPUBHTMLGENERATOR_H

#include <memory>

#include <librevenge/librevenge.h>

#include <libepubgen/libepubgen-decls.h>

namespace libepubgen
{

struct EPUBHTMLGeneratorImpl;
class EPUBImageManager;
class EPUBFontManager;
class EPUBListStyleManager;
class EPUBSpanStyleManager;
class EPUBParagraphStyleManager;
class EPUBTableStyleManager;
class EPUBPath;
class EPUBXMLContent;

class EPUBHTMLGenerator : public librevenge::RVNGTextInterface
{
public:
  EPUBHTMLGenerator(EPUBXMLContent &document, EPUBImageManager &imageManager, EPUBFontManager &fontManager, EPUBListStyleManager &listStyleManager, EPUBParagraphStyleManager &paragraphStyleManager, EPUBSpanStyleManager &spanStyleManager, EPUBSpanStyleManager &bodyStyleManager, EPUBTableStyleManager &tableStyleManager, const EPUBPath &path, const EPUBPath &stylesheetPath, EPUBStylesMethod stylesMethod, EPUBLayoutMethod layoutMethod, int version);
  ~EPUBHTMLGenerator() override;

  void setDocumentMetaData(const librevenge::RVNGPropertyList &propList) override;

  void startDocument(const librevenge::RVNGPropertyList &propList) override;
  void endDocument() override;

  void defineEmbeddedFont(const librevenge::RVNGPropertyList &propList) override;

  void definePageStyle(const librevenge::RVNGPropertyList &) override;
  void openPageSpan(const librevenge::RVNGPropertyList &propList) override;
  void closePageSpan() override;
  void openHeader(const librevenge::RVNGPropertyList &propList) override;
  void closeHeader() override;
  void openFooter(const librevenge::RVNGPropertyList &propList) override;
  void closeFooter() override;

  void defineSectionStyle(const librevenge::RVNGPropertyList &) override;
  void openSection(const librevenge::RVNGPropertyList &propList) override;
  void closeSection() override;

  void defineParagraphStyle(const librevenge::RVNGPropertyList &) override;
  void openParagraph(const librevenge::RVNGPropertyList &propList) override;
  void closeParagraph() override;

  void defineCharacterStyle(const librevenge::RVNGPropertyList &) override;
  void openSpan(const librevenge::RVNGPropertyList &propList) override;
  void closeSpan() override;

  void openLink(const librevenge::RVNGPropertyList &propList) override;
  void closeLink() override;

  void insertTab() override;
  void insertText(const librevenge::RVNGString &text) override;
  void insertSpace() override;
  void insertLineBreak() override;
  void insertField(const librevenge::RVNGPropertyList &propList) override;

  void openOrderedListLevel(const librevenge::RVNGPropertyList &propList) override;
  void openUnorderedListLevel(const librevenge::RVNGPropertyList &propList) override;
  void closeOrderedListLevel() override;
  void closeUnorderedListLevel() override;
  void openListElement(const librevenge::RVNGPropertyList &propList) override;
  void closeListElement() override;

  void openFootnote(const librevenge::RVNGPropertyList &propList) override;
  void closeFootnote() override;
  void openEndnote(const librevenge::RVNGPropertyList &propList) override;
  void closeEndnote() override;
  void openComment(const librevenge::RVNGPropertyList &propList) override;
  void closeComment() override;
  void openTextBox(const librevenge::RVNGPropertyList &propList) override;
  void closeTextBox() override;

  void openTable(const librevenge::RVNGPropertyList &propList) override;
  void openTableRow(const librevenge::RVNGPropertyList &propList) override;
  void closeTableRow() override;
  void openTableCell(const librevenge::RVNGPropertyList &propList) override;
  void closeTableCell() override;
  void insertCoveredTableCell(const librevenge::RVNGPropertyList &propList) override;
  void closeTable() override;

  void openFrame(const librevenge::RVNGPropertyList &propList) override;
  void closeFrame() override;

  void openGroup(const librevenge::RVNGPropertyList &propList) override;
  void closeGroup() override;

  void defineGraphicStyle(const librevenge::RVNGPropertyList &propList) override;
  void drawRectangle(const librevenge::RVNGPropertyList &propList) override;
  void drawEllipse(const librevenge::RVNGPropertyList &propList) override;
  void drawPolygon(const librevenge::RVNGPropertyList &propList) override;
  void drawPolyline(const librevenge::RVNGPropertyList &propList) override;
  void drawPath(const librevenge::RVNGPropertyList &propList) override;
  void drawConnector(const librevenge::RVNGPropertyList &propList) override;

  void insertBinaryObject(const librevenge::RVNGPropertyList &propList) override;
  void insertEquation(const librevenge::RVNGPropertyList &propList) override;

  /// Gets the actual page properties into propList.
  void getPageProperties(librevenge::RVNGPropertyList &propList) const;
  /// Sets the actual page properties from propList.
  void setPageProperties(const librevenge::RVNGPropertyList &propList);

private:
  EPUBXMLContent &openPopup();
  void closePopup(EPUBXMLContent &main);

  std::unique_ptr<EPUBHTMLGeneratorImpl> m_impl;

  // Unimplemented to prevent compiler from creating crasher ones
  EPUBHTMLGenerator(const EPUBHTMLGenerator &);
  EPUBHTMLGenerator &operator=(const EPUBHTMLGenerator &);
};

}

#endif /* EPUBHTMLGENERATOR_H */

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
