/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <libepubgen/EPUBDrawingGenerator.h>

#include "EPUBPagedGenerator.h"

namespace libepubgen
{

using librevenge::RVNGPropertyList;
using librevenge::RVNGString;

class EPUBEmbeddingContact;
class EPUBPackage;

class EPUBDrawingGenerator::Impl : public EPUBPagedGenerator
{
public:
  Impl(EPUBPackage *const package, int version);
};

EPUBDrawingGenerator::Impl::Impl(EPUBPackage *const package, int version)
  : EPUBPagedGenerator(package, version)
{
}

EPUBDrawingGenerator::EPUBDrawingGenerator(EPUBPackage *const package, int version)
  : m_impl(new Impl(package, version))
{
}

EPUBDrawingGenerator::EPUBDrawingGenerator(EPUBEmbeddingContact &contact, EPUBPackage *const package)
  : m_impl(nullptr)
{
  // TODO: implement me
  (void) contact;
  (void) package;
}

EPUBDrawingGenerator::~EPUBDrawingGenerator()
{
  delete m_impl;
}

void EPUBDrawingGenerator::setSplitHeadingLevel(const unsigned level)
{
  m_impl->setSplitHeadingLevel(level);
}

void EPUBDrawingGenerator::setOption(int key, int value)
{
  switch (key)
  {
  case EPUB_GENERATOR_OPTION_SPLIT:
    m_impl->setSplitMethod(static_cast<EPUBSplitMethod>(value));
    break;
  }
}

void EPUBDrawingGenerator::setSplitSize(const unsigned size)
{
  m_impl->setSplitSize(size);
}

void EPUBDrawingGenerator::registerEmbeddedImageHandler(const librevenge::RVNGString &mimeType, EPUBEmbeddedImage imageHandler)
{
  // TODO: implement me
  (void) mimeType;
  (void) imageHandler;
}

void EPUBDrawingGenerator::registerEmbeddedObjectHandler(const librevenge::RVNGString &mimeType, EPUBEmbeddedObject objectHandler)
{
  // TODO: implement me
  (void) mimeType;
  (void) objectHandler;
}

void EPUBDrawingGenerator::startDocument(const RVNGPropertyList &propList)
{
  m_impl->startDocument(propList);
}

void EPUBDrawingGenerator::endDocument()
{
  m_impl->endDocument();
}

void EPUBDrawingGenerator::setDocumentMetaData(const RVNGPropertyList &propList)
{
  m_impl->setDocumentMetaData(propList);
}

void EPUBDrawingGenerator::defineEmbeddedFont(const RVNGPropertyList &propList)
{
  m_impl->defineEmbeddedFont(propList);
}

void EPUBDrawingGenerator::startPage(const RVNGPropertyList &propList)
{
  m_impl->startSlide(propList);
}

void EPUBDrawingGenerator::endPage()
{
  m_impl->endSlide();
}

void EPUBDrawingGenerator::startMasterPage(const RVNGPropertyList &propList)
{
  m_impl->startMasterSlide(propList);
}

void EPUBDrawingGenerator::endMasterPage()
{
  m_impl->endMasterSlide();
}

void EPUBDrawingGenerator::setStyle(const RVNGPropertyList &propList)
{
  m_impl->setStyle(propList);
}

void EPUBDrawingGenerator::startLayer(const RVNGPropertyList &propList)
{
  m_impl->startLayer(propList);
}

void EPUBDrawingGenerator::endLayer()
{
  m_impl->endLayer();
}

void EPUBDrawingGenerator::startEmbeddedGraphics(const RVNGPropertyList &propList)
{
  m_impl->startEmbeddedGraphics(propList);
}

void EPUBDrawingGenerator::endEmbeddedGraphics()
{
  m_impl->endEmbeddedGraphics();
}

void EPUBDrawingGenerator::openGroup(const RVNGPropertyList &propList)
{
  m_impl->openGroup(propList);
}

void EPUBDrawingGenerator::closeGroup()
{
  m_impl->closeGroup();
}

void EPUBDrawingGenerator::drawRectangle(const RVNGPropertyList &propList)
{
  m_impl->drawRectangle(propList);
}

void EPUBDrawingGenerator::drawEllipse(const RVNGPropertyList &propList)
{
  m_impl->drawEllipse(propList);
}

void EPUBDrawingGenerator::drawPolygon(const RVNGPropertyList &propList)
{
  m_impl->drawPolygon(propList);
}

void EPUBDrawingGenerator::drawPolyline(const RVNGPropertyList &propList)
{
  m_impl->drawPolyline(propList);
}

void EPUBDrawingGenerator::drawPath(const RVNGPropertyList &propList)
{
  m_impl->drawPath(propList);
}

void EPUBDrawingGenerator::drawGraphicObject(const RVNGPropertyList &propList)
{
  m_impl->drawGraphicObject(propList);
}

void EPUBDrawingGenerator::drawConnector(const RVNGPropertyList &propList)
{
  m_impl->drawConnector(propList);
}

void EPUBDrawingGenerator::startTextObject(const RVNGPropertyList &propList)
{
  m_impl->startTextObject(propList);
}

void EPUBDrawingGenerator::endTextObject()
{
  m_impl->endTextObject();
}

void EPUBDrawingGenerator::startTableObject(const RVNGPropertyList &propList)
{
  m_impl->startTableObject(propList);
}

void EPUBDrawingGenerator::openTableRow(const RVNGPropertyList &propList)
{
  m_impl->openTableRow(propList);
}

void EPUBDrawingGenerator::closeTableRow()
{
  m_impl->closeTableRow();
}

void EPUBDrawingGenerator::openTableCell(const RVNGPropertyList &propList)
{
  m_impl->openTableCell(propList);
}

void EPUBDrawingGenerator::closeTableCell()
{
  m_impl->closeTableCell();
}

void EPUBDrawingGenerator::insertCoveredTableCell(const RVNGPropertyList &propList)
{
  m_impl->insertCoveredTableCell(propList);
}

void EPUBDrawingGenerator::endTableObject()
{
  m_impl->endTableObject();
}

void EPUBDrawingGenerator::insertTab()
{
  m_impl->insertTab();
}

void EPUBDrawingGenerator::insertSpace()
{
  m_impl->insertSpace();
}

void EPUBDrawingGenerator::insertText(const RVNGString &text)
{
  m_impl->insertText(text);
}

void EPUBDrawingGenerator::insertLineBreak()
{
  m_impl->insertLineBreak();
}

void EPUBDrawingGenerator::insertField(const RVNGPropertyList &propList)
{
  m_impl->insertField(propList);
}

void EPUBDrawingGenerator::openOrderedListLevel(const RVNGPropertyList &propList)
{
  m_impl->openOrderedListLevel(propList);
}

void EPUBDrawingGenerator::openUnorderedListLevel(const RVNGPropertyList &propList)
{
  m_impl->openUnorderedListLevel(propList);
}

void EPUBDrawingGenerator::closeOrderedListLevel()
{
  m_impl->closeOrderedListLevel();
}

void EPUBDrawingGenerator::closeUnorderedListLevel()
{
  m_impl->closeUnorderedListLevel();
}

void EPUBDrawingGenerator::openListElement(const RVNGPropertyList &propList)
{
  m_impl->openListElement(propList);
}

void EPUBDrawingGenerator::closeListElement()
{
  m_impl->closeListElement();
}

void EPUBDrawingGenerator::defineParagraphStyle(const RVNGPropertyList &propList)
{
  m_impl->defineParagraphStyle(propList);
}

void EPUBDrawingGenerator::openParagraph(const RVNGPropertyList &propList)
{
  m_impl->openParagraph(propList);
}

void EPUBDrawingGenerator::closeParagraph()
{
  m_impl->closeParagraph();
}

void EPUBDrawingGenerator::defineCharacterStyle(const RVNGPropertyList &propList)
{
  m_impl->defineCharacterStyle(propList);
}

void EPUBDrawingGenerator::openSpan(const RVNGPropertyList &propList)
{
  m_impl->openSpan(propList);
}

void EPUBDrawingGenerator::closeSpan()
{
  m_impl->closeSpan();
}

void EPUBDrawingGenerator::openLink(const RVNGPropertyList &propList)
{
  m_impl->openLink(propList);
}

void EPUBDrawingGenerator::closeLink()
{
  m_impl->closeLink();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
