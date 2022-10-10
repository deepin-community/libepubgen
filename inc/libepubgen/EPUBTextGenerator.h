/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LIBEPUBGEN_EPUBTEXTGENERATOR_H
#define INCLUDED_LIBEPUBGEN_EPUBTEXTGENERATOR_H

#include <librevenge/librevenge.h>
#include <librevenge-stream/librevenge-stream.h>

#include "libepubgen-api.h"
#include "libepubgen-decls.h"

namespace libepubgen
{

class EPUBPackage;

/** EPUB generator for text documents.
  */
class EPUBGENAPI EPUBTextGenerator : public librevenge::RVNGTextInterface
{
  // disable copying
  EPUBTextGenerator(const EPUBTextGenerator &);
  EPUBTextGenerator &operator=(const EPUBTextGenerator &);

  struct Impl;

public:
  /** Constructor.
    *
    * @param[in] version possible values: 20, 30.
    */
  explicit EPUBTextGenerator(EPUBPackage *package, int version = 30);
  EPUBTextGenerator(EPUBEmbeddingContact &contact, EPUBPackage *package);
  ~EPUBTextGenerator() override;

  void setSplitHeadingLevel(unsigned level);
  void setSplitSize(unsigned size);

  /** Set an option for the EPUB generator
   *
   * @param[in] key a value from the EPUBGeneratorOption enumeration
   * @param[in] value depends on the value of key
   */
  void setOption(int key, int value);

  /** Register a handler for embedded images.
    *
    * The handler must convert the image to one of the OPS Core Media Types.
    *
    * @param[in] mimeType the MIME type of the image
    * @param[in] imageHandler a function that converts the image to a
    *   suitable format
    *
    * @sa EPUBEmbeddedImage
    */
  void registerEmbeddedImageHandler(const librevenge::RVNGString &mimeType, EPUBEmbeddedImage imageHandler);

  /** Register a handler for embedded objects.
    *
    * @param[in] mimeType the MIME type of the object
    * @param[in] objectHandler a function that generates EPUB content
    *   from an object's data
    *
    * @sa EPUBEmbeddedObject
    */
  void registerEmbeddedObjectHandler(const librevenge::RVNGString &mimeType, EPUBEmbeddedObject objectHandler);

  void setDocumentMetaData(const librevenge::RVNGPropertyList &propList) override;

  void startDocument(const librevenge::RVNGPropertyList &propList) override;

  void endDocument() override;

  void definePageStyle(const librevenge::RVNGPropertyList &propList) override;

  void defineEmbeddedFont(const librevenge::RVNGPropertyList &propList) override;

  void openPageSpan(const librevenge::RVNGPropertyList &propList) override;
  void closePageSpan() override;

  void openHeader(const librevenge::RVNGPropertyList &propList) override;
  void closeHeader() override;

  void openFooter(const librevenge::RVNGPropertyList &propList) override;
  void closeFooter() override;

  void defineParagraphStyle(const librevenge::RVNGPropertyList &propList) override;

  void openParagraph(const librevenge::RVNGPropertyList &propList) override;
  void closeParagraph() override;

  void defineCharacterStyle(const librevenge::RVNGPropertyList &propList) override;

  void openSpan(const librevenge::RVNGPropertyList &propList) override;
  void closeSpan() override;
  void openLink(const librevenge::RVNGPropertyList &propList) override;
  void closeLink() override;

  void defineSectionStyle(const librevenge::RVNGPropertyList &propList) override;

  void openSection(const librevenge::RVNGPropertyList &propList) override;
  void closeSection() override;

  void insertTab() override;
  void insertSpace() override;
  void insertText(const librevenge::RVNGString &text) override;
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
  void insertBinaryObject(const librevenge::RVNGPropertyList &propList) override;
  void insertEquation(const librevenge::RVNGPropertyList &propList) override;

  void openGroup(const librevenge::RVNGPropertyList &propList) override;
  void closeGroup() override;

  void defineGraphicStyle(const librevenge::RVNGPropertyList &propList) override;

  void drawRectangle(const librevenge::RVNGPropertyList &propList) override;
  void drawEllipse(const librevenge::RVNGPropertyList &propList) override;
  void drawPolygon(const librevenge::RVNGPropertyList &propList) override;
  void drawPolyline(const librevenge::RVNGPropertyList &propList) override;
  void drawPath(const librevenge::RVNGPropertyList &propList) override;

  void drawConnector(const librevenge::RVNGPropertyList &propList) override;

private:
  Impl *const m_impl;
};

} // namespace libepubgen

#endif // INCLUDED_LIBEPUBGEN_EPUBTEXTGENERATOR_H

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
