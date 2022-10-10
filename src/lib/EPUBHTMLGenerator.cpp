/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "EPUBHTMLGenerator.h"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

#include <boost/algorithm/string/replace.hpp>

#include "EPUBFontManager.h"
#include "EPUBImageManager.h"
#include "EPUBListStyleManager.h"
#include "EPUBParagraphStyleManager.h"
#include "EPUBPath.h"
#include "EPUBSpanStyleManager.h"
#include "EPUBTableStyleManager.h"
#include "EPUBXMLContent.h"

#include "libepubgen_utils.h"

namespace libepubgen
{

using librevenge::RVNGBinaryData;
using librevenge::RVNGPropertyList;
using librevenge::RVNGString;

using std::string;

namespace
{

class ZoneSinkImpl
{
public:
  ZoneSinkImpl();

  void openElement(const char *name, const librevenge::RVNGPropertyList &attributes);
  void closeElement(const char *name);

  void insertCharacters(const librevenge::RVNGString &characters);

  void append(const ZoneSinkImpl &other);

  const EPUBXMLContent &get() const;
  EPUBXMLContent &get();

  bool endsInLineBreak() const;

private:
  EPUBXMLContent m_content;
  std::string m_lastCloseElement;
};

ZoneSinkImpl::ZoneSinkImpl()
  : m_content()
  , m_lastCloseElement()
{
}

void ZoneSinkImpl::openElement(const char *const name, const librevenge::RVNGPropertyList &attributes)
{
  m_content.openElement(name, attributes);
  m_lastCloseElement.clear();
}

void ZoneSinkImpl::closeElement(const char *const name)
{
  m_content.closeElement(name);
  m_lastCloseElement = name;
}

void ZoneSinkImpl::insertCharacters(const librevenge::RVNGString &characters)
{
  m_content.insertCharacters(characters);
  m_lastCloseElement.clear();
}

void ZoneSinkImpl::append(const ZoneSinkImpl &other)
{
  m_content.append(other.m_content);
  m_lastCloseElement = other.m_lastCloseElement;
}

const EPUBXMLContent &ZoneSinkImpl::get() const
{
  return m_content;
}

EPUBXMLContent &ZoneSinkImpl::get()
{
  return m_content;
}

bool ZoneSinkImpl::endsInLineBreak() const
{
  return m_lastCloseElement == "p"
         || m_lastCloseElement == "ul"
         || m_lastCloseElement == "ol"
         || m_lastCloseElement == "br"
         ;
}

/// Convers inches to CSS pixels.
int inchToCSSPixel(const librevenge::RVNGProperty *property)
{
  return round(property->getDouble() * 96);
}
}

namespace
{

struct TextZoneSink;

//! a zone to regroup footnote/endnote,... data
struct EPUBHTMLTextZone
{
  friend struct TextZoneSink;

