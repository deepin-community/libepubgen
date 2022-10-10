/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "EPUBGenerator.h"

#include <ctime>
#include <sstream>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "EPUBCSSContent.h"
#include "EPUBHTMLGenerator.h"
#include "EPUBXMLContent.h"

namespace libepubgen
{

using librevenge::RVNGPropertyFactory;
using librevenge::RVNGPropertyList;
using librevenge::RVNGString;

EPUBGenerator::EPUBGenerator(EPUBPackage *const package, int version)
  : m_package(package)
  , m_manifest()
  , m_htmlManager(m_manifest)
  , m_imageManager(m_manifest)
  , m_fontManager(m_manifest)
  , m_listStyleManager()
  , m_paragraphStyleManager()
  , m_spanStyleManager("span")
  , m_bodyStyleManager("body")
  , m_tableStyleManager()
  , m_stylesheetPath("OEBPS/styles/stylesheet.css")
  , m_documentProps()
  , m_metadata()
  , m_currentHtml()
  , m_splitGuard(EPUB_SPLIT_METHOD_PAGE_BREAK,true)
  , m_version(version)
  , m_stylesMethod(EPUB_STYLES_METHOD_CSS)
  , m_layoutMethod(EPUB_LAYOUT_METHOD_REFLOWABLE)
{
}

EPUBGenerator::~EPUBGenerator()
{
}

void EPUBGenerator::startDocument(const RVNGPropertyList &props)
{
  m_documentProps = props;

  startNewHtmlFile();

  if (m_version >= 30)
    m_manifest.insert(EPUBPath("OEBPS/toc.xhtml"), "application/xhtml+xml", "toc.xhtml", "nav");
  m_manifest.insert(EPUBPath("OEBPS/toc.ncx"), "application/x-dtbncx+xml", "toc.ncx", "");
  m_manifest.insert(m_stylesheetPath, "text/css", "stylesheet.css", "");
}

void EPUBGenerator::endDocument()
{
  if (bool(m_currentHtml))
  {
    endHtmlFile();
    m_currentHtml->endDocument();
  }

  writeContainer();
  writeRoot();
  writeNavigation();
  writeStylesheet();
  m_htmlManager.writeTo(*m_package);
  m_imageManager.writeTo(*m_package);
  m_fontManager.writeTo(*m_package);
}

void EPUBGenerator::setDocumentMetaData(const RVNGPropertyList &props)
{
  m_metadata = props;

  if (m_version >= 30)
  {
    const librevenge::RVNGPropertyListVector *coverImages = props.child("librevenge:cover-images");
    if (coverImages)
    {
      for (size_t i = 0; i < coverImages->count(); ++i)
      {
        librevenge::RVNGPropertyList const &propertyList = (*coverImages)[i];
        if (propertyList["office:binary-data"] && propertyList["librevenge:mime-type"])
        {
          m_imageManager.insert(librevenge::RVNGBinaryData(propertyList["office:binary-data"]->getStr()),
                                propertyList["librevenge:mime-type"]->getStr(),
                                "cover-image");
        }
      }
    }
  }
}

void EPUBGenerator::startNewHtmlFile()
{
  // close the current HTML file
  librevenge::RVNGPropertyList pageProperties;
  if (bool(m_currentHtml))
  {
    endHtmlFile();
    m_currentHtml->endDocument();
    m_currentHtml->getPageProperties(pageProperties);
  }

  m_splitGuard.onSplit();

  m_currentHtml = m_htmlManager.create(m_imageManager, m_fontManager, m_listStyleManager, m_paragraphStyleManager, m_spanStyleManager, m_bodyStyleManager, m_tableStyleManager, m_stylesheetPath, m_stylesMethod, m_layoutMethod, m_version);

  // Splitted html file should keep the same page property.
  m_currentHtml->setPageProperties(pageProperties);

  // restore state in the new file
  m_currentHtml->startDocument(m_documentProps);
  m_currentHtml->setDocumentMetaData(m_metadata);

  startHtmlFile();
}

const std::shared_ptr<EPUBHTMLGenerator> &EPUBGenerator::getHtml() const
{
  return m_currentHtml;
}

EPUBHTMLManager &EPUBGenerator::getHtmlManager()
{
  return m_htmlManager;
}

const EPUBSplitGuard &EPUBGenerator::getSplitGuard() const
{
  return m_splitGuard;
}

EPUBSplitGuard &EPUBGenerator::getSplitGuard()
{
  return m_splitGuard;
}

int EPUBGenerator::getVersion() const
{
  return m_version;
}

void EPUBGenerator::setSplitMethod(EPUBSplitMethod split)
{
  m_splitGuard.setSplitMethod(split);
}

void EPUBGenerator::setStylesMethod(EPUBStylesMethod styles)
{
  m_stylesMethod = styles;
}

void EPUBGenerator::setLayoutMethod(EPUBLayoutMethod layout)
{
  m_layoutMethod = layout;
  if (m_layoutMethod == EPUB_LAYOUT_METHOD_FIXED)
    // Fixed layout implies split on page break.
    m_splitGuard.setSplitMethod(EPUB_SPLIT_METHOD_PAGE_BREAK);

  m_splitGuard.setSplitOnSecondPageSpan(m_layoutMethod == EPUB_LAYOUT_METHOD_REFLOWABLE);
}

void EPUBGenerator::writeContainer()
{
  EPUBXMLContent xml;

  RVNGPropertyList containerAttrs;
  containerAttrs.insert("version", RVNGPropertyFactory::newStringProp("1.0"));
  containerAttrs.insert("xmlns", "urn:oasis:names:tc:opendocument:xmlns:container");

  xml.openElement("container", containerAttrs);
  xml.openElement("rootfiles");

  RVNGPropertyList rootfileAttrs;
  rootfileAttrs.insert("full-path", "OEBPS/content.opf");
  rootfileAttrs.insert("media-type", "application/oebps-package+xml");

  xml.insertEmptyElement("rootfile", rootfileAttrs);

  xml.closeElement("rootfiles");
  xml.closeElement("container");

  xml.writeTo(*m_package, "META-INF/container.xml");
}

void EPUBGenerator::writeNavigation()
{
  if (m_version >= 30)
  {
    EPUBXMLContent xml;

    const EPUBPath path("OEBPS/toc.xhtml");
    RVNGPropertyList htmlAttrs;
    htmlAttrs.insert("xmlns", "http://www.w3.org/1999/xhtml");
    htmlAttrs.insert("xmlns:epub", "http://www.idpf.org/2007/ops");
    xml.openElement("html", htmlAttrs);

    xml.openElement("head");
    xml.closeElement("head");
    xml.openElement("body");

    RVNGPropertyList navAttrs;
    navAttrs.insert("epub:type", "toc");
    xml.openElement("nav", navAttrs);

    xml.openElement("ol");
    m_htmlManager.writeTocTo(xml, path, m_version, m_layoutMethod);
    xml.closeElement("ol");

    xml.closeElement("nav");
    xml.closeElement("body");
    xml.closeElement("html");

    xml.writeTo(*m_package, path.str().c_str());
  }

  EPUBXMLContent xml;

  const EPUBPath path("OEBPS/toc.ncx");
  RVNGPropertyList ncxAttrs;
  ncxAttrs.insert("xmlns", "http://www.daisy.org/z3986/2005/ncx/");
  ncxAttrs.insert("version", "2005-1");

  xml.openElement("ncx", ncxAttrs);

  xml.openElement("head");
  RVNGPropertyList metaAttrs;
  metaAttrs.insert("name", "");
  metaAttrs.insert("content", "");
  metaAttrs.insert("scheme", "");
  xml.insertEmptyElement("meta", metaAttrs);
  xml.closeElement("head");
  xml.openElement("docTitle");
  xml.openElement("text");
  xml.closeElement("text");
  xml.closeElement("docTitle");

  xml.openElement("navMap");
  // In case of EPUB3 the (deprecated, but valid) EPUB2 markup is wanted, so
  // the version is unconditional here.
  m_htmlManager.writeTocTo(xml, path, /*version=*/20, m_layoutMethod);
  xml.closeElement("navMap");

  xml.closeElement("ncx");

  xml.writeTo(*m_package, path.str().c_str());
}

void EPUBGenerator::writeStylesheet()
{
  EPUBCSSContent stylesheet;

  m_fontManager.send(stylesheet);
  m_listStyleManager.send(stylesheet);
  m_paragraphStyleManager.send(stylesheet);
  m_spanStyleManager.send(stylesheet);
  m_bodyStyleManager.send(stylesheet);
  m_tableStyleManager.send(stylesheet);
  m_imageManager.send(stylesheet);

  stylesheet.writeTo(*m_package, m_stylesheetPath.str().c_str());
}

void EPUBGenerator::writeRoot()
{
  EPUBXMLContent sink;

  const RVNGString uniqueId("unique-identifier");

  RVNGPropertyList packageAttrs;
  packageAttrs.insert("xmlns", "http://www.idpf.org/2007/opf");
  packageAttrs.insert("xmlns:dc", "http://purl.org/dc/elements/1.1/");
  packageAttrs.insert("xmlns:dcterms", "http://purl.org/dc/terms/");
  packageAttrs.insert("xmlns:opf", "http://www.idpf.org/2007/opf");
  if (m_version >= 30)
    packageAttrs.insert("version", RVNGPropertyFactory::newStringProp("3.0"));
  else
    packageAttrs.insert("version", RVNGPropertyFactory::newStringProp("2.0"));
  packageAttrs.insert("unique-identifier", uniqueId);

  sink.openElement("package", packageAttrs);

  sink.openElement("metadata");

  RVNGPropertyList identifierAttrs;
  identifierAttrs.insert("id", uniqueId);
  sink.openElement("dc:identifier", identifierAttrs);
  // The identifier element is required to have a unique character content.
  std::stringstream identifierStream("urn:uuid:");
  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  identifierStream << uuid;
  std::string identifierCharactrs = identifierStream.str();
  RVNGString identifier = identifierCharactrs.c_str();
  if (m_metadata["dc:identifier"] && !m_metadata["dc:identifier"]->getStr().empty())
    identifier = m_metadata["dc:identifier"]->getStr();
  sink.insertCharacters(identifier);
  sink.closeElement("dc:identifier");

  RVNGString title("Unknown Title");
  if (m_metadata["dc:title"] && !m_metadata["dc:title"]->getStr().empty())
    title = m_metadata["dc:title"]->getStr();
  sink.openElement("dc:title");
  sink.insertCharacters(title);
  sink.closeElement("dc:title");

  RVNGString creator("Unknown Author");
  if (m_metadata["meta:initial-creator"] && !m_metadata["meta:initial-creator"]->getStr().empty())
    creator = m_metadata["meta:initial-creator"]->getStr();
  sink.openElement("dc:creator");
  sink.insertCharacters(creator);
  sink.closeElement("dc:creator");

  RVNGString language("en");
  if (m_metadata["dc:language"] && !m_metadata["dc:language"]->getStr().empty())
    language = m_metadata["dc:language"]->getStr();
  sink.openElement("dc:language");
  sink.insertCharacters(language);
  sink.closeElement("dc:language");

  if (m_version >= 30)
  {
    RVNGString date;
    time_t now = 0;
    time(&now);
    const struct tm *local = localtime(&now);
    if (local)
    {
      const int MAX_BUFFER = 1024;
      char buffer[MAX_BUFFER];
      strftime(&buffer[0], MAX_BUFFER-1, "%Y-%m-%dT%H:%M:%SZ", local);
      date.append(buffer);
    }

    if (m_metadata["dc:date"] && !m_metadata["dc:date"]->getStr().empty())
    {
      // Expecting CCYY-MM-DDThh:mm:ssZ, librevenge provides CCYY-MM-DDThh:mm:ss.sssssssss
      date = std::string(m_metadata["dc:date"]->getStr().cstr()).substr(0, 19).c_str();
      date.append("Z");
    }

    RVNGPropertyList metaAttrs;
    metaAttrs.insert("property", "dcterms:modified");
    sink.openElement("meta", metaAttrs);
    sink.insertCharacters(date);
    sink.closeElement("meta");

#ifdef VERSION
    const std::string version(VERSION);
#else
    const std::string version("unknown");
#endif
    std::string generator;
    if (m_metadata["meta:generator"])
      generator = m_metadata["meta:generator"]->getStr().cstr();

    if (generator.empty())
      generator = "libepubgen/" + version;
    else
    {
      generator += " (";
      generator += "libepubgen/" + version;
      generator += ")";
    }

    metaAttrs.clear();
    metaAttrs.insert("name", "generator");
    metaAttrs.insert("content", generator.c_str());
    sink.openElement("meta", metaAttrs);
    sink.closeElement("meta");

    if (m_layoutMethod == EPUB_LAYOUT_METHOD_FIXED)
    {
      metaAttrs.clear();
      metaAttrs.insert("property", "rendition:layout");
      sink.openElement("meta", metaAttrs);
      sink.insertCharacters("pre-paginated");
      sink.closeElement("meta");
    }
  }

  sink.closeElement("metadata");

  sink.openElement("manifest");
  m_manifest.writeTo(sink);
  sink.closeElement("manifest");

  RVNGPropertyList spineAttrs;
  spineAttrs.insert("toc", "toc.ncx");

  sink.openElement("spine", spineAttrs);
  m_htmlManager.writeSpineTo(sink);
  sink.closeElement("spine");

  sink.closeElement("package");

  sink.writeTo(*m_package, "OEBPS/content.opf");
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
