/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <libepubgen/EPUBTextGenerator.h>

#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "libepubgen_utils.h"
#include "EPUBGenerator.h"
#include "EPUBHTMLGenerator.h"
#include "EPUBHTMLManager.h"
#include "EPUBSplitGuard.h"
#include "EPUBTextElements.h"

using std::shared_ptr;

using librevenge::RVNGBinaryData;
using librevenge::RVNGProperty;
using librevenge::RVNGPropertyFactory;
using librevenge::RVNGPropertyList;
using librevenge::RVNGString;

namespace libepubgen
{

class EPUBEmbeddingContact;
class EPUBPackage;

namespace
{

typedef std::unordered_map<std::string, EPUBEmbeddedImage> ImageHandlerMap_t;

const char *CORE_MEDIA_TYPES[] =
{
  "image/gif",
  "image/jpeg",
  "image/png",
  "image/svg+xml"
};

/// Determines if this break property a page break one.
bool isPageBreak(const librevenge::RVNGProperty *property)
{
  if (!property)
    return false;

  librevenge::RVNGString str = property->getStr();
  return str != "column" && str != "auto";
}

}

struct EPUBTextGenerator::Impl : public EPUBGenerator
{
  Impl(EPUBPackage *package, int version);

private:
  void startHtmlFile() override;
  void endHtmlFile() override;

public:
  bool m_inPageSpan;
  bool m_inHeader;
  bool m_inFooter;

  RVNGPropertyList m_pageSpanProps;
  shared_ptr<EPUBTextElements> m_currentHeader;
  shared_ptr<EPUBTextElements> m_currentFooter;
  shared_ptr<EPUBTextElements> m_currentHeaderOrFooter;

  ImageHandlerMap_t m_imageHandlers;