  //! the different zone
  enum Type { Z_Comment=0, Z_EndNote, Z_FootNote, Z_Main, Z_MetaData, Z_TextBox, Z_Unknown, Z_NumZones= Z_Unknown+1};
  //! constructor for basic stream
  EPUBHTMLTextZone(Type tp=Z_Unknown) : m_type(tp), m_actualId(0), m_zoneSinks(), m_version(20)
  {
  }
  //! the type
  Type type() const
  {
    return m_type;
  }
  //! the type
  void setType(Type tp)
  {
    m_type=tp;
  }
  void setVersion(int version)
  {
    m_version = version;
  }
  int getVersion() const
  {
    return m_version;
  }
  //! returns a new sink corresponding to this zone
  std::unique_ptr<TextZoneSink> getNewSink();
  //! returns true if there is no data
  bool isEmpty() const
  {
    for (const auto &zoneSink : m_zoneSinks)
      if (!zoneSink.get().empty())
        return false;
    return true;
  }
  //! send the zone data
  void send(EPUBXMLContent &out) const
  {
    if (isEmpty() || m_type==Z_Unknown || m_type==Z_Main)
      return;
    if (m_type!=Z_MetaData && m_version < 30)
    {
      out.openElement("hr", RVNGPropertyList());
      out.closeElement("hr");
    }
    if (m_type==Z_MetaData)
    {
      for (const auto &zoneSink : m_zoneSinks)
        out.append(zoneSink.get());
      return;
    }
    if (m_type==Z_TextBox)
    {
      out.openElement("p", RVNGPropertyList());
      out.openElement("b", RVNGPropertyList());
      out.insertCharacters("TEXT BOXES");
      out.closeElement("b");
      out.closeElement("p");
      for (const auto &zoneSink : m_zoneSinks)
      {
        out.append(zoneSink.get());
        out.openElement("hr", RVNGPropertyList());
        out.closeElement("hr");
      }
      return;
    }
    for (const auto &zoneSink : m_zoneSinks)
    {
      out.append(zoneSink.get());
      // check if we need to add a return line
      if (!zoneSink.endsInLineBreak())
      {
        out.openElement("br", RVNGPropertyList());
        out.closeElement("br");
      }
    }
  }

protected:
  //! return a label corresponding to the zone
  std::string label(int id) const;
  //! the zone type
  Type m_type;
  //! the actual id
  mutable int m_actualId;
  //! the list of data string
  std::vector<ZoneSinkImpl> m_zoneSinks;
  int m_version;
private:
  EPUBHTMLTextZone(EPUBHTMLTextZone const &orig);
  EPUBHTMLTextZone operator=(EPUBHTMLTextZone const &orig);
};

struct TextZoneSink
{
  //! constructor
  TextZoneSink(EPUBHTMLTextZone *zone): m_zone(zone), m_zoneId(0), m_sink(), m_delayedLabel()
  {
    if (m_zone)
      m_zoneId=m_zone->m_actualId++;
  }
  //! destructor
  ~TextZoneSink() { }
  //! add a label called on main and a label in this ( delayed to allow openParagraph to be called )
  //! @param closeAnchor determintes if the anchor on the main sink should be closed or not.
  void addLabel(EPUBXMLContent &output, const librevenge::RVNGString &number, bool closeAnchor)
  {
    // Unique label, e.g. 'F1' for the first footnote.
    std::string lbl=label();
    // User-visible label, e.g. '1'.
    std::string uiLabel = lbl;
    if (!number.empty())
      uiLabel = number.cstr();
    if (!lbl.length())
      return;
    int version = 30;
    if (m_zone)
      version = m_zone->getVersion();
    {
      RVNGPropertyList supAttrs;
      supAttrs.insert("id", ("called" + lbl).c_str());
      if (closeAnchor)
        output.openElement("sup", supAttrs);
      RVNGPropertyList aAttrs;
      if (version >= 30)
        aAttrs.insert("epub:type", "noteref");
      aAttrs.insert("href", ("#data" + lbl).c_str());
      output.openElement("a", aAttrs);
      if (closeAnchor)
      {
        output.insertCharacters(uiLabel.c_str());
        output.closeElement("a");
        output.closeElement("sup");
      }
    }
    flush();
    if (version >= 30)
    {
      RVNGPropertyList asideAttrs;
      asideAttrs.insert("epub:type", "footnote");
      asideAttrs.insert("id", ("data" + lbl).c_str());
      m_sink.openElement("aside", asideAttrs);
    }
    RVNGPropertyList supAttrs;
    if (version < 30)
      supAttrs.insert("id", ("data" + lbl).c_str());
    if (closeAnchor)
    {
      m_delayedLabel.openElement("sup", supAttrs);
      RVNGPropertyList aAttrs;
      aAttrs.insert("href", ("#called" + lbl).c_str());
      m_delayedLabel.openElement("a", aAttrs);
      m_delayedLabel.insertCharacters(uiLabel.c_str());
      m_delayedLabel.closeElement("a");
      m_delayedLabel.closeElement("sup");
    }
  }
  //! flush delayed label, ...
  void flush()
  {
    m_sink.append(m_delayedLabel);
    m_delayedLabel = ZoneSinkImpl();
  }
  //! return the sink
  EPUBXMLContent &getContent()
  {
    return m_sink.get();
  }
  //! send the data to the zone
  void send()
  {
    if (!m_zone || m_zone->m_type==EPUBHTMLTextZone::Z_Main)
    {
      EPUBGEN_DEBUG_MSG(("TextZoneSink::send: must not be called\n"));
      return;
    }
    flush();
    if (m_zone->m_zoneSinks.size() <= size_t(m_zoneId))
      m_zone->m_zoneSinks.resize(size_t(m_zoneId)+1);
    m_zone->m_zoneSinks[size_t(m_zoneId)] = m_sink;
  }
  //! send the data to the zone
  void sendMain(EPUBXMLContent &output)
  {
    flush();
    output.append(m_sink.get());
  }
protected:
  //! return the zone label
  std::string label() const
  {
    if (!m_zone || m_zone->m_type==EPUBHTMLTextZone::Z_Main)
    {
      EPUBGEN_DEBUG_MSG(("TextZoneSink::label: must not be called\n"));
      return "";
    }
    return m_zone->label(m_zoneId);
  }
  //! a zone
  EPUBHTMLTextZone *m_zone;
  //! the zone id
  int m_zoneId;
  //! the sink
  ZoneSinkImpl m_sink;
  //! the label
  ZoneSinkImpl m_delayedLabel;
private:
  TextZoneSink(TextZoneSink const &orig);
  TextZoneSink operator=(TextZoneSink const &orig);
};

std::unique_ptr<TextZoneSink> EPUBHTMLTextZone::getNewSink()
{
  return make_unique<TextZoneSink>(this);
}

std::string EPUBHTMLTextZone::label(int id) const
{
  char c=0;
  switch (m_type)
  {
  case Z_Comment:
    c='C';
    break;
  case Z_EndNote:
    c='E';
    break;
  case Z_FootNote:
    c='F';
    break;
  case Z_TextBox:
    c='T';
    break;
  case Z_Main:
  case Z_MetaData:
  case Z_Unknown:
  case Z_NumZones:
  default:
    break;
  }
  if (c==0)
    return "";
  std::stringstream s;
  s << c << id+1;
  return s.str();
}

}

//! the internal state of a html document generator
struct EPUBHTMLGeneratorImpl
{
  //! constructor
  EPUBHTMLGeneratorImpl(EPUBXMLContent &document, EPUBImageManager &imageManager, EPUBFontManager &fontManager, EPUBListStyleManager &listStyleManager, EPUBParagraphStyleManager &paragraphStyleManager, EPUBSpanStyleManager &spanStyleManager, EPUBSpanStyleManager &bodyStyleManager, EPUBTableStyleManager &tableStyleManager, const EPUBPath &path, const EPUBPath &stylesheetPath, EPUBStylesMethod stylesMethod, EPUBLayoutMethod layoutMethod, int version)
    : m_document(document)
    , m_imageManager(imageManager)
    , m_fontManager(fontManager)
    , m_listManager(listStyleManager)
    , m_paragraphManager(paragraphStyleManager)
    , m_spanManager(spanStyleManager)
    , m_bodyManager(bodyStyleManager)
    , m_tableManager(tableStyleManager)
    , m_path(path)
    , m_stylesheetPath(stylesheetPath)
    , m_actualPage(0)
    , m_actualPageProperties()
    , m_ignore(false)
    , m_hasText(false)
    , m_version(version)
    , m_frameAnchorTypes()
    , m_framePropertiesStack()
    , m_linkPropertiesStack()
    , m_paragraphAttributesStack()
    , m_spanAttributesStack()
    , m_rubyText()
    , m_stylesMethod(stylesMethod)
    , m_layoutMethod(layoutMethod)
    , m_actualSink()
    , m_sinkStack()
  {
    for (int i = 0; i < EPUBHTMLTextZone::Z_NumZones; ++i)
    {
      m_zones[i].setType(EPUBHTMLTextZone::Type(i));
      m_zones[i].setVersion(version);
    }
    m_actualSink=m_zones[EPUBHTMLTextZone::Z_Main].getNewSink();
  }
  //! destructor
  ~EPUBHTMLGeneratorImpl()
  {
  }

