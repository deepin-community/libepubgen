/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_EPUBPAGEDGENERATOR_H
#define INCLUDED_EPUBPAGEDGENERATOR_H

#include <memory>

#include <librevenge/librevenge.h>
#include <librevenge-stream/librevenge-stream.h>

#include <libepubgen/libepubgen-decls.h>

namespace libepubgen
{

class EPUBPackage;

class EPUBPagedGenerator: public librevenge::RVNGPresentationInterface
{
  class Impl;

public:
  EPUBPagedGenerator(EPUBPackage *package, int version);

  void setSplitMethod(EPUBSplitMethod split);
  void setSplitHeadingLevel(unsigned level);
  void setSplitSize(unsigned size);

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
  std::shared_ptr<Impl> m_impl;
};

}

#endif // INCLUDED_EPUBPAGEDGENERATOR_H

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