  bool m_breakAfterPara;

private:
  // disable copying
  Impl(const Impl &);
  Impl &operator=(const Impl &);
};

EPUBTextGenerator::Impl::Impl(EPUBPackage *const package, int version)
  : EPUBGenerator(package, version)
  , m_inPageSpan(false)
  , m_inHeader(false)
  , m_inFooter(false)
  , m_pageSpanProps()
  , m_currentHeader()
  , m_currentFooter()
  , m_currentHeaderOrFooter()
  , m_imageHandlers()
  , m_breakAfterPara(false)
{
}

void EPUBTextGenerator::Impl::startHtmlFile()
{
  if (m_inPageSpan)
    getHtml()->closePageSpan();
}

void EPUBTextGenerator::Impl::endHtmlFile()
{
  getSplitGuard().setHtmlEverInPageSpan(false);
  if (m_inPageSpan)
    getHtml()->openPageSpan(m_pageSpanProps);
  if (bool(m_currentHeader))
    m_currentHeader->write(getHtml().get());
  if (bool(m_currentFooter))
    m_currentFooter->write(getHtml().get());
}

EPUBTextGenerator::EPUBTextGenerator(EPUBPackage *const package, int version)
  : m_impl(new Impl(package, version))
{
}

EPUBTextGenerator::EPUBTextGenerator(EPUBEmbeddingContact &contact, EPUBPackage *const package)
  : m_impl(nullptr)
{
  // TODO: implement me
  (void) contact;
  (void) package;
}

EPUBTextGenerator::~EPUBTextGenerator()
{
  delete m_impl;
}

void EPUBTextGenerator::setSplitHeadingLevel(const unsigned level)
{
  m_impl->getSplitGuard().setSplitHeadingLevel(level);
}

void EPUBTextGenerator::setSplitSize(const unsigned size)
{
  m_impl->getSplitGuard().setSplitSize(size);
}

void EPUBTextGenerator::setOption(int key, int value)
{
  switch (key)
  {
  case EPUB_GENERATOR_OPTION_SPLIT:
    m_impl->setSplitMethod(static_cast<EPUBSplitMethod>(value));
    break;
  case EPUB_GENERATOR_OPTION_STYLES:
    m_impl->setStylesMethod(static_cast<EPUBStylesMethod>(value));
    break;
  case EPUB_GENERATOR_OPTION_LAYOUT:
    m_impl->setLayoutMethod(static_cast<EPUBLayoutMethod>(value));
    break;
  }
}

void EPUBTextGenerator::registerEmbeddedImageHandler(const librevenge::RVNGString &mimeType, EPUBEmbeddedImage imageHandler)
{
  if (!mimeType.empty() && imageHandler)
    m_impl->m_imageHandlers[mimeType.cstr()] = imageHandler;
}

void EPUBTextGenerator::registerEmbeddedObjectHandler(const librevenge::RVNGString &mimeType, EPUBEmbeddedObject objectHandler)
{
  // TODO: implement me
  (void) mimeType;
  (void) objectHandler;
}

void EPUBTextGenerator::setDocumentMetaData(const librevenge::RVNGPropertyList &propList)
{
  m_impl->setDocumentMetaData(propList);

  m_impl->getHtml()->setDocumentMetaData(propList);
}

void EPUBTextGenerator::startDocument(const librevenge::RVNGPropertyList &propList)
{
  m_impl->startDocument(propList);
}

void EPUBTextGenerator::endDocument()
{
  m_impl->endDocument();
}

void EPUBTextGenerator::definePageStyle(const librevenge::RVNGPropertyList &propList)
{
  m_impl->getHtml()->definePageStyle(propList);
}

void EPUBTextGenerator::defineEmbeddedFont(const librevenge::RVNGPropertyList &propList)
{
  m_impl->getHtml()->defineEmbeddedFont(propList);
}

void EPUBTextGenerator::openPageSpan(const librevenge::RVNGPropertyList &propList)
{
  assert(!m_impl->m_inPageSpan);

  if (m_impl->getSplitGuard().splitOnSecondPageSpan())
    m_impl->startNewHtmlFile();

  m_impl->m_inPageSpan = true;
  m_impl->m_pageSpanProps = propList;

  m_impl->getHtml()->openPageSpan(propList);
  m_impl->getSplitGuard().setHtmlEverInPageSpan(true);
}

void EPUBTextGenerator::closePageSpan()
{
  assert(m_impl->m_inPageSpan);

  m_impl->m_inPageSpan = false;
  m_impl->m_pageSpanProps = RVNGPropertyList();

  m_impl->getHtml()->closePageSpan();
}

void EPUBTextGenerator::openHeader(const librevenge::RVNGPropertyList &propList)
{
  m_impl->m_inHeader = true;
  m_impl->m_currentHeader.reset(new EPUBTextElements());
  m_impl->m_currentHeaderOrFooter = m_impl->m_currentHeader;
  m_impl->m_currentHeaderOrFooter->addOpenHeader(propList);

  m_impl->getHtml()->openHeader(propList);
}

void EPUBTextGenerator::closeHeader()
{
  m_impl->m_inHeader = false;
  m_impl->m_currentHeaderOrFooter->addCloseHeader();
  m_impl->m_currentHeaderOrFooter.reset();

  m_impl->getHtml()->closeHeader();
}

void EPUBTextGenerator::openFooter(const librevenge::RVNGPropertyList &propList)
{
  m_impl->m_inFooter = true;
  m_impl->m_currentFooter.reset(new EPUBTextElements());
  m_impl->m_currentHeaderOrFooter = m_impl->m_currentFooter;
  m_impl->m_currentHeaderOrFooter->addOpenHeader(propList);

  m_impl->getHtml()->openFooter(propList);
}

void EPUBTextGenerator::closeFooter()
{
  m_impl->m_inFooter = false;
  m_impl->m_currentHeaderOrFooter->addCloseFooter();
  m_impl->m_currentHeaderOrFooter.reset();

  m_impl->getHtml()->closeFooter();
}


void EPUBTextGenerator::defineParagraphStyle(const librevenge::RVNGPropertyList &propList)
{
  m_impl->getHtml()->defineParagraphStyle(propList);
}

void EPUBTextGenerator::openParagraph(const librevenge::RVNGPropertyList &propList)
{
  const RVNGProperty *const breakBefore = propList["fo:break-before"];
  if (isPageBreak(breakBefore) && m_impl->getSplitGuard().splitOnPageBreak())
  {
    m_impl->startNewHtmlFile();
  }
  const RVNGProperty *const breakAfter = propList["fo:break-after"];
  m_impl->m_breakAfterPara = isPageBreak(breakAfter);
  if (m_impl->getSplitGuard().splitOnSize())
    m_impl->startNewHtmlFile();

  // Handle split by chapters.
  const RVNGProperty *const outlineLevel = propList["text:outline-level"];
  if (outlineLevel && m_impl->getSplitGuard().splitOnHeading(outlineLevel->getInt()))
    m_impl->startNewHtmlFile();
  m_impl->getSplitGuard().setCurrentHeadingLevel(outlineLevel ? outlineLevel->getInt() : 0);

  if (const librevenge::RVNGPropertyListVector *chapterNames = m_impl->m_pageSpanProps.child("librevenge:chapter-names"))
  {
    for (unsigned long i = 0; i < chapterNames->count(); i++)
    {
      RVNGPropertyList const &chapter=(*chapterNames)[i];
      const RVNGProperty *const chapterName = chapter["librevenge:name"];
      if (!chapterName)
        continue;

      m_impl->getHtmlManager().addChapterName(chapterName->getStr().cstr());
    }
  }

  m_impl->getSplitGuard().openLevel();

  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addOpenParagraph(propList);

  m_impl->getHtml()->openParagraph(propList);
}

void EPUBTextGenerator::closeParagraph()
{
  m_impl->getSplitGuard().closeLevel();

  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addCloseParagraph();

  m_impl->getHtml()->closeParagraph();

  if (m_impl->m_breakAfterPara && m_impl->getSplitGuard().splitOnPageBreak())
    m_impl->startNewHtmlFile();
  m_impl->m_breakAfterPara = false;
}

void EPUBTextGenerator::defineCharacterStyle(const librevenge::RVNGPropertyList &propList)
{
  m_impl->getHtml()->defineCharacterStyle(propList);
}

void EPUBTextGenerator::openSpan(const librevenge::RVNGPropertyList &propList)
{
  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addOpenSpan(propList);

  m_impl->getHtml()->openSpan(propList);
}

void EPUBTextGenerator::closeSpan()
{
  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addCloseSpan();

  m_impl->getHtml()->closeSpan();
}

void EPUBTextGenerator::openLink(const librevenge::RVNGPropertyList &propList)
{
  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addOpenLink(propList);

  m_impl->getHtml()->openLink(propList);
}

void EPUBTextGenerator::closeLink()
{
  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addCloseLink();

  m_impl->getHtml()->closeLink();
}

void EPUBTextGenerator::defineSectionStyle(const librevenge::RVNGPropertyList &propList)
{
  m_impl->getHtml()->defineSectionStyle(propList);
}

void EPUBTextGenerator::openSection(const librevenge::RVNGPropertyList &propList)
{
  if (m_impl->getSplitGuard().splitOnSize())
    m_impl->startNewHtmlFile();

  m_impl->getHtml()->openSection(propList);
}

void EPUBTextGenerator::closeSection()
{
  m_impl->getHtml()->closeSection();
}

void EPUBTextGenerator::insertTab()
{
  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addInsertTab();

  m_impl->getSplitGuard().incrementSize(1);

  m_impl->getHtml()->insertTab();
}

void EPUBTextGenerator::insertSpace()
{
  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addInsertSpace();

  m_impl->getSplitGuard().incrementSize(1);

  m_impl->getHtml()->insertSpace();
}

void EPUBTextGenerator::insertText(const librevenge::RVNGString &text)
{
  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addInsertText(text);

  if (m_impl->getSplitGuard().inHeading(!m_impl->getHtmlManager().hasHeadingText()))
    m_impl->getHtmlManager().insertHeadingText(text.cstr());

  m_impl->getSplitGuard().incrementSize(text.len());

  m_impl->getHtml()->insertText(text);
}

void EPUBTextGenerator::insertLineBreak()
{
  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addInsertLineBreak();

  m_impl->getSplitGuard().incrementSize(1);

  m_impl->getHtml()->insertLineBreak();
}

void EPUBTextGenerator::insertField(const librevenge::RVNGPropertyList &propList)
{
  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addInsertField(propList);

  m_impl->getHtml()->insertField(propList);
}

void EPUBTextGenerator::openOrderedListLevel(const librevenge::RVNGPropertyList &propList)
{
  if (m_impl->getSplitGuard().splitOnSize())
    m_impl->startNewHtmlFile();
  m_impl->getSplitGuard().openLevel();

  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addOpenOrderedListLevel(propList);

  m_impl->getHtml()->openOrderedListLevel(propList);
}

void EPUBTextGenerator::openUnorderedListLevel(const librevenge::RVNGPropertyList &propList)
{
  if (m_impl->getSplitGuard().splitOnSize())
    m_impl->startNewHtmlFile();
  m_impl->getSplitGuard().openLevel();

  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addOpenUnorderedListLevel(propList);

  m_impl->getHtml()->openUnorderedListLevel(propList);
}

void EPUBTextGenerator::closeOrderedListLevel()
{
  m_impl->getSplitGuard().closeLevel();

  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addCloseOrderedListLevel();

  m_impl->getHtml()->closeOrderedListLevel();
}

void EPUBTextGenerator::closeUnorderedListLevel()
{
  m_impl->getSplitGuard().closeLevel();

  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addCloseUnorderedListLevel();

  m_impl->getHtml()->closeUnorderedListLevel();
}

void EPUBTextGenerator::openListElement(const librevenge::RVNGPropertyList &propList)
{
  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addOpenListElement(propList);

  m_impl->getHtml()->openListElement(propList);
}

void EPUBTextGenerator::closeListElement()
{
  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addCloseListElement();

  m_impl->getHtml()->closeListElement();
}

void EPUBTextGenerator::openFootnote(const librevenge::RVNGPropertyList &propList)
{
  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addOpenFootnote(propList);

  m_impl->getHtml()->openFootnote(propList);
}

void EPUBTextGenerator::closeFootnote()
{
  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addCloseFootnote();

  m_impl->getHtml()->closeFootnote();
}

void EPUBTextGenerator::openEndnote(const librevenge::RVNGPropertyList &propList)
{
  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addOpenEndnote(propList);

  m_impl->getHtml()->openEndnote(propList);
}

void EPUBTextGenerator::closeEndnote()
{
  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addCloseEndnote();

  m_impl->getHtml()->closeEndnote();
}

void EPUBTextGenerator::openComment(const librevenge::RVNGPropertyList &propList)
{
  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addOpenComment(propList);

  m_impl->getHtml()->openComment(propList);
}

void EPUBTextGenerator::closeComment()
{
  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addCloseComment();

  m_impl->getHtml()->closeComment();
}

void EPUBTextGenerator::openTextBox(const librevenge::RVNGPropertyList &propList)
{
  if (m_impl->getSplitGuard().splitOnSize())
    m_impl->startNewHtmlFile();
  m_impl->getSplitGuard().openLevel();

  m_impl->getHtml()->openTextBox(propList);
}

void EPUBTextGenerator::closeTextBox()
{
  m_impl->getSplitGuard().closeLevel();

  m_impl->getHtml()->closeTextBox();
}

void EPUBTextGenerator::openTable(const librevenge::RVNGPropertyList &propList)
{
  if (m_impl->getSplitGuard().splitOnSize())
    m_impl->startNewHtmlFile();
  m_impl->getSplitGuard().openLevel();

  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addOpenTable(propList);

  m_impl->getHtml()->openTable(propList);
}

void EPUBTextGenerator::openTableRow(const librevenge::RVNGPropertyList &propList)
{
  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addOpenTableRow(propList);

  m_impl->getHtml()->openTableRow(propList);
}

void EPUBTextGenerator::closeTableRow()
{
  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addCloseTableRow();

  m_impl->getHtml()->closeTableRow();
}

void EPUBTextGenerator::openTableCell(const librevenge::RVNGPropertyList &propList)
{
  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addOpenTableCell(propList);

  m_impl->getHtml()->openTableCell(propList);
}

void EPUBTextGenerator::closeTableCell()
{
  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addCloseTableCell();

  m_impl->getHtml()->closeTableCell();
}

void EPUBTextGenerator::insertCoveredTableCell(const librevenge::RVNGPropertyList &propList)
{
  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addInsertCoveredTableCell(propList);

  m_impl->getHtml()->insertCoveredTableCell(propList);
}

void EPUBTextGenerator::closeTable()
{
  m_impl->getSplitGuard().closeLevel();

  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addCloseTable();

  m_impl->getHtml()->closeTable();
}

void EPUBTextGenerator::openFrame(const librevenge::RVNGPropertyList &propList)
{
  if (m_impl->getSplitGuard().splitOnSize())
    m_impl->startNewHtmlFile();
  m_impl->getSplitGuard().openLevel();

  m_impl->getHtml()->openFrame(propList);
}

void EPUBTextGenerator::closeFrame()
{
  m_impl->getSplitGuard().closeLevel();

  m_impl->getHtml()->closeFrame();
}

/// Checks if the media type is an EPUB 3 Core Media Type or not.
static bool isValidMimeType(const RVNGString &mediaType)
{
  // Defined at <https://idpf.github.io/epub-cmt/v3/#sec-cmt-supported>.
  static char const *types[] =
  {
    "image/gif",
    "image/png",
    "image/jpeg",
    "image/svg+xml"
  };

  for (const auto &i : types)
  {
    if (mediaType == i)
      return true;
  }

  return false;
}

void EPUBTextGenerator::insertBinaryObject(const librevenge::RVNGPropertyList &propList)
{
  if (m_impl->getSplitGuard().splitOnSize())
    m_impl->startNewHtmlFile();

  RVNGPropertyList newPropList;
  shared_ptr<RVNGProperty> mimetype;
  shared_ptr<RVNGProperty> data;

  for (RVNGPropertyList::Iter iter(propList); !iter.last(); iter.next())
  {
    if (RVNGString("librevenge:mime-type") == iter.key())
    {
      if (isValidMimeType(iter()->getStr()))
        mimetype.reset(iter()->clone());
    }
    else if (RVNGString("office:binary-data") == iter.key())
      data.reset(iter()->clone());
    else
      newPropList.insert(iter.key(), iter()->clone());
  }

  if (!mimetype || mimetype->getStr().empty() || !data)
  {
    EPUBGEN_DEBUG_MSG(("invalid binary object dropped"));
    return;
  }

  const ImageHandlerMap_t::const_iterator it = m_impl->m_imageHandlers.find(mimetype->getStr().cstr());
  if (m_impl->m_imageHandlers.end() != it)
  {
    RVNGBinaryData outData;
    EPUBImageType outType;
    const EPUBEmbeddedImage imageHandler = it->second;
    if (imageHandler(RVNGBinaryData(data->getStr()), outData, outType))
    {
      mimetype.reset(RVNGPropertyFactory::newStringProp(CORE_MEDIA_TYPES[outType]));
      data.reset(RVNGPropertyFactory::newBinaryDataProp(outData));
    }
    else
    {
      EPUBGEN_DEBUG_MSG(("image conversion failed"));
    }
  }

  newPropList.insert("librevenge:mime-type", mimetype->clone());
  newPropList.insert("office:binary-data", data->clone());

  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addInsertBinaryObject(newPropList);

  m_impl->getSplitGuard().incrementSize(1);
  m_impl->getHtml()->insertBinaryObject(newPropList);
}

void EPUBTextGenerator::insertEquation(const librevenge::RVNGPropertyList &propList)
{
  if (m_impl->getSplitGuard().splitOnSize())
    m_impl->startNewHtmlFile();

  if (m_impl->m_inHeader || m_impl->m_inFooter)
    m_impl->m_currentHeaderOrFooter->addInsertEquation(propList);

  m_impl->getHtml()->insertEquation(propList);
}

void EPUBTextGenerator::openGroup(const librevenge::RVNGPropertyList &propList)
{
  if (m_impl->getSplitGuard().splitOnSize())
    m_impl->startNewHtmlFile();
  m_impl->getSplitGuard().openLevel();

  m_impl->getHtml()->openGroup(propList);
}

void EPUBTextGenerator::closeGroup()
{
  m_impl->getSplitGuard().closeLevel();

  m_impl->getHtml()->closeGroup();
}

void EPUBTextGenerator::defineGraphicStyle(const librevenge::RVNGPropertyList &propList)
{
  m_impl->getHtml()->defineGraphicStyle(propList);
}

void EPUBTextGenerator::drawRectangle(const librevenge::RVNGPropertyList &propList)
{
  m_impl->getHtml()->drawRectangle(propList);
}

void EPUBTextGenerator::drawEllipse(const librevenge::RVNGPropertyList &propList)
{
  m_impl->getHtml()->drawEllipse(propList);
}

void EPUBTextGenerator::drawPolygon(const librevenge::RVNGPropertyList &propList)
{
  m_impl->getHtml()->drawPolygon(propList);
}

void EPUBTextGenerator::drawPolyline(const librevenge::RVNGPropertyList &propList)
{
  m_impl->getHtml()->drawPolyline(propList);
}

void EPUBTextGenerator::drawPath(const librevenge::RVNGPropertyList &propList)
{
  m_impl->getHtml()->drawPath(propList);
}

void EPUBTextGenerator::drawConnector(const librevenge::RVNGPropertyList &propList)
{
  m_impl->getHtml()->drawConnector(propList);
}

} // namespace libepubgen

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