  //! returns the actual output ( sending delayed data if needed)
  EPUBXMLContent &output(bool sendDelayed=true)
  {
    if (sendDelayed)
      m_actualSink->flush();
    return m_actualSink->getContent();
  }
  //! returns the actual sink
  TextZoneSink &getSink()
  {
    return *m_actualSink;
  }
  void push(EPUBHTMLTextZone::Type type)
  {
    m_sinkStack.push(std::move(m_actualSink));
    if (type==EPUBHTMLTextZone::Z_Main || type==EPUBHTMLTextZone::Z_NumZones)
    {
      EPUBGEN_DEBUG_MSG(("EPUBHTMLGeneratorImpl::push: can not push the main zone\n"));
      type=EPUBHTMLTextZone::Z_Unknown;
    }
    m_actualSink=m_zones[type].getNewSink();
  }
  void pop()
  {
    if (m_sinkStack.empty())
    {
      EPUBGEN_DEBUG_MSG(("EPUBHTMLGenerator::pop: can not pop sink\n"));
      return;
    }
    if (m_actualSink)
    {
      m_actualSink->send();
      m_actualSink.reset();
    }
    m_actualSink = std::move(m_sinkStack.top());
    m_sinkStack.pop();
  }
  void sendMetaData(EPUBXMLContent &out)
  {
    m_zones[EPUBHTMLTextZone::Z_MetaData].send(out);
  }
  void flushUnsent(EPUBXMLContent &out)
  {
    if (!m_sinkStack.empty())
    {
      EPUBGEN_DEBUG_MSG(("EPUBHTMLGenerator::flushUnsent: the sink stack is not empty\n"));
      while (!m_sinkStack.empty())
        pop();
    }
    // first send the main data
    if (!m_actualSink)
    {
      EPUBGEN_DEBUG_MSG(("EPUBHTMLGenerator::flushUnsent: can not find the main block\n"));
    }
    else
      m_actualSink->sendMain(out);
    m_zones[EPUBHTMLTextZone::Z_Comment].send(out);
    m_zones[EPUBHTMLTextZone::Z_FootNote].send(out);
    m_zones[EPUBHTMLTextZone::Z_EndNote].send(out);
    m_zones[EPUBHTMLTextZone::Z_TextBox].send(out);
  }

  EPUBXMLContent &m_document;
  EPUBImageManager &m_imageManager;
  EPUBFontManager &m_fontManager;
  EPUBListStyleManager &m_listManager;
  EPUBParagraphStyleManager &m_paragraphManager;
  EPUBSpanStyleManager &m_spanManager;
  EPUBSpanStyleManager &m_bodyManager;
  EPUBTableStyleManager &m_tableManager;
  const EPUBPath m_path;
  const EPUBPath m_stylesheetPath;

  int m_actualPage;
  RVNGPropertyList m_actualPageProperties;
  bool m_ignore;
  /// Does the currently opened paragraph have some text?
  bool m_hasText;
  int m_version;

  std::stack<std::string> m_frameAnchorTypes;
  std::stack<RVNGPropertyList> m_framePropertiesStack;
  /// This is used for links which don't have a href.
  std::stack<RVNGPropertyList> m_linkPropertiesStack;
  std::stack<RVNGPropertyList> m_paragraphAttributesStack;
  std::stack<RVNGPropertyList> m_spanAttributesStack;

  /// This is set when the span has ruby text and should be wrapped in <ruby></ruby>.
  std::string m_rubyText;

  EPUBStylesMethod m_stylesMethod;
  EPUBLayoutMethod m_layoutMethod;

protected:
  std::unique_ptr<TextZoneSink> m_actualSink;
  std::stack<std::unique_ptr<TextZoneSink>> m_sinkStack;

