/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <libepubgen/EPUBPresentationGenerator.h>

#include "EPUBPagedGenerator.h"

namespace libepubgen
{

using librevenge::RVNGPropertyList;
using librevenge::RVNGString;

class EPUBEmbeddingContact;
class EPUBPackage;

class EPUBPresentationGenerator::Impl : public EPUBPagedGenerator
{
public:
  Impl(EPUBPackage *const package, int version);
};

EPUBPresentationGenerator::Impl::Impl(EPUBPackage *const package, int version)
  : EPUBPagedGenerator(package, version)
{
}

EPUBPresentationGenerator::EPUBPresentationGenerator(EPUBPackage *const package, int version)
  : m_impl(new Impl(package, version))
{
}

EPUBPresentationGenerator::EPUBPresentationGenerator(EPUBEmbeddingContact &contact, EPUBPackage *const package)
  : m_impl(nullptr)
{
  // TODO: implement me
  (void) contact;
  (void) package;
}

EPUBPresentationGenerator::~EPUBPresentationGenerator()
{
  delete m_impl;
}

void EPUBPresentationGenerator::setOption(int key, int value)
{
  switch (key)
  {
  case EPUB_GENERATOR_OPTION_SPLIT:
    m_impl->setSplitMethod(static_cast<EPUBSplitMethod>(value));
    break;
  }
}

void EPUBPresentationGenerator::setSplitHeadingLevel(const unsigned level)
{
  m_impl->setSplitHeadingLevel(level);
}

void EPUBPresentationGenerator::setSplitSize(const unsigned size)
{
  m_impl->setSplitSize(size);
}

void EPUBPresentationGenerator::registerEmbeddedImageHandler(const librevenge::RVNGString &mimeType, EPUBEmbeddedImage imageHandler)
{
  // TODO: implement me
  (void) mimeType;
  (void) imageHandler;
}

void EPUBPresentationGenerator::registerEmbeddedObjectHandler(const librevenge::RVNGString &mimeType, EPUBEmbeddedObject objectHandler)
{
  // TODO: implement me
  (void) mimeType;
  (void) objectHandler;
}

void EPUBPresentationGenerator::startDocument(const RVNGPropertyList &propList)
{
  m_impl->startDocument(propList);
}

void EPUBPresentationGenerator::endDocument()
{
  m_impl->endDocument();
}

void EPUBPresentationGenerator::setDocumentMetaData(const RVNGPropertyList &propList)
{
  m_impl->setDocumentMetaData(propList);
}

void EPUBPresentationGenerator::defineEmbeddedFont(const RVNGPropertyList &propList)
{
  m_impl->defineEmbeddedFont(propList);
}

void EPUBPresentationGenerator::startSlide(const RVNGPropertyList &propList)
{
  m_impl->startSlide(propList);
}

void EPUBPresentationGenerator::endSlide()
{
  m_impl->endSlide();
}

void EPUBPresentationGenerator::startMasterSlide(const RVNGPropertyList &propList)
{
  m_impl->startMasterSlide(propList);
}

void EPUBPresentationGenerator::endMasterSlide()
{
  m_impl->endMasterSlide();
}

void EPUBPresentationGenerator::setStyle(const RVNGPropertyList &propList)
{
  m_impl->setStyle(propList);
}

void EPUBPresentationGenerator::setSlideTransition(const RVNGPropertyList &propList)
{
  m_impl->setSlideTransition(propList);
}

void EPUBPresentationGenerator::startLayer(const RVNGPropertyList &propList)
{
  m_impl->startLayer(propList);
}

void EPUBPresentationGenerator::endLayer()
{
  m_impl->endLayer();
}

void EPUBPresentationGenerator::startEmbeddedGraphics(const RVNGPropertyList &propList)
{
  m_impl->startEmbeddedGraphics(propList);
}

void EPUBPresentationGenerator::endEmbeddedGraphics()
{
  m_impl->endEmbeddedGraphics();
}

void EPUBPresentationGenerator::openGroup(const RVNGPropertyList &propList)
{
  m_impl->openGroup(propList);
}

void EPUBPresentationGenerator::closeGroup()
{
  m_impl->closeGroup();
}

void EPUBPresentationGenerator::drawRectangle(const RVNGPropertyList &propList)
{
  m_impl->drawRectangle(propList);
}

void EPUBPresentationGenerator::drawEllipse(const RVNGPropertyList &propList)
{
  m_impl->drawEllipse(propList);
}

void EPUBPresentationGenerator::drawPolygon(const RVNGPropertyList &propList)
{
  m_impl->drawPolygon(propList);
}

void EPUBPresentationGenerator::drawPolyline(const RVNGPropertyList &propList)
{
  m_impl->drawPolyline(propList);
}

void EPUBPresentationGenerator::drawPath(const RVNGPropertyList &propList)
{
  m_impl->drawPath(propList);
}

void EPUBPresentationGenerator::drawGraphicObject(const RVNGPropertyList &propList)
{
  m_impl->drawGraphicObject(propList);
}

void EPUBPresentationGenerator::drawConnector(const RVNGPropertyList &propList)
{
  m_impl->drawConnector(propList);
}

void EPUBPresentationGenerator::startTextObject(const RVNGPropertyList &propList)
{
  m_impl->startTextObject(propList);
}

void EPUBPresentationGenerator::endTextObject()
{
  m_impl->endTextObject();
}

void EPUBPresentationGenerator::insertTab()
{
  m_impl->insertTab();
}

void EPUBPresentationGenerator::insertSpace()
{
  m_impl->insertSpace();
}

void EPUBPresentationGenerator::insertText(const RVNGString &text)
{
  m_impl->insertText(text);
}

void EPUBPresentationGenerator::insertLineBreak()
{
  m_impl->insertLineBreak();
}

void EPUBPresentationGenerator::insertField(const RVNGPropertyList &propList)
{
  m_impl->insertField(propList);
}

void EPUBPresentationGenerator::openOrderedListLevel(const RVNGPropertyList &propList)
{
  m_impl->openOrderedListLevel(propList);
}

void EPUBPresentationGenerator::openUnorderedListLevel(const RVNGPropertyList &propList)
{
  m_impl->openUnorderedListLevel(propList);
}

void EPUBPresentationGenerator::closeOrderedListLevel()
{
  m_impl->closeOrderedListLevel();
}

void EPUBPresentationGenerator::closeUnorderedListLevel()
{
  m_impl->closeUnorderedListLevel();
}

void EPUBPresentationGenerator::openListElement(const RVNGPropertyList &propList)
{
  m_impl->openListElement(propList);
}

void EPUBPresentationGenerator::closeListElement()
{
  m_impl->closeListElement();
}

void EPUBPresentationGenerator::defineParagraphStyle(const RVNGPropertyList &propList)
{
  m_impl->defineParagraphStyle(propList);
}

void EPUBPresentationGenerator::openParagraph(const RVNGPropertyList &propList)
{
  m_impl->openParagraph(propList);
}

void EPUBPresentationGenerator::closeParagraph()
{
  m_impl->closeParagraph();
}

void EPUBPresentationGenerator::defineCharacterStyle(const RVNGPropertyList &propList)
{
  m_impl->defineCharacterStyle(propList);
}

void EPUBPresentationGenerator::openSpan(const RVNGPropertyList &propList)
{
  m_impl->openSpan(propList);
}

void EPUBPresentationGenerator::closeSpan()
{
  m_impl->closeSpan();
}

void EPUBPresentationGenerator::openLink(const RVNGPropertyList &propList)
{
  m_impl->openLink(propList);
}

void EPUBPresentationGenerator::closeLink()
{
  m_impl->closeLink();
}

void EPUBPresentationGenerator::startTableObject(const RVNGPropertyList &propList)
{
  m_impl->startTableObject(propList);
}

void EPUBPresentationGenerator::openTableRow(const RVNGPropertyList &propList)
{
  m_impl->openTableRow(propList);
}

void EPUBPresentationGenerator::closeTableRow()
{
  m_impl->closeTableRow();
}

void EPUBPresentationGenerator::openTableCell(const RVNGPropertyList &propList)
{
  m_impl->openTableCell(propList);
}

void EPUBPresentationGenerator::closeTableCell()
{
  m_impl->closeTableCell();
}

void EPUBPresentationGenerator::insertCoveredTableCell(const RVNGPropertyList &propList)
{
  m_impl->insertCoveredTableCell(propList);
}

void EPUBPresentationGenerator::endTableObject()
{
  m_impl->endTableObject();
}

void EPUBPresentationGenerator::startComment(const RVNGPropertyList &propList)
{
  m_impl->startComment(propList);
}

void EPUBPresentationGenerator::endComment()
{
  m_impl->endComment();
}

void EPUBPresentationGenerator::startNotes(const RVNGPropertyList &propList)
{
  m_impl->startNotes(propList);
}

void EPUBPresentationGenerator::endNotes()
{
  m_impl->endNotes();
}

void EPUBPresentationGenerator::defineChartStyle(const RVNGPropertyList &propList)
{
  m_impl->defineChartStyle(propList);
}

void EPUBPresentationGenerator::openChart(const RVNGPropertyList &propList)
{
  m_impl->openChart(propList);
}

void EPUBPresentationGenerator::closeChart()
{
  m_impl->closeChart();
}

void EPUBPresentationGenerator::openChartTextObject(const RVNGPropertyList &propList)
{
  m_impl->openChartTextObject(propList);
}

void EPUBPresentationGenerator::closeChartTextObject()
{
  m_impl->closeChartTextObject();
}

void EPUBPresentationGenerator::openChartPlotArea(const RVNGPropertyList &propList)
{
  m_impl->openChartPlotArea(propList);
}

void EPUBPresentationGenerator::closeChartPlotArea()
{
  m_impl->closeChartPlotArea();
}

void EPUBPresentationGenerator::insertChartAxis(const RVNGPropertyList &propList)
{
  m_impl->insertChartAxis(propList);
}

void EPUBPresentationGenerator::openChartSeries(const librevenge::RVNGPropertyList &propList)
{
  m_impl->openChartSeries(propList);
}

void EPUBPresentationGenerator::closeChartSeries()
{
  m_impl->closeChartSeries();
}

void EPUBPresentationGenerator::openAnimationSequence(const RVNGPropertyList &propList)
{
  m_impl->openAnimationSequence(propList);
}

void EPUBPresentationGenerator::closeAnimationSequence()
{
  m_impl->closeAnimationSequence();
}

void EPUBPresentationGenerator::openAnimationGroup(const RVNGPropertyList &propList)
{
  m_impl->openAnimationGroup(propList);
}

void EPUBPresentationGenerator::closeAnimationGroup()
{
  m_impl->closeAnimationGroup();
}

void EPUBPresentationGenerator::openAnimationIteration(const RVNGPropertyList &propList)
{
  m_impl->openAnimationIteration(propList);
}

void EPUBPresentationGenerator::closeAnimationIteration()
{
  m_impl->closeAnimationIteration();
}

void EPUBPresentationGenerator::insertMotionAnimation(const RVNGPropertyList &propList)
{
  m_impl->insertMotionAnimation(propList);
}

void EPUBPresentationGenerator::insertColorAnimation(const RVNGPropertyList &propList)
{
  m_impl->insertColorAnimation(propList);
}

void EPUBPresentationGenerator::insertAnimation(const RVNGPropertyList &propList)
{
  m_impl->insertAnimation(propList);
}

void EPUBPresentationGenerator::insertEffect(const RVNGPropertyList &propList)
{
  m_impl->insertEffect(propList);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
