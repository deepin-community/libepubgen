/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LIBEPUBGEN_EPUBPRESENTATIONGENERATOR_H
#define INCLUDED_LIBEPUBGEN_EPUBPRESENTATIONGENERATOR_H

#include <librevenge/librevenge.h>
#include <librevenge-stream/librevenge-stream.h>

#include "libepubgen-api.h"
#include "libepubgen-decls.h"

namespace libepubgen
{

class EPUBPackage;

/** EPUB generator for presentations.
  */
class EPUBGENAPI EPUBPresentationGenerator: public librevenge::RVNGPresentationInterface
{
  // disable copying
  EPUBPresentationGenerator(const EPUBPresentationGenerator &);
  EPUBPresentationGenerator &operator=(const EPUBPresentationGenerator &);

  class Impl;

public:
  /** Constructor.
    *
    * @param[in] version possible values: 20, 30.
    */
  explicit EPUBPresentationGenerator(EPUBPackage *package, int version = 30);
  EPUBPresentationGenerator(EPUBEmbeddingContact &contact, EPUBPackage *package);
  ~EPUBPresentationGenerator() override;

  void setSplitHeadingLevel(unsigned level);
  void setSplitSize(unsigned size);

  /** Set an option for the EPUB generator
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

  void startDocument(const librevenge::RVNGPropertyList &propList) override;

  void endDocument() override;

  void setDocumentMetaData(const librevenge::RVNGPropertyList &propList) override;

  void defineEmbeddedFont(const librevenge::RVNGPropertyList &propList) override;

  void startSlide(const librevenge::RVNGPropertyList &propList) override;

  void endSlide() override;

  void startMasterSlide(const librevenge::RVNGPropertyList &propList) override;

  void endMasterSlide() override;

  void setStyle(const librevenge::RVNGPropertyList &propList) override;

  void setSlideTransition(const librevenge::RVNGPropertyList &propList) override;

  void startLayer(const librevenge::RVNGPropertyList &propList) override;

  void endLayer() override;

  void startEmbeddedGraphics(const librevenge::RVNGPropertyList &propList) override;

  void endEmbeddedGraphics() override;

  void openGroup(const librevenge::RVNGPropertyList &propList) override;

  void closeGroup() override;

  void drawRectangle(const librevenge::RVNGPropertyList &propList) override;

  void drawEllipse(const librevenge::RVNGPropertyList &propList) override;

  void drawPolygon(const librevenge::RVNGPropertyList &propList) override;

  void drawPolyline(const librevenge::RVNGPropertyList &propList) override;

  void drawPath(const librevenge::RVNGPropertyList &propList) override;

  void drawGraphicObject(const librevenge::RVNGPropertyList &propList) override;

  void drawConnector(const librevenge::RVNGPropertyList &propList) override;

  void startTextObject(const librevenge::RVNGPropertyList &propList) override;

  void endTextObject() override;

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

  void defineParagraphStyle(const librevenge::RVNGPropertyList &propList) override;

  void openParagraph(const librevenge::RVNGPropertyList &propList) override;

  void closeParagraph() override;

  void defineCharacterStyle(const librevenge::RVNGPropertyList &propList) override;

  void openSpan(const librevenge::RVNGPropertyList &propList) override;

  void closeSpan() override;

  void openLink(const librevenge::RVNGPropertyList &propList) override;

  void closeLink() override;

  void startTableObject(const librevenge::RVNGPropertyList &propList) override;

  void openTableRow(const librevenge::RVNGPropertyList &propList) override;

  void closeTableRow() override;

  void openTableCell(const librevenge::RVNGPropertyList &propList) override;

  void closeTableCell() override;

  void insertCoveredTableCell(const librevenge::RVNGPropertyList &propList) override;

  void endTableObject() override;

  void startComment(const librevenge::RVNGPropertyList &propList) override;

  void endComment() override;

  void startNotes(const librevenge::RVNGPropertyList &propList) override;

  void endNotes() override;

  void defineChartStyle(const librevenge::RVNGPropertyList &propList) override;

  void openChart(const librevenge::RVNGPropertyList &propList) override;

  void closeChart() override;

  void openChartTextObject(const librevenge::RVNGPropertyList &propList) override;

  void closeChartTextObject() override;

  void openChartPlotArea(const librevenge::RVNGPropertyList &propList) override;

  void closeChartPlotArea() override;

  void insertChartAxis(const librevenge::RVNGPropertyList &propList) override;

  void openChartSeries(const librevenge::RVNGPropertyList &propList) override;

  void closeChartSeries() override;

  void openAnimationSequence(const librevenge::RVNGPropertyList &propList) override;

  void closeAnimationSequence() override;

  void openAnimationGroup(const librevenge::RVNGPropertyList &propList) override;

  void closeAnimationGroup() override;

  void openAnimationIteration(const librevenge::RVNGPropertyList &propList) override;

  void closeAnimationIteration() override;

  void insertMotionAnimation(const librevenge::RVNGPropertyList &propList) override;

  void insertColorAnimation(const librevenge::RVNGPropertyList &propList) override;

  void insertAnimation(const librevenge::RVNGPropertyList &propList) override;

  void insertEffect(const librevenge::RVNGPropertyList &propList) override;

private:
  Impl *m_impl;
};

}

#endif // INCLUDED_LIBEPUBGEN_EPUBPRESENTATIONGENERATOR_H

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