  EPUBHTMLTextZone m_zones[EPUBHTMLTextZone::Z_NumZones];
private:
  EPUBHTMLGeneratorImpl(EPUBHTMLGeneratorImpl const &orig);
  EPUBHTMLGeneratorImpl operator=(EPUBHTMLGeneratorImpl const &orig);
};

EPUBHTMLGenerator::EPUBHTMLGenerator(EPUBXMLContent &document, EPUBImageManager &imageManager, EPUBFontManager &fontManager, EPUBListStyleManager &listStyleManager, EPUBParagraphStyleManager &paragraphStyleManager, EPUBSpanStyleManager &spanStyleManager, EPUBSpanStyleManager &bodyStyleManager, EPUBTableStyleManager &tableStyleManager, const EPUBPath &path, const EPUBPath &stylesheetPath, EPUBStylesMethod stylesMethod, EPUBLayoutMethod layoutMethod, int version)
  : m_impl(new EPUBHTMLGeneratorImpl(document, imageManager, fontManager, listStyleManager, paragraphStyleManager, spanStyleManager, bodyStyleManager, tableStyleManager, path, stylesheetPath, stylesMethod, layoutMethod, version))
{
}

EPUBHTMLGenerator::~EPUBHTMLGenerator()
{
}

void EPUBHTMLGenerator::setDocumentMetaData(const RVNGPropertyList &propList)
{
  m_impl->push(EPUBHTMLTextZone::Z_MetaData);
  EPUBXMLContent &meta=m_impl->output();
  static char const *wpdMetaFields[9]=
  {
    "meta:initial-creator", "dc:creator", "dc:subject", "dc:publisher", "meta:keywords",
    "dc:language", "dc:description", "librevenge:descriptive-name", "librevenge:descriptive-type"
  };
  static char const *metaFields[9]=
  {
    "author", "typist", "subject", "publisher", "keywords",
    "language", "abstract", "descriptive-name", "descriptive-type"
  };
  for (int i = 0; i < 9; i++)
  {
    if (!propList[wpdMetaFields[i]])
      continue;
    RVNGPropertyList attrs;
    attrs.insert("name", metaFields[i]);
    attrs.insert("content", propList[wpdMetaFields[i]]->getStr());
    meta.openElement("meta", attrs);
    meta.closeElement("meta");
  }
  meta.openElement("title", RVNGPropertyList());
  if (propList["librevenge:descriptive-name"])
    meta.insertCharacters(propList["librevenge:descriptive-name"]->getStr());
  meta.closeElement("title");
  m_impl->pop();
}

void EPUBHTMLGenerator::startDocument(const RVNGPropertyList &)
{
}

void EPUBHTMLGenerator::endDocument()
{
  RVNGPropertyList htmlAttrs;
  // TODO: set lang and xml:lang from metadata
  htmlAttrs.insert("xmlns", "http://www.w3.org/1999/xhtml");
  m_impl->m_document.openElement("html", htmlAttrs);
  m_impl->m_document.openElement("head", RVNGPropertyList());
  if (m_impl->m_version < 30)
  {
    m_impl->m_document.openElement("title", RVNGPropertyList());
    m_impl->m_document.closeElement("title");
  }
  RVNGPropertyList metaAttrs;
  metaAttrs.insert("http-equiv", "content-type");
  metaAttrs.insert("content", "text/html; charset=UTF-8");
  m_impl->m_document.openElement("meta", metaAttrs);
  m_impl->m_document.closeElement("meta");
  if (m_impl->m_version >= 30 && m_impl->m_layoutMethod == EPUB_LAYOUT_METHOD_FIXED)
  {
    metaAttrs.clear();
    metaAttrs.insert("name", "viewport");
    std::stringstream content;
    if (const librevenge::RVNGProperty *pageWidth = m_impl->m_actualPageProperties["fo:page-width"])
    {
      content << "width=";
      content << inchToCSSPixel(pageWidth);
    }
    if (const librevenge::RVNGProperty *pageHeight = m_impl->m_actualPageProperties["fo:page-height"])
    {
      content << ", height=";
      content << inchToCSSPixel(pageHeight);
    }
    metaAttrs.insert("content", content.str().c_str());
    m_impl->m_document.openElement("meta", metaAttrs);
    m_impl->m_document.closeElement("meta");
  }
  if (m_impl->m_version < 30)
    m_impl->sendMetaData(m_impl->m_document);
  RVNGPropertyList linkAttrs;
  linkAttrs.insert("href", m_impl->m_stylesheetPath.relativeTo(m_impl->m_path).str().c_str());
  linkAttrs.insert("type", "text/css");
  linkAttrs.insert("rel", "stylesheet");
  m_impl->m_document.insertEmptyElement("link", linkAttrs);
  m_impl->m_document.closeElement("head");
  RVNGPropertyList bodyAttrs;
  if (m_impl->m_version >= 30)
    bodyAttrs.insert("xmlns:epub", "http://www.idpf.org/2007/ops");

  if (m_impl->m_actualPageProperties["style:writing-mode"])
  {
    switch (m_impl->m_stylesMethod)
    {
    case EPUB_STYLES_METHOD_CSS:
      bodyAttrs.insert("class", m_impl->m_bodyManager.getClass(m_impl->m_actualPageProperties).c_str());
      break;
    case EPUB_STYLES_METHOD_INLINE:
      bodyAttrs.insert("style", m_impl->m_bodyManager.getStyle(m_impl->m_actualPageProperties).c_str());
      break;
    }
  }

  m_impl->m_document.openElement("body", bodyAttrs);
  m_impl->flushUnsent(m_impl->m_document);
  m_impl->m_document.closeElement("body");
  m_impl->m_document.closeElement("html");
}

void EPUBHTMLGenerator::defineEmbeddedFont(const RVNGPropertyList &propList)
{
  m_impl->m_fontManager.insert(propList, m_impl->m_path);
}

void EPUBHTMLGenerator::openPageSpan(const RVNGPropertyList &propList)
{
  m_impl->m_actualPage++;

  librevenge::RVNGPropertyList::Iter i(propList);
  for (i.rewind(); i.next();)
    m_impl->m_actualPageProperties.insert(i.key(), i()->clone());
}

void EPUBHTMLGenerator::closePageSpan()
{
}

void EPUBHTMLGenerator::definePageStyle(const RVNGPropertyList &) {}

void EPUBHTMLGenerator::openHeader(const RVNGPropertyList & /* propList */)
{
  m_impl->m_ignore = true;
}

void EPUBHTMLGenerator::closeHeader()
{
  m_impl->m_ignore = false;
}


void EPUBHTMLGenerator::openFooter(const RVNGPropertyList & /* propList */)
{
  m_impl->m_ignore = true;
}

void EPUBHTMLGenerator::closeFooter()
{
  m_impl->m_ignore = false;
}

void EPUBHTMLGenerator::defineSectionStyle(const RVNGPropertyList &) {}

void EPUBHTMLGenerator::openSection(const RVNGPropertyList & /* propList */)
{
  // Once output is produced here, EPUBTextGenerator::openSection() will need
  // to call EPUBSplitGuard::openLevel().
}

void EPUBHTMLGenerator::closeSection()
{
  // Once output is produced here, EPUBTextGenerator::closeSection() will need
  // to call EPUBSplitGuard::closeLevel().
}

void EPUBHTMLGenerator::defineParagraphStyle(const RVNGPropertyList &propList)
{
  m_impl->m_paragraphManager.defineParagraph(propList);
}

void EPUBHTMLGenerator::openParagraph(const RVNGPropertyList &propList)
{
  if (m_impl->m_ignore)
    return;

  RVNGPropertyList attrs;
  switch (m_impl->m_stylesMethod)
  {
  case EPUB_STYLES_METHOD_CSS:
    attrs.insert("class", m_impl->m_paragraphManager.getClass(propList).c_str());
    break;
  case EPUB_STYLES_METHOD_INLINE:
    attrs.insert("style", m_impl->m_paragraphManager.getStyle(propList).c_str());
    break;
  }
  m_impl->output(false).openElement("p", attrs);
  m_impl->m_hasText = false;

  librevenge::RVNGPropertyList::Iter i(attrs);
  RVNGPropertyList paragraphAttributes;
  for (i.rewind(); i.next();)
    paragraphAttributes.insert(i.key(), i()->clone());
  m_impl->m_paragraphAttributesStack.push(paragraphAttributes);
}

void EPUBHTMLGenerator::closeParagraph()
{
  if (m_impl->m_ignore)
    return;

  if (!m_impl->m_paragraphAttributesStack.empty())
    m_impl->m_paragraphAttributesStack.pop();

  if (!m_impl->m_hasText)
    insertSpace();

  m_impl->output().closeElement("p");
}

void EPUBHTMLGenerator::defineCharacterStyle(const RVNGPropertyList &propList)
{
  m_impl->m_spanManager.defineSpan(propList);
}

void EPUBHTMLGenerator::openSpan(const RVNGPropertyList &propList)
{
  if (m_impl->m_ignore)
    return;

  RVNGPropertyList attrs;
  switch (m_impl->m_stylesMethod)
  {
  case EPUB_STYLES_METHOD_CSS:
    attrs.insert("class", m_impl->m_spanManager.getClass(propList).c_str());
    break;
  case EPUB_STYLES_METHOD_INLINE:
    attrs.insert("style", m_impl->m_spanManager.getStyle(propList).c_str());
    break;
  }

  const librevenge::RVNGProperty *rubyText = propList["text:ruby-text"];
  if (rubyText)
  {
    m_impl->m_rubyText = rubyText->getStr().cstr();
    m_impl->output(false).openElement("ruby", attrs);
  }

  m_impl->output(false).openElement("span", attrs);

  librevenge::RVNGPropertyList::Iter i(attrs);
  RVNGPropertyList spanAttributes;
  for (i.rewind(); i.next();)
    spanAttributes.insert(i.key(), i()->clone());
  m_impl->m_spanAttributesStack.push(spanAttributes);
}

void EPUBHTMLGenerator::closeSpan()
{
  if (m_impl->m_ignore)
    return;

  if (!m_impl->m_spanAttributesStack.empty())
    m_impl->m_spanAttributesStack.pop();

  m_impl->output().closeElement("span");

  if (m_impl->m_rubyText.length())
  {
    m_impl->output().openElement("rt");
    m_impl->output().insertCharacters(m_impl->m_rubyText.c_str());
    m_impl->output().closeElement("rt");
    m_impl->output().closeElement("ruby");
    m_impl->m_hasText = true;
    m_impl->m_rubyText.clear();
  }
}

void EPUBHTMLGenerator::openLink(const RVNGPropertyList &propList)
{
  if (m_impl->m_ignore)
    return;

  if (!propList["xlink:type"])
  {
    EPUBGEN_DEBUG_MSG(("EPUBHTMLGenerator::openLink: xlink:type: not filled, suppose link\n"));
  }
  RVNGPropertyList attrs;
  if (propList["xlink:href"])
  {
    std::string href(propList["xlink:href"]->getStr().cstr());

    // Basic URL sanitization.
    boost::replace_all(href, "http:///", "http://");
    boost::replace_all(href, "https:///", "https://");

    attrs.insert("href", href.c_str());
  }
  const librevenge::RVNGProperty *binaryDataProp = propList["office:binary-data"];
  const librevenge::RVNGProperty *mimeTypeProp = propList["librevenge:mime-type"];
  if (binaryDataProp && mimeTypeProp)
  {
    // This is not a real link, but more an additional image on top of an
    // existing one, map it to footnotes instead.
    RVNGPropertyList linkProperties;
    linkProperties.insert("office:binary-data", binaryDataProp->clone());
    linkProperties.insert("librevenge:mime-type", mimeTypeProp->clone());
    m_impl->m_linkPropertiesStack.push(linkProperties);
  }
  else
    // Implicit sendDelayed=true, so that in case the link is at the start of a
    // footnote, links are not nested.
    m_impl->output().openElement("a", attrs);
}

void EPUBHTMLGenerator::closeLink()
{
  if (m_impl->m_ignore)
    return;
  if (!m_impl->m_linkPropertiesStack.empty())
    m_impl->m_linkPropertiesStack.pop();
  else
    m_impl->output().closeElement("a");
}

void EPUBHTMLGenerator::insertTab()
{
  if (m_impl->m_ignore)
    return;

  // \t would not have a lot of effect since tabs in html are ignorable
  // white-space. Write NBSPs and a breakable space instead.
  for (int i = 0; i < 15; ++i)
    m_impl->output().insertCharacters("\xc2\xa0");
  m_impl->output().insertCharacters(" ");
  m_impl->m_hasText = true;
}

void EPUBHTMLGenerator::insertLineBreak()
{
  if (m_impl->m_ignore)
    return;
  m_impl->output().openElement("br", RVNGPropertyList());
  m_impl->output().closeElement("br");
}

void EPUBHTMLGenerator::insertField(const RVNGPropertyList & /* propList */)
{
  if (m_impl->m_ignore)
    return;
  m_impl->output().insertCharacters("#");
}

void EPUBHTMLGenerator::insertText(const RVNGString &text)
{
  if (m_impl->m_ignore)
    return;
  EPUBXMLContent &output = openPopup();
  output.insertCharacters(text);
  closePopup(output);
  m_impl->m_hasText = true;
}

void EPUBHTMLGenerator::insertSpace()
{
  if (m_impl->m_ignore)
    return;
  // NBSP.
  m_impl->output().insertCharacters("\xc2\xa0");
}

void EPUBHTMLGenerator::openOrderedListLevel(const RVNGPropertyList &propList)
{
  if (m_impl->m_ignore)
    return;
  m_impl->m_listManager.defineLevel(propList, true);
  RVNGPropertyList attrs;
  attrs.insert("class", m_impl->m_listManager.openLevel(propList, true).c_str());
  // fixme: if level is > 1, we must first insert a div here
  m_impl->output(false).openElement("ol", attrs);
}

void EPUBHTMLGenerator::closeOrderedListLevel()
{
  if (m_impl->m_ignore)
    return;
  m_impl->m_listManager.closeLevel();
  m_impl->output().closeElement("ol");
}

void EPUBHTMLGenerator::openUnorderedListLevel(const RVNGPropertyList &propList)
{
  if (m_impl->m_ignore)
    return;
  m_impl->m_listManager.defineLevel(propList, false);
  RVNGPropertyList attrs;
  attrs.insert("class", m_impl->m_listManager.openLevel(propList, false).c_str());
  // fixme: if level is > 1, we must first insert a div here
  m_impl->output(false).openElement("ul", attrs);
}

void EPUBHTMLGenerator::closeUnorderedListLevel()
{
  if (m_impl->m_ignore)
    return;
  m_impl->m_listManager.closeLevel();
  m_impl->output().closeElement("ul");
}


void EPUBHTMLGenerator::openListElement(const RVNGPropertyList &propList)
{
  if (m_impl->m_ignore)
    return;
  RVNGPropertyList attrs;
  attrs.insert("class", m_impl->m_listManager.getClass(propList).c_str());
  m_impl->output(false).openElement("li", attrs);
}

void EPUBHTMLGenerator::closeListElement()
{
  if (m_impl->m_ignore)
    return;
  m_impl->output().closeElement("li");
}

void EPUBHTMLGenerator::openFootnote(const RVNGPropertyList &propList)
{
  if (m_impl->m_ignore)
    return;
  EPUBXMLContent &output = m_impl->output();
  m_impl->push(EPUBHTMLTextZone::Z_FootNote);
  librevenge::RVNGString number;
  if (const librevenge::RVNGProperty *numProp = propList["librevenge:number"])
    number = numProp->getStr();
  bool closeAnchor = m_impl->m_linkPropertiesStack.empty();
  m_impl->getSink().addLabel(output, number, closeAnchor);
}

void EPUBHTMLGenerator::closeFootnote()
{
  if (m_impl->m_ignore)
    return;
  if (m_impl->m_version >= 30)
    m_impl->output().closeElement("aside");
  m_impl->pop();
}

void EPUBHTMLGenerator::openEndnote(const RVNGPropertyList &)
{
  if (m_impl->m_ignore)
    return;
  EPUBXMLContent &output = m_impl->output();
  m_impl->push(EPUBHTMLTextZone::Z_EndNote);
  m_impl->getSink().addLabel(output, librevenge::RVNGString(), true);
}

void EPUBHTMLGenerator::closeEndnote()
{
  if (m_impl->m_ignore)
    return;
  m_impl->pop();
}

void EPUBHTMLGenerator::openComment(const RVNGPropertyList & /*propList*/)
{
  if (m_impl->m_ignore)
    return;
  EPUBXMLContent &output = m_impl->output();
  m_impl->push(EPUBHTMLTextZone::Z_Comment);
  m_impl->getSink().addLabel(output, librevenge::RVNGString(), true);
}

void EPUBHTMLGenerator::closeComment()
{
  if (m_impl->m_ignore)
    return;
  m_impl->pop();
}

void EPUBHTMLGenerator::openTextBox(const RVNGPropertyList & /*propList*/)
{
  if (m_impl->m_ignore)
    return;

  if (!m_impl->m_spanAttributesStack.empty())
    m_impl->output().closeElement("span");
  if (!m_impl->m_paragraphAttributesStack.empty())
    m_impl->output().closeElement("p");

  RVNGPropertyList attrs;

  if (!m_impl->m_framePropertiesStack.empty())
  {
    RVNGPropertyList &frameProperties = m_impl->m_framePropertiesStack.top();
    switch (m_impl->m_stylesMethod)
    {
    case EPUB_STYLES_METHOD_CSS:
      attrs.insert("class", m_impl->m_imageManager.getFrameClass(frameProperties).c_str());
      break;
    case EPUB_STYLES_METHOD_INLINE:
      attrs.insert("style", m_impl->m_imageManager.getFrameStyle(frameProperties).c_str());
      break;
    }
  }

  m_impl->output().openElement("div", attrs);
}

void EPUBHTMLGenerator::closeTextBox()
{
  if (m_impl->m_ignore)
    return;

  m_impl->output().closeElement("div");

  if (!m_impl->m_framePropertiesStack.empty())
  {
    RVNGPropertyList &frameProperties = m_impl->m_framePropertiesStack.top();
    RVNGString wrapStyle = m_impl->m_imageManager.getWrapStyle(frameProperties).c_str();
    if (!wrapStyle.empty())
    {
      RVNGPropertyList attrs;
      attrs.insert("style", wrapStyle);
      m_impl->output().insertEmptyElement("br", attrs);
    }
  }

  if (!m_impl->m_paragraphAttributesStack.empty())
    m_impl->output(false).openElement("p", m_impl->m_paragraphAttributesStack.top());
  if (!m_impl->m_spanAttributesStack.empty())
    m_impl->output(false).openElement("span", m_impl->m_spanAttributesStack.top());
}

void EPUBHTMLGenerator::openTable(const RVNGPropertyList &propList)
{
  if (m_impl->m_ignore)
    return;

  m_impl->m_tableManager.openTable(propList);
  RVNGPropertyList attrs;
  switch (m_impl->m_stylesMethod)
  {
  case EPUB_STYLES_METHOD_CSS:
    attrs.insert("class", m_impl->m_tableManager.getTableClass(propList).c_str());
    break;
  case EPUB_STYLES_METHOD_INLINE:
    attrs.insert("style", m_impl->m_tableManager.getTableStyle(propList).c_str());
    break;
  }
  m_impl->output().openElement("table", attrs);
  m_impl->output().openElement("tbody", RVNGPropertyList());
}

void EPUBHTMLGenerator::openTableRow(const RVNGPropertyList &propList)
{
  if (m_impl->m_ignore)
    return;
  RVNGPropertyList attrs;
  switch (m_impl->m_stylesMethod)
  {
  case EPUB_STYLES_METHOD_CSS:
    attrs.insert("class", m_impl->m_tableManager.getRowClass(propList).c_str());
    break;
  case EPUB_STYLES_METHOD_INLINE:
    attrs.insert("style", m_impl->m_tableManager.getRowStyle(propList).c_str());
    break;
  }
  m_impl->output().openElement("tr", attrs);
}

void EPUBHTMLGenerator::closeTableRow()
{
  if (m_impl->m_ignore)
    return;
  m_impl->output().closeElement("tr");
}

void EPUBHTMLGenerator::openTableCell(const RVNGPropertyList &propList)
{
  if (m_impl->m_ignore)
    return;
  RVNGPropertyList attrs;
  switch (m_impl->m_stylesMethod)
  {
  case EPUB_STYLES_METHOD_CSS:
    attrs.insert("class", m_impl->m_tableManager.getCellClass(propList).c_str());
    break;
  case EPUB_STYLES_METHOD_INLINE:
    attrs.insert("style", m_impl->m_tableManager.getCellStyle(propList).c_str());
    break;
  }
  if (propList["table:number-columns-spanned"])
    attrs.insert("colspan", propList["table:number-columns-spanned"]->getInt());
  if (propList["table:number-rows-spanned"])
    attrs.insert("rowspan", propList["table:number-rows-spanned"]->getInt());
  m_impl->output().openElement("td", attrs);

}

void EPUBHTMLGenerator::closeTableCell()
{
  if (m_impl->m_ignore)
    return;
  m_impl->output().closeElement("td");
}

void EPUBHTMLGenerator::insertCoveredTableCell(const RVNGPropertyList & /* propList */) {}

void EPUBHTMLGenerator::closeTable()
{
  if (m_impl->m_ignore)
    return;
  m_impl->output().closeElement("tbody");
  m_impl->output().closeElement("table");
  m_impl->m_tableManager.closeTable();
}

void EPUBHTMLGenerator::openFrame(const RVNGPropertyList &propList)
{
  librevenge::RVNGPropertyList::Iter i(propList);
  std::string anchorType;
  RVNGPropertyList frameProperties;
  for (i.rewind(); i.next();)
  {
    if (std::string("text:anchor-type") == i.key())
      anchorType = i()->getStr().cstr();

    // Remember the property for binary object purposes.
    frameProperties.insert(i.key(), i()->clone());
  }

  if (anchorType == "page")
    // Other anchor types are already inside a paragraph.
    m_impl->output().openElement("p", RVNGPropertyList());
  m_impl->m_frameAnchorTypes.push(anchorType);

  m_impl->m_framePropertiesStack.push(frameProperties);
}

void EPUBHTMLGenerator::closeFrame()
{
  if (!m_impl->m_framePropertiesStack.empty())
    m_impl->m_framePropertiesStack.pop();

  if (m_impl->m_frameAnchorTypes.empty())
    return;

  if (m_impl->m_frameAnchorTypes.top() == "page")
    m_impl->output().closeElement("p");

  m_impl->m_frameAnchorTypes.pop();
}

void EPUBHTMLGenerator::openGroup(const librevenge::RVNGPropertyList & /* propList */) {}
void EPUBHTMLGenerator::closeGroup() {}

void EPUBHTMLGenerator::defineGraphicStyle(const librevenge::RVNGPropertyList & /* propList */) {}
void EPUBHTMLGenerator::drawRectangle(const librevenge::RVNGPropertyList & /* propList */) {}
void EPUBHTMLGenerator::drawEllipse(const librevenge::RVNGPropertyList & /* propList */) {}
void EPUBHTMLGenerator::drawPolygon(const librevenge::RVNGPropertyList & /* propList */) {}
void EPUBHTMLGenerator::drawPolyline(const librevenge::RVNGPropertyList & /* propList */) {}
void EPUBHTMLGenerator::drawPath(const librevenge::RVNGPropertyList & /* propList */) {}
void EPUBHTMLGenerator::drawConnector(const librevenge::RVNGPropertyList & /* propList */) {}

void EPUBHTMLGenerator::insertBinaryObject(const RVNGPropertyList &propList)
{
  const EPUBPath &path = m_impl->m_imageManager.insert(
                           RVNGBinaryData(propList["office:binary-data"]->getStr()),
                           propList["librevenge:mime-type"]->getStr());

  RVNGPropertyList attrs;
  RVNGString wrapStyle;

  if (!m_impl->m_framePropertiesStack.empty())
  {
    RVNGPropertyList &frameProperties = m_impl->m_framePropertiesStack.top();
    switch (m_impl->m_stylesMethod)
    {
    case EPUB_STYLES_METHOD_CSS:
      attrs.insert("class", m_impl->m_imageManager.getFrameClass(frameProperties).c_str());
      break;
    case EPUB_STYLES_METHOD_INLINE:
      attrs.insert("style", m_impl->m_imageManager.getFrameStyle(frameProperties).c_str());
      break;
    }

    wrapStyle = m_impl->m_imageManager.getWrapStyle(frameProperties).c_str();
  }

  attrs.insert("src", path.relativeTo(m_impl->m_path).str().c_str());
  // FIXME: use alternative repr. if available
  attrs.insert("alt", path.str().c_str());
  EPUBXMLContent &popup = openPopup();
  popup.insertEmptyElement("img", attrs);
  closePopup(popup);

  if (!wrapStyle.empty())
  {
    attrs.clear();
    attrs.insert("style", wrapStyle);
    m_impl->output().insertEmptyElement("br", attrs);
  }
}

EPUBXMLContent &EPUBHTMLGenerator::openPopup()
{
  if (!m_impl->m_linkPropertiesStack.empty())
  {
    // Save the main sink, as m_impl->output() will point to the footnote sink.
    libepubgen::EPUBXMLContent &main = m_impl->output();
    openFootnote(RVNGPropertyList());
    return main;
  }
  else
    return m_impl->output();
}

void EPUBHTMLGenerator::closePopup(EPUBXMLContent &main)
{
  if (!m_impl->m_linkPropertiesStack.empty())
  {
    RVNGPropertyList &linkProperties = m_impl->m_linkPropertiesStack.top();
    main.closeElement("a");
    const EPUBPath &linkPath = m_impl->m_imageManager.insert(
                                 RVNGBinaryData(linkProperties["office:binary-data"]->getStr()),
                                 linkProperties["librevenge:mime-type"]->getStr());
    RVNGPropertyList linkAttrs;
    linkAttrs.insert("src", linkPath.relativeTo(m_impl->m_path).str().c_str());
    linkAttrs.insert("alt", linkPath.str().c_str());
    m_impl->output().insertEmptyElement("img", linkAttrs);
    closeFootnote();
  }
}

void EPUBHTMLGenerator::insertEquation(const RVNGPropertyList & /* propList */) {}

void EPUBHTMLGenerator::getPageProperties(librevenge::RVNGPropertyList &propList) const
{
  propList.clear();
  librevenge::RVNGPropertyList::Iter i(m_impl->m_actualPageProperties);
  for (i.rewind(); i.next();)
    propList.insert(i.key(), i()->clone());
}

void EPUBHTMLGenerator::setPageProperties(const librevenge::RVNGPropertyList &propList)
{
  m_impl->m_actualPageProperties.clear();
  librevenge::RVNGPropertyList::Iter i(propList);
  for (i.rewind(); i.next();)
    m_impl->m_actualPageProperties.insert(i.key(), i()->clone());
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
