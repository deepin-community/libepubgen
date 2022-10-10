/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "EPUBPagedGenerator.h"

#include <libepubgen/libepubgen-decls.h>

#include "EPUBGenerator.h"
#include "EPUBHTMLGenerator.h"
#include "EPUBSplitGuard.h"

namespace libepubgen
{

using librevenge::RVNGPropertyList;
using librevenge::RVNGString;

class EPUBPagedGenerator::Impl : public EPUBGenerator
{
  // disable copying
  Impl(const Impl &);
  Impl &operator=(const Impl &);

public:
  Impl(EPUBPackage *const package, int version);

private:
  void startHtmlFile() override;
  void endHtmlFile() override;

public:
  bool m_firstPage;
};

EPUBPagedGenerator::Impl::Impl(EPUBPackage *const package, int version)
  : EPUBGenerator(package, version)
  , m_firstPage(true)
{
}

void EPUBPagedGenerator::setSplitMethod(EPUBSplitMethod split)
{
  m_impl->setSplitMethod(split);
}

void EPUBPagedGenerator::setSplitHeadingLevel(const unsigned level)
{
  m_impl->getSplitGuard().setSplitHeadingLevel(level);
}

void EPUBPagedGenerator::setSplitSize(const unsigned size)
{
  m_impl->getSplitGuard().setSplitSize(size);
}

void EPUBPagedGenerator::Impl::startHtmlFile()
{
}

void EPUBPagedGenerator::Impl::endHtmlFile()
{
}

EPUBPagedGenerator::EPUBPagedGenerator(EPUBPackage *const package, int version)
  : m_impl(new Impl(package, version))
{
}

void EPUBPagedGenerator::startDocument(const RVNGPropertyList &propList)
{
  m_impl->startDocument(propList);
}

void EPUBPagedGenerator::endDocument()
{
  m_impl->endDocument();
}

void EPUBPagedGenerator::setDocumentMetaData(const RVNGPropertyList &propList)
{
  m_impl->setDocumentMetaData(propList);

  m_impl->getHtml()->setDocumentMetaData(propList);
}

void EPUBPagedGenerator::defineEmbeddedFont(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::startSlide(const RVNGPropertyList &/*propList*/)
{
  if (!m_impl->m_firstPage)
    m_impl->startNewHtmlFile();
  m_impl->m_firstPage = false;
}

void EPUBPagedGenerator::endSlide()
{
}

void EPUBPagedGenerator::startMasterSlide(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::endMasterSlide()
{
}

void EPUBPagedGenerator::setStyle(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::setSlideTransition(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::startLayer(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::endLayer()
{
}

void EPUBPagedGenerator::startEmbeddedGraphics(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::endEmbeddedGraphics()
{
}

void EPUBPagedGenerator::openGroup(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::closeGroup()
{
}

void EPUBPagedGenerator::drawRectangle(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::drawEllipse(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::drawPolygon(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::drawPolyline(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::drawPath(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::drawGraphicObject(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::drawConnector(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::startTextObject(const RVNGPropertyList &/*propList*/)
{
  m_impl->getSplitGuard().openLevel();
}

void EPUBPagedGenerator::endTextObject()
{
  m_impl->getSplitGuard().closeLevel();
}

void EPUBPagedGenerator::insertTab()
{
  m_impl->getHtml()->insertTab();
}

void EPUBPagedGenerator::insertSpace()
{
  m_impl->getHtml()->insertSpace();
}

void EPUBPagedGenerator::insertText(const RVNGString &text)
{
  m_impl->getHtml()->insertText(text);
}

void EPUBPagedGenerator::insertLineBreak()
{
  m_impl->getHtml()->insertLineBreak();
}

void EPUBPagedGenerator::insertField(const RVNGPropertyList &propList)
{
  m_impl->getHtml()->insertField(propList);
}

void EPUBPagedGenerator::openOrderedListLevel(const RVNGPropertyList &propList)
{
  m_impl->getSplitGuard().openLevel();

  m_impl->getHtml()->openOrderedListLevel(propList);
}

void EPUBPagedGenerator::openUnorderedListLevel(const RVNGPropertyList &propList)
{
  m_impl->getSplitGuard().openLevel();

  m_impl->getHtml()->openUnorderedListLevel(propList);
}

void EPUBPagedGenerator::closeOrderedListLevel()
{
  m_impl->getSplitGuard().closeLevel();

  m_impl->getHtml()->closeOrderedListLevel();
}

void EPUBPagedGenerator::closeUnorderedListLevel()
{
  m_impl->getSplitGuard().closeLevel();

  m_impl->getHtml()->closeUnorderedListLevel();
}

void EPUBPagedGenerator::openListElement(const RVNGPropertyList &propList)
{
  m_impl->getSplitGuard().openLevel();

  m_impl->getHtml()->openListElement(propList);
}

void EPUBPagedGenerator::closeListElement()
{
  m_impl->getSplitGuard().closeLevel();

  m_impl->getHtml()->closeListElement();
}

void EPUBPagedGenerator::defineParagraphStyle(const RVNGPropertyList &propList)
{
  m_impl->getHtml()->defineParagraphStyle(propList);
}

void EPUBPagedGenerator::openParagraph(const RVNGPropertyList &propList)
{
  m_impl->getSplitGuard().openLevel();

  m_impl->getHtml()->openParagraph(propList);
}

void EPUBPagedGenerator::closeParagraph()
{
  m_impl->getSplitGuard().closeLevel();

  m_impl->getHtml()->closeParagraph();
}

void EPUBPagedGenerator::defineCharacterStyle(const RVNGPropertyList &propList)
{
  m_impl->getHtml()->defineCharacterStyle(propList);
}

void EPUBPagedGenerator::openSpan(const RVNGPropertyList &propList)
{
  m_impl->getHtml()->openSpan(propList);
}

void EPUBPagedGenerator::closeSpan()
{
  m_impl->getHtml()->closeSpan();
}

void EPUBPagedGenerator::openLink(const RVNGPropertyList &propList)
{
  m_impl->getHtml()->openLink(propList);
}

void EPUBPagedGenerator::closeLink()
{
  m_impl->getHtml()->closeSpan();
}

void EPUBPagedGenerator::startTableObject(const RVNGPropertyList &propList)
{
  m_impl->getSplitGuard().openLevel();

  m_impl->getHtml()->openTable(propList);
}

void EPUBPagedGenerator::openTableRow(const RVNGPropertyList &propList)
{
  m_impl->getHtml()->openTableRow(propList);
}

void EPUBPagedGenerator::closeTableRow()
{
  m_impl->getHtml()->closeTableRow();
}

void EPUBPagedGenerator::openTableCell(const RVNGPropertyList &propList)
{
  m_impl->getHtml()->openTableCell(propList);
}

void EPUBPagedGenerator::closeTableCell()
{
  m_impl->getHtml()->closeTableCell();
}

void EPUBPagedGenerator::insertCoveredTableCell(const RVNGPropertyList &propList)
{
  m_impl->getHtml()->insertCoveredTableCell(propList);
}

void EPUBPagedGenerator::endTableObject()
{
  m_impl->getHtml()->closeTable();
}

void EPUBPagedGenerator::startComment(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::endComment()
{
}

void EPUBPagedGenerator::startNotes(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::endNotes()
{
}

void EPUBPagedGenerator::defineChartStyle(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::openChart(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::closeChart()
{
}

void EPUBPagedGenerator::openChartTextObject(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::closeChartTextObject()
{
}

void EPUBPagedGenerator::openChartPlotArea(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::closeChartPlotArea()
{
}

void EPUBPagedGenerator::insertChartAxis(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::openChartSeries(const librevenge::RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::closeChartSeries()
{
}

void EPUBPagedGenerator::openAnimationSequence(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::closeAnimationSequence()
{
}

void EPUBPagedGenerator::openAnimationGroup(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::closeAnimationGroup()
{
}

void EPUBPagedGenerator::openAnimationIteration(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::closeAnimationIteration()
{
}

void EPUBPagedGenerator::insertMotionAnimation(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::insertColorAnimation(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::insertAnimation(const RVNGPropertyList &propList)
{
  (void) propList;
}

void EPUBPagedGenerator::insertEffect(const RVNGPropertyList &propList)
{
  (void) propList;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
