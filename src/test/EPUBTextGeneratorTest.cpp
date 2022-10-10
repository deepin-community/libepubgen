/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <algorithm>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <libxml/parser.h>
#include <libxml/xmlwriter.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <libepubgen/EPUBPackage.h>
#include <libepubgen/EPUBTextGenerator.h>

namespace test
{

/// A package implementation that collects all input in memory.
class StringEPUBPackage : public libepubgen::EPUBPackage
{
public:
  StringEPUBPackage();
  ~StringEPUBPackage() override;
  StringEPUBPackage(const StringEPUBPackage &) = delete;
  StringEPUBPackage &operator=(const StringEPUBPackage &) = delete;

  void openXMLFile(const char *name) override;

  void openElement(const char *name, const librevenge::RVNGPropertyList &attributes) override;
  void closeElement(const char *name) override;

  void insertCharacters(const librevenge::RVNGString &characters) override;

  void closeXMLFile() override;

  void openCSSFile(const char *name) override;

  void insertRule(const librevenge::RVNGString &selector, const librevenge::RVNGPropertyList &properties) override;

  void closeCSSFile() override;

  void openBinaryFile(const char *name) override;

  void insertBinaryData(const librevenge::RVNGBinaryData &data) override;

  void closeBinaryFile() override;

  void openTextFile(const char *name) override;

  void insertText(const librevenge::RVNGString &characters) override;
  void insertLineBreak() override;

  void closeTextFile() override;

  std::map<std::string, xmlBufferPtr> m_streams;
  std::map<std::string, std::map< std::string, std::vector<std::string> > > m_cssStreams;
  std::set<std::string> m_binaryStreams;

private:
  std::string m_currentFileName;
  xmlBufferPtr m_currentXmlBuffer;
  xmlTextWriterPtr m_currentXmlContent;
};

StringEPUBPackage::StringEPUBPackage()
  : m_streams()
  , m_cssStreams()
  , m_binaryStreams()
  , m_currentFileName()
  , m_currentXmlBuffer(nullptr)
  , m_currentXmlContent(nullptr)
{
}

StringEPUBPackage::~StringEPUBPackage()
{
  for (auto &stream : m_streams)
    xmlBufferFree(stream.second);
}

void StringEPUBPackage::openXMLFile(const char *name)
{
  CPPUNIT_ASSERT(m_currentFileName.empty());
  CPPUNIT_ASSERT(name);

  m_currentFileName = name;
  m_currentXmlBuffer = xmlBufferCreate();
  m_currentXmlContent = xmlNewTextWriterMemory(m_currentXmlBuffer, 0);
  xmlTextWriterStartDocument(m_currentXmlContent, nullptr, nullptr, nullptr);
}

void StringEPUBPackage::openElement(const char *name, const librevenge::RVNGPropertyList &attributes)
{
  CPPUNIT_ASSERT(!m_currentFileName.empty());
  CPPUNIT_ASSERT(m_currentXmlContent);

  xmlTextWriterStartElement(m_currentXmlContent, BAD_CAST(name));

  librevenge::RVNGPropertyList::Iter i(attributes);
  for (i.rewind(); i.next();)
    xmlTextWriterWriteAttribute(m_currentXmlContent, BAD_CAST(i.key()), BAD_CAST(i()->getStr().cstr()));
}

void StringEPUBPackage::closeElement(const char *)
{
  CPPUNIT_ASSERT(!m_currentFileName.empty());
  CPPUNIT_ASSERT(m_currentXmlContent);

  xmlTextWriterEndElement(m_currentXmlContent);
}

void StringEPUBPackage::insertCharacters(const librevenge::RVNGString &characters)
{
  CPPUNIT_ASSERT(!m_currentFileName.empty());
  CPPUNIT_ASSERT(m_currentXmlContent);

  xmlTextWriterWriteString(m_currentXmlContent, BAD_CAST(characters.cstr()));
}

void StringEPUBPackage::closeXMLFile()
{
  CPPUNIT_ASSERT(!m_currentFileName.empty());

  xmlTextWriterEndDocument(m_currentXmlContent);
  xmlFreeTextWriter(m_currentXmlContent);
  m_currentXmlContent = nullptr;
  m_streams[m_currentFileName] = m_currentXmlBuffer;
  m_currentXmlBuffer = nullptr;
  m_currentFileName.clear();
}

void StringEPUBPackage::openCSSFile(const char *name)
{
  CPPUNIT_ASSERT(m_currentFileName.empty());
  CPPUNIT_ASSERT(name);

  m_currentFileName = name;
}

void StringEPUBPackage::insertRule(const librevenge::RVNGString &selector, const librevenge::RVNGPropertyList &properties)
{
  std::vector<std::string> &rule = m_cssStreams[m_currentFileName][selector.cstr()];
  for (librevenge::RVNGPropertyList::Iter propIt(properties); !propIt.last(); propIt.next())
  {
    if (propIt())
      rule.emplace_back(std::string(propIt.key()) + ": " + propIt()->getStr().cstr());
  }
}

void StringEPUBPackage::closeCSSFile()
{
  CPPUNIT_ASSERT(!m_currentFileName.empty());

  m_currentFileName.clear();
}

void StringEPUBPackage::openBinaryFile(const char *name)
{
  m_binaryStreams.insert(name);
}

void StringEPUBPackage::insertBinaryData(const librevenge::RVNGBinaryData &)
{
}

void StringEPUBPackage::closeBinaryFile()
{
}

void StringEPUBPackage::openTextFile(const char *)
{
}

void StringEPUBPackage::insertText(const librevenge::RVNGString &)
{
}

void StringEPUBPackage::insertLineBreak()
{
}

void StringEPUBPackage::closeTextFile()
{
}

/// Test class covering libepubgen::EPUBTextGenerator functionality.
class EPUBTextGeneratorTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(EPUBTextGeneratorTest);
  CPPUNIT_TEST(testChapterNames);
  CPPUNIT_TEST(testChapterNames2);
  CPPUNIT_TEST(testImage);
  CPPUNIT_TEST(testImageBorder);
  CPPUNIT_TEST(testImageHorizontalPosLeft);
  CPPUNIT_TEST(testImageHorizontalPosRight);
  CPPUNIT_TEST(testImageWidth);
  CPPUNIT_TEST(testImageWidthRelative);
  CPPUNIT_TEST(testImageWrapTypes);
  CPPUNIT_TEST(testImagePopup);
  CPPUNIT_TEST(testInvalidImage);
  CPPUNIT_TEST(testBrokenImage);
  CPPUNIT_TEST(testBrokenLink);
  CPPUNIT_TEST(testSplitMethodPageBreak);
  CPPUNIT_TEST(testMetadata);
  CPPUNIT_TEST(testSpace);
  CPPUNIT_TEST(testLineThough);
  CPPUNIT_TEST(testUnderline);
  CPPUNIT_TEST(testEmptyPara);
  CPPUNIT_TEST(testTab);
  CPPUNIT_TEST(testStylesMethodInline);
  CPPUNIT_TEST(testStylesMethodInlineRowCell);
  CPPUNIT_TEST(testStylesMethodCSS);
  CPPUNIT_TEST(testRelColumnWidth);
  CPPUNIT_TEST(testRelTableWidth);
  CPPUNIT_TEST(testSection);
  CPPUNIT_TEST(testTextBox);
  CPPUNIT_TEST(testEmbeddedFont);
  CPPUNIT_TEST(testCoverImage);
  CPPUNIT_TEST(testFootnote);
  CPPUNIT_TEST(testFootnote3);
  CPPUNIT_TEST(testFootnoteAnchor);
  CPPUNIT_TEST(testTextPopup);
  CPPUNIT_TEST(testFixedLayout);
  CPPUNIT_TEST(testFixedLayoutBreakExplicit);
  CPPUNIT_TEST(testFixedLayoutSpine);
  CPPUNIT_TEST(testFixedLayoutChapters);
  CPPUNIT_TEST(testPageBreak);
  CPPUNIT_TEST(testPageBreakImage);
  CPPUNIT_TEST(testPageSpanProperties);
  CPPUNIT_TEST(testSplitOnPageBreakInPageSpan);
  CPPUNIT_TEST(testSplitOnHeadingInPageSpan);
  CPPUNIT_TEST(testSplitOnSizeInPageSpan);
  CPPUNIT_TEST(testManyWritingModes);
  CPPUNIT_TEST(testRubyElements);
  CPPUNIT_TEST_SUITE_END();

private:
  void testChapterNames();
  void testChapterNames2();
  void testImage();
  void testImageBorder();
  void testImageHorizontalPosLeft();
  void testImageHorizontalPosRight();
  void testImageWidth();
  void testImageWidthRelative();
  void testImageWrapTypes();
  void testImagePopup();
  void testInvalidImage();
  void testBrokenImage();
  void testBrokenLink();
  void testSplitMethodPageBreak();
  void testMetadata();
  void testSpace();
  void testLineThough();
  void testUnderline();
  void testEmptyPara();
  void testTab();
  void testStylesMethodInline();
  void testStylesMethodInlineRowCell();
  void testStylesMethodCSS();
  void testRelColumnWidth();
  void testRelTableWidth();
  void testSection();
  void testTextBox();
  void testEmbeddedFont();
  void testCoverImage();
  void testFootnote();
  void testFootnote3();
  void testFootnoteAnchor();
  void testTextPopup();
  void testFixedLayout();
  void testFixedLayoutBreakExplicit();
  void testFixedLayoutSpine();
  void testFixedLayoutChapters();
  void testPageBreak();
  void testPageBreakImage();
  void testPageSpanProperties();
  void testSplitOnPageBreakInPageSpan();
  void testSplitOnHeadingInPageSpan();
  void testSplitOnSizeInPageSpan();
  void testManyWritingModes();
  void testRubyElements();

  /// Asserts that exactly one xpath exists in buffer, and its content equals content.
  void assertXPathContent(xmlBufferPtr buffer, const std::string &xpath, const std::string &content, const CppUnit::SourceLine &rSourceLine);
  /// Assert that rXPath exists, and returns exactly numberOfNodes nodes.
  void assertXPath(xmlBufferPtr buffer, const std::string &xpath, int numberOfNodes, const CppUnit::SourceLine &rSourceLine);
  /// Asserts that xpath exists, returns exactly one node and xpath's attribute's value equals to expectedValue.
  void assertXPathAttribute(xmlBufferPtr buffer, const std::string &xpath, const std::string &attribute, const std::string &expectedValue, const CppUnit::SourceLine &rSourceLine);
  /// Asserts that cssDoc has a key named key and one of its rules is value.
  void assertCss(const std::map< std::string, std::vector<std::string> > &cssDoc, const std::string &key, const std::string &value, bool positive, const CppUnit::SourceLine &rSourceLine);
};

#define CPPUNIT_ASSERT_XPATH_CONTENT(buffer, xpath, content) \
  assertXPathContent(buffer, xpath, content, CPPUNIT_SOURCELINE())

#define CPPUNIT_ASSERT_XPATH(buffer, xpath, numberOfNodes) \
  assertXPath(buffer, xpath, numberOfNodes, CPPUNIT_SOURCELINE())

#define CPPUNIT_ASSERT_XPATH_ATTRIBUTE(buffer, xpath, attribute, expectedValue) \
  assertXPathAttribute(buffer, xpath, attribute, expectedValue, CPPUNIT_SOURCELINE())

#define CPPUNIT_ASSERT_CSS(cssDoc, key, value, positive) \
  assertCss(cssDoc, key, value, positive, CPPUNIT_SOURCELINE())

void EPUBTextGeneratorTest::setUp()
{
}

void EPUBTextGeneratorTest::tearDown()
{
}

namespace
{
void registerNamespaces(xmlXPathContextPtr xpathContext)
{
  xmlXPathRegisterNs(xpathContext, BAD_CAST("dc"), BAD_CAST("http://purl.org/dc/elements/1.1/"));
  xmlXPathRegisterNs(xpathContext, BAD_CAST("opf"), BAD_CAST("http://www.idpf.org/2007/opf"));
  xmlXPathRegisterNs(xpathContext, BAD_CAST("xhtml"), BAD_CAST("http://www.w3.org/1999/xhtml"));
}
}

void EPUBTextGeneratorTest::assertXPathContent(xmlBufferPtr buffer, const std::string &xpath, const std::string &content, const CppUnit::SourceLine &rSourceLine)
{
  xmlDocPtr doc = xmlParseMemory(reinterpret_cast<const char *>(xmlBufferContent(buffer)), xmlBufferLength(buffer));
  xmlXPathContextPtr xpathContext = xmlXPathNewContext(doc);
  registerNamespaces(xpathContext);
  xmlXPathObjectPtr xpathObject = xmlXPathEvalExpression(BAD_CAST(xpath.c_str()), xpathContext);
  int nActualNodes = xmlXPathNodeSetGetLength(xpathObject->nodesetval);
  CPPUNIT_NS::assertEquals<int>(1, nActualNodes, rSourceLine, "Unexpected number of nodes on XPath.");

  xmlNodePtr node = xpathObject->nodesetval->nodeTab[0];
  std::string actual(reinterpret_cast<char const *>(node->children[0].content), xmlStrlen(node->children[0].content));
  CPPUNIT_NS::assertEquals<std::string>(content, actual, rSourceLine, "Unexpected content on XPath.");

  xmlXPathFreeObject(xpathObject);
  xmlXPathFreeContext(xpathContext);
  xmlFreeDoc(doc);
}

void EPUBTextGeneratorTest::assertXPath(xmlBufferPtr buffer, const std::string &xpath, int numberOfNodes, const CppUnit::SourceLine &rSourceLine)
{
  xmlDocPtr doc = xmlParseMemory(reinterpret_cast<const char *>(xmlBufferContent(buffer)), xmlBufferLength(buffer));
  xmlXPathContextPtr xpathContext = xmlXPathNewContext(doc);
  registerNamespaces(xpathContext);
  xmlXPathObjectPtr xpathObject = xmlXPathEvalExpression(BAD_CAST(xpath.c_str()), xpathContext);
  int nActualNodes = xmlXPathNodeSetGetLength(xpathObject->nodesetval);
  CPPUNIT_NS::assertEquals<int>(numberOfNodes, nActualNodes, rSourceLine, "Unexpected number of nodes on XPath.");

  xmlXPathFreeObject(xpathObject);
  xmlXPathFreeContext(xpathContext);
  xmlFreeDoc(doc);
}

void EPUBTextGeneratorTest::assertXPathAttribute(xmlBufferPtr buffer, const std::string &xpath, const std::string &attribute, const std::string &expectedValue, const CppUnit::SourceLine &rSourceLine)
{
  xmlDocPtr doc = xmlParseMemory(reinterpret_cast<const char *>(xmlBufferContent(buffer)), xmlBufferLength(buffer));
  xmlXPathContextPtr xpathContext = xmlXPathNewContext(doc);
  registerNamespaces(xpathContext);
  xmlXPathObjectPtr xpathObject = xmlXPathEvalExpression(BAD_CAST(xpath.c_str()), xpathContext);
  CPPUNIT_NS::assertEquals<int>(1, xmlXPathNodeSetGetLength(xpathObject->nodesetval), rSourceLine, "Unexpected number of nodes on XPath.");

  xmlNodePtr node = xpathObject->nodesetval->nodeTab[0];
  xmlChar *prop = xmlGetProp(node, BAD_CAST(attribute.c_str()));
  std::string actual(reinterpret_cast<char const *>(prop), xmlStrlen(prop));
  CPPUNIT_NS::assertEquals<std::string>(expectedValue, actual, rSourceLine, "Unexpected XPath node attribute value.");

  xmlXPathFreeObject(xpathObject);
  xmlXPathFreeContext(xpathContext);
  xmlFreeDoc(doc);
}

void EPUBTextGeneratorTest::assertCss(const std::map< std::string, std::vector<std::string> > &cssDoc, const std::string &key, const std::string &value, bool positive, const CppUnit::SourceLine &rSourceLine)
{
  auto it = cssDoc.find(key);
  CPPUNIT_NS::Asserter::failIf(it == cssDoc.end(), CPPUNIT_NS::Message(std::string("Class ") + key + " not found"), rSourceLine);

  const std::vector<std::string> &rule = it->second;

  bool bFound = std::find(rule.begin(), rule.end(), value) != rule.end();
  std::string msg = std::string("In '") + key + "', rule '" + value + "' is " + (bFound ? "" : "not ") + "found.";

  CPPUNIT_NS::Asserter::failIf(positive != bFound, msg, rSourceLine);
}

void EPUBTextGeneratorTest::testChapterNames()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());
  {
    librevenge::RVNGPropertyList propertyList;
    propertyList.insert("text:outline-level", "1");
    generator.openParagraph(propertyList);
    generator.insertText("Chapter 1");
    generator.closeParagraph();
  }
  {
    librevenge::RVNGPropertyList propertyList;
    propertyList.insert("text:outline-level", "2");
    generator.openParagraph(propertyList);
    generator.insertText("Chapter 1.1");
    generator.closeParagraph();
  }
  {
    librevenge::RVNGPropertyList propertyList;
    propertyList.insert("text:outline-level", "1");
    generator.openParagraph(propertyList);
    generator.insertText("Chapter 2");
    generator.closeParagraph();
  }
  {
    librevenge::RVNGPropertyList propertyList;
    propertyList.insert("text:outline-level", "2");
    generator.openParagraph(propertyList);
    generator.insertText("Chapter 2.1");
    generator.closeParagraph();
  }
  generator.endDocument();

  // Make sure that the content was split to two sections.
  CPPUNIT_ASSERT(package.m_streams.find("OEBPS/sections/section0001.xhtml") != package.m_streams.end());
  CPPUNIT_ASSERT(package.m_streams.find("OEBPS/sections/section0002.xhtml") != package.m_streams.end());
  CPPUNIT_ASSERT(package.m_streams.find("OEBPS/sections/section0003.xhtml") == package.m_streams.end());

  // This was "Chapter 1Chapter 1.1", i.e. heading 2 text was also included in the chapter name.
  CPPUNIT_ASSERT_XPATH_CONTENT(package.m_streams["OEBPS/toc.xhtml"], "//xhtml:li[1]/xhtml:a", "Chapter 1");
  CPPUNIT_ASSERT_XPATH_CONTENT(package.m_streams["OEBPS/toc.xhtml"], "//xhtml:li[2]/xhtml:a", "Chapter 2");
}

void EPUBTextGeneratorTest::testChapterNames2()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());
  {
    librevenge::RVNGPropertyList propertyList;
    propertyList.insert("text:outline-level", "2");
    generator.openParagraph(propertyList);
    generator.insertText("Copyright");
    generator.closeParagraph();
  }
  {
    librevenge::RVNGPropertyList propertyList;
    propertyList.insert("text:outline-level", "1");
    generator.openParagraph(propertyList);
    generator.insertText("Chapter 1");
    generator.closeParagraph();
  }
  {
    librevenge::RVNGPropertyList propertyList;
    propertyList.insert("text:outline-level", "2");
    generator.openParagraph(propertyList);
    generator.insertText("Chapter 1.1");
    generator.closeParagraph();
  }
  generator.endDocument();

  // Make sure that the content was split to two sections.
  CPPUNIT_ASSERT(package.m_streams.find("OEBPS/sections/section0002.xhtml") != package.m_streams.end());
  CPPUNIT_ASSERT(package.m_streams.find("OEBPS/sections/section0003.xhtml") == package.m_streams.end());

  // This was 'Section 1', not 'Copyright'.
  CPPUNIT_ASSERT_XPATH_CONTENT(package.m_streams["OEBPS/toc.xhtml"], "//xhtml:li[1]/xhtml:a", "Copyright");
  CPPUNIT_ASSERT_XPATH_CONTENT(package.m_streams["OEBPS/toc.xhtml"], "//xhtml:li[2]/xhtml:a", "Chapter 1");
}

void EPUBTextGeneratorTest::testImage()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.openSpan(librevenge::RVNGPropertyList());
  generator.openFrame(librevenge::RVNGPropertyList());
  librevenge::RVNGPropertyList propertyList;
  propertyList.insert("librevenge:mime-type", "image/png");
  propertyList.insert("office:binary-data", librevenge::RVNGBinaryData());
  generator.insertBinaryObject(propertyList);
  generator.closeFrame();
  generator.closeSpan();
  generator.closeParagraph();
  generator.endDocument();

  // This was 0, as-char image resulted in p/span/p/img, i.e. an invalid <p> inside <span>.
  CPPUNIT_ASSERT_XPATH(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:p/xhtml:span/xhtml:img", 1);
}

void EPUBTextGeneratorTest::testImagePopup()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.openSpan(librevenge::RVNGPropertyList());

  librevenge::RVNGPropertyList link;
  // This will be an image inside a popup.
  link.insert("office:binary-data", librevenge::RVNGBinaryData());
  link.insert("librevenge:mime-type", "image/png");

  generator.openLink(link);
  generator.openFrame(librevenge::RVNGPropertyList());
  librevenge::RVNGPropertyList propertyList;
  // The popup will open when clicking on this image.
  propertyList.insert("librevenge:mime-type", "image/png");
  propertyList.insert("office:binary-data", librevenge::RVNGBinaryData());
  generator.insertBinaryObject(propertyList);
  generator.closeFrame();
  generator.closeLink();
  generator.closeSpan();
  generator.closeParagraph();
  generator.endDocument();

  // Link without URL around image was missing.
  CPPUNIT_ASSERT_XPATH(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:p/xhtml:span/xhtml:a/xhtml:img", 1);
  // Popup content was missing.
  CPPUNIT_ASSERT_XPATH(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:aside/xhtml:img", 1);
}

void EPUBTextGeneratorTest::testImageBorder()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.openSpan(librevenge::RVNGPropertyList());
  librevenge::RVNGPropertyList frame;
  frame.insert("fo:border", "0.99pt dashed #ed1c24");
  generator.openFrame(frame);
  librevenge::RVNGPropertyList propertyList;
  propertyList.insert("librevenge:mime-type", "image/png");
  propertyList.insert("office:binary-data", librevenge::RVNGBinaryData());
  generator.insertBinaryObject(propertyList);
  generator.closeFrame();
  generator.closeSpan();
  generator.closeParagraph();
  generator.endDocument();

  // This failed, <img> had no class attribute.
  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:p/xhtml:span/xhtml:img", "class", "frame0");
  CPPUNIT_ASSERT_CSS(package.m_cssStreams["OEBPS/styles/stylesheet.css"], ".frame0", "border: 0.99pt dashed #ed1c24", true);
}

void EPUBTextGeneratorTest::testImageHorizontalPosLeft()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.openSpan(librevenge::RVNGPropertyList());
  librevenge::RVNGPropertyList frame;
  frame.insert("style:wrap", "dynamic");
  frame.insert("style:vertical-pos", "top");
  frame.insert("style:vertical-rel", "paragraph");
  frame.insert("style:horizontal-pos", "left");
  frame.insert("style:horizontal-rel", "paragraph");
  frame.insert("svg:x", "0cm");
  frame.insert("svg:y", "0cm");
  frame.insert("text:anchor-type", "char");
  generator.openFrame(frame);
  librevenge::RVNGPropertyList propertyList;
  propertyList.insert("librevenge:mime-type", "image/png");
  propertyList.insert("office:binary-data", librevenge::RVNGBinaryData());
  generator.insertBinaryObject(propertyList);
  generator.closeFrame();
  generator.closeSpan();
  generator.closeParagraph();
  generator.endDocument();

  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:p/xhtml:span/xhtml:img", "class", "frame0");
  // This failed, float: was missing.
  CPPUNIT_ASSERT_CSS(package.m_cssStreams["OEBPS/styles/stylesheet.css"], ".frame0", "float: left", true);
}

void EPUBTextGeneratorTest::testImageHorizontalPosRight()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.openSpan(librevenge::RVNGPropertyList());
  librevenge::RVNGPropertyList frame;
  frame.insert("style:wrap", "dynamic");
  frame.insert("style:vertical-pos", "top");
  frame.insert("style:vertical-rel", "paragraph");
  frame.insert("style:horizontal-pos", "right");
  frame.insert("style:horizontal-rel", "paragraph");
  frame.insert("svg:x", "0cm");
  frame.insert("svg:y", "0cm");
  frame.insert("text:anchor-type", "char");
  generator.openFrame(frame);
  librevenge::RVNGPropertyList propertyList;
  propertyList.insert("librevenge:mime-type", "image/png");
  propertyList.insert("office:binary-data", librevenge::RVNGBinaryData());
  generator.insertBinaryObject(propertyList);
  generator.closeFrame();
  generator.closeSpan();
  generator.closeParagraph();
  generator.endDocument();

  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:p/xhtml:span/xhtml:img", "class", "frame0");
  // This failed, float: was missing.
  CPPUNIT_ASSERT_CSS(package.m_cssStreams["OEBPS/styles/stylesheet.css"], ".frame0", "float: right", true);
}

void EPUBTextGeneratorTest::testImageWidth()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.openSpan(librevenge::RVNGPropertyList());
  librevenge::RVNGPropertyList frame;
  frame.insert("svg:width", librevenge::RVNGPropertyFactory::newStringProp("3.81cm"));
  generator.openFrame(frame);
  librevenge::RVNGPropertyList propertyList;
  propertyList.insert("librevenge:mime-type", "image/png");
  propertyList.insert("office:binary-data", librevenge::RVNGBinaryData());
  generator.insertBinaryObject(propertyList);
  generator.closeFrame();
  generator.closeSpan();
  generator.closeParagraph();
  generator.endDocument();

  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:p/xhtml:span/xhtml:img", "class", "frame0");
  // This failed, width: was missing.
  CPPUNIT_ASSERT_CSS(package.m_cssStreams["OEBPS/styles/stylesheet.css"], ".frame0", "width: 3.81cm", true);
}

void EPUBTextGeneratorTest::testImageWidthRelative()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.openSpan(librevenge::RVNGPropertyList());
  librevenge::RVNGPropertyList frame;
  frame.insert("style:rel-width", "42%");
  generator.openFrame(frame);
  librevenge::RVNGPropertyList propertyList;
  propertyList.insert("librevenge:mime-type", "image/png");
  propertyList.insert("office:binary-data", librevenge::RVNGBinaryData());
  generator.insertBinaryObject(propertyList);
  generator.closeFrame();
  generator.closeSpan();
  generator.closeParagraph();
  generator.endDocument();

  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:p/xhtml:span/xhtml:img", "class", "frame0");
  // This failed, width: was missing.
  CPPUNIT_ASSERT_CSS(package.m_cssStreams["OEBPS/styles/stylesheet.css"], ".frame0", "width: 42.0000%", true);
}

void EPUBTextGeneratorTest::testImageWrapTypes()
{
  std::vector<std::string> wrapTypes = {"none", "left", "right", "parallel"};
  std::vector<std::string> clearTypes = {"both", "left", "right", "none"};

  for (size_t i = 0; i < wrapTypes.size(); ++i)
  {
    StringEPUBPackage package;
    libepubgen::EPUBTextGenerator generator(&package);
    generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
    generator.startDocument(librevenge::RVNGPropertyList());
    generator.openParagraph(librevenge::RVNGPropertyList());
    generator.openSpan(librevenge::RVNGPropertyList());
    librevenge::RVNGPropertyList frame;
    frame.insert("style:wrap", wrapTypes[i].c_str());
    frame.insert("style:vertical-pos", "top");
    frame.insert("style:vertical-rel", "paragraph");
    frame.insert("style:horizontal-pos", "left");
    frame.insert("style:horizontal-rel", "paragraph");
    frame.insert("svg:x", "0cm");
    frame.insert("svg:y", "0cm");
    frame.insert("text:anchor-type", "char");
    generator.openFrame(frame);
    librevenge::RVNGPropertyList propertyList;
    propertyList.insert("librevenge:mime-type", "image/png");
    propertyList.insert("office:binary-data", librevenge::RVNGBinaryData());
    generator.insertBinaryObject(propertyList);
    generator.closeFrame();
    generator.closeSpan();
    generator.closeParagraph();
    generator.endDocument();

    // Wrap type was ignored, <br> was missing.
    CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:br", "style", std::string("clear: ") + clearTypes[i] + ";");
  }
}

void EPUBTextGeneratorTest::testInvalidImage()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.openSpan(librevenge::RVNGPropertyList());
  generator.openFrame(librevenge::RVNGPropertyList());
  librevenge::RVNGPropertyList propertyList;
  propertyList.insert("librevenge:mime-type", "image/x-vclgraphic");
  propertyList.insert("office:binary-data", librevenge::RVNGBinaryData());
  generator.insertBinaryObject(propertyList);
  generator.closeFrame();
  generator.closeSpan();
  generator.closeParagraph();
  generator.endDocument();

  // This was 1, image/x-vclgraphic mime type was not rejected.
  CPPUNIT_ASSERT_XPATH(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:p/xhtml:span/xhtml:img", 0);
}

void EPUBTextGeneratorTest::testBrokenImage()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.openSpan(librevenge::RVNGPropertyList());
  generator.openFrame(librevenge::RVNGPropertyList());
  librevenge::RVNGPropertyList propertyList;
  // Set mime type to empty, which is invalid.
  propertyList.insert("librevenge:mime-type", "");
  propertyList.insert("office:binary-data", librevenge::RVNGBinaryData());
  generator.insertBinaryObject(propertyList);
  generator.closeFrame();
  generator.closeSpan();
  generator.closeParagraph();
  generator.endDocument();

  // Image was not dropped in this case, this was 1 -> invalid EPUB was written.
  CPPUNIT_ASSERT_XPATH(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:p/xhtml:span/xhtml:img", 0);
}

void EPUBTextGeneratorTest::testBrokenLink()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());
  generator.openParagraph(librevenge::RVNGPropertyList());
  librevenge::RVNGPropertyList propertyList;
  propertyList.insert("xlink:href", "https:///www.fsf.org");
  generator.openLink(propertyList);
  generator.insertText("x");
  generator.closeLink();
  generator.closeParagraph();
  generator.endDocument();

  // This was https:///www.fsf.org, i.e. the URL had an additional unexpected '/'.
  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:p/xhtml:a", "href", "https://www.fsf.org");
}

void EPUBTextGeneratorTest::testSplitMethodPageBreak()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_PAGE_BREAK);
  generator.startDocument(librevenge::RVNGPropertyList());
  generator.insertText("hello");
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.closeParagraph();
  librevenge::RVNGPropertyList propertyList;
  propertyList.insert("fo:break-before", "page");
  generator.openParagraph(propertyList);
  generator.closeParagraph();
  generator.endDocument();

  // Assert that the output is split into 2 parts.
  CPPUNIT_ASSERT(package.m_streams.find("OEBPS/sections/section0001.xhtml") != package.m_streams.end());
  // This failed: output wasn't split at page break.
  CPPUNIT_ASSERT(package.m_streams.find("OEBPS/sections/section0002.xhtml") != package.m_streams.end());
  CPPUNIT_ASSERT(package.m_streams.find("OEBPS/sections/section0003.xhtml") == package.m_streams.end());
}

void EPUBTextGeneratorTest::testMetadata()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());

  librevenge::RVNGPropertyList propertyList;
  propertyList.insert("meta:initial-creator", "A U Thor");
  propertyList.insert("dc:title", "Title");
  propertyList.insert("dc:identifier", "deadbeef-e394-4cd6-9b83-7172794612e5");
  generator.setDocumentMetaData(propertyList);

  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.insertText("x");
  generator.closeParagraph();
  generator.endDocument();

  // Generator was not exported, this was 0.
  CPPUNIT_ASSERT_XPATH(package.m_streams["OEBPS/content.opf"], "/opf:package/opf:metadata/opf:meta[@name='generator']", 1);
  // Author was not exported, this XML element was missing.
  CPPUNIT_ASSERT_XPATH_CONTENT(package.m_streams["OEBPS/content.opf"], "/opf:package/opf:metadata/dc:creator", "A U Thor");
  CPPUNIT_ASSERT_XPATH_CONTENT(package.m_streams["OEBPS/content.opf"], "/opf:package/opf:metadata/dc:title", "Title");
  // It was not possible to provide a value instead of the random dc:identifier.
  CPPUNIT_ASSERT_XPATH_CONTENT(package.m_streams["OEBPS/content.opf"], "/opf:package/opf:metadata/dc:identifier", "deadbeef-e394-4cd6-9b83-7172794612e5");

  // Make sure we have no duplication of metadata in the XHTML content document (from content.opf).
  CPPUNIT_ASSERT_XPATH(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:head/xhtml:title", 0);
}

void EPUBTextGeneratorTest::testSpace()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.openSpan(librevenge::RVNGPropertyList());
  generator.insertSpace();
  generator.closeSpan();
  generator.closeParagraph();
  generator.endDocument();

  // This was &nbsp;, i.e. it was double-escaped.
  CPPUNIT_ASSERT_XPATH_CONTENT(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:p/xhtml:span", "\xc2\xa0");
}

void EPUBTextGeneratorTest::testLineThough()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());
  generator.openParagraph(librevenge::RVNGPropertyList());
  librevenge::RVNGPropertyList propertyList;
  propertyList.insert("style:text-line-through-style", "none");
  propertyList.insert("style:text-line-through-type", "none");
  generator.openSpan(propertyList);
  generator.insertSpace();
  generator.closeSpan();
  generator.closeParagraph();
  generator.endDocument();

  // This failed, style:text-line-through-style=none resulted in a line-through rule.
  CPPUNIT_ASSERT_CSS(package.m_cssStreams["OEBPS/styles/stylesheet.css"], ".span0", "text-decoration:  line-through", false);
}

void EPUBTextGeneratorTest::testUnderline()
{
  // Test that we don't export unexpected underline formatting.
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());
  generator.openParagraph(librevenge::RVNGPropertyList());
  librevenge::RVNGPropertyList propertyList;
  propertyList.insert("style:text-underline-style", "none");
  propertyList.insert("style:text-underline-type", "none");
  generator.openSpan(propertyList);
  generator.insertSpace();
  generator.closeSpan();
  generator.closeParagraph();
  generator.endDocument();

  // This failed, style:text-underline-style=none resulted in an underline rule.
  CPPUNIT_ASSERT_CSS(package.m_cssStreams["OEBPS/styles/stylesheet.css"], ".span0", "text-decoration:  underline", false);
}

void EPUBTextGeneratorTest::testEmptyPara()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.closeParagraph();
  generator.endDocument();

  // This failed, empty paragraph was <p></p> in the HTML output, so vertical space collapsed.
  CPPUNIT_ASSERT_XPATH_CONTENT(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:p", "\xc2\xa0");
}

void EPUBTextGeneratorTest::testTab()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.insertTab();
  generator.closeParagraph();
  generator.endDocument();

  // This failed, we wrote \t instead, which is ignorable whitespace in HTML.
  CPPUNIT_ASSERT_XPATH_CONTENT(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:p", "\xc2\xa0\xc2\xa0\xc2\xa0\xc2\xa0\xc2\xa0\xc2\xa0\xc2\xa0\xc2\xa0\xc2\xa0\xc2\xa0\xc2\xa0\xc2\xa0\xc2\xa0\xc2\xa0\xc2\xa0 ");
}

void EPUBTextGeneratorTest::testStylesMethodInline()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_STYLES, libepubgen::EPUB_STYLES_METHOD_INLINE);

  generator.startDocument(librevenge::RVNGPropertyList());
  librevenge::RVNGPropertyList paraProps;
  paraProps.insert("fo:text-align", "center");
  generator.openParagraph(paraProps);
  librevenge::RVNGPropertyList charProps;
  charProps.insert("fo:font-weight", "bold");
  generator.openSpan(charProps);
  generator.insertSpace();
  generator.closeSpan();
  generator.closeParagraph();
  generator.endDocument();

  // Make sure that the style is inline, so it's not in the CSS file.
  auto it = package.m_cssStreams["OEBPS/styles/stylesheet.css"].find(".para0");
  // This failed, the named style was there.
  CPPUNIT_ASSERT(it == package.m_cssStreams["OEBPS/styles/stylesheet.css"].end());
}

void EPUBTextGeneratorTest::testStylesMethodInlineRowCell()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_STYLES, libepubgen::EPUB_STYLES_METHOD_INLINE);

  generator.startDocument(librevenge::RVNGPropertyList());
  librevenge::RVNGPropertyList props;
  generator.openTable(props);
  generator.openTableRow(props);
  generator.openTableCell(props);
  generator.openParagraph(props);
  generator.openSpan(props);
  generator.closeSpan();
  generator.closeParagraph();
  generator.closeTableCell();
  generator.closeTableRow();
  generator.closeTable();
  generator.endDocument();

  // Make sure that the row style is inline, so it's not in the CSS file.
  auto it = package.m_cssStreams["OEBPS/styles/stylesheet.css"].find(".rowTable0");
  // This failed, the named row style was there.
  CPPUNIT_ASSERT(it == package.m_cssStreams["OEBPS/styles/stylesheet.css"].end());

  // Make sure that the cell style is inline, so it's not in the CSS file.
  it = package.m_cssStreams["OEBPS/styles/stylesheet.css"].find(".cellTable0");
  // This failed, the named cell style was there.
  CPPUNIT_ASSERT(it == package.m_cssStreams["OEBPS/styles/stylesheet.css"].end());
}

void EPUBTextGeneratorTest::testStylesMethodCSS()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_STYLES, libepubgen::EPUB_STYLES_METHOD_CSS);

  generator.startDocument(librevenge::RVNGPropertyList());
  librevenge::RVNGPropertyList paraProps;
  paraProps.insert("fo:text-align", "center");
  generator.openParagraph(paraProps);
  librevenge::RVNGPropertyList charProps;
  charProps.insert("fo:font-weight", "bold");
  generator.openSpan(charProps);
  generator.insertSpace();
  generator.closeSpan();
  generator.closeParagraph();
  generator.endDocument();

  // The attribute was missing, external stylesheet wasn't loaded in a number of readers.
  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:link", "rel", "stylesheet");
}

void EPUBTextGeneratorTest::testRelColumnWidth()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);

  librevenge::RVNGPropertyList props;
  generator.startDocument(props);
  librevenge::RVNGPropertyList column;
  column.insert("style:rel-column-width", "42*");
  librevenge::RVNGPropertyList column2;
  column2.insert("style:rel-column-width", "42*");
  librevenge::RVNGPropertyListVector columns;
  columns.append(column);
  columns.append(column2);
  librevenge::RVNGPropertyList table;
  table.insert("librevenge:table-columns", columns);
  generator.openTable(table);
  generator.openTableRow(props);
  librevenge::RVNGPropertyList cell;
  cell.insert("librevenge:column", 0);
  generator.openTableCell(cell);
  generator.openParagraph(props);
  generator.openSpan(props);
  generator.closeSpan();
  generator.closeParagraph();
  generator.closeTableCell();
  librevenge::RVNGPropertyList cell2;
  cell2.insert("librevenge:column", 1);
  generator.openTableCell(cell2);
  generator.openParagraph(props);
  generator.openSpan(props);
  generator.closeSpan();
  generator.closeParagraph();
  generator.closeTableCell();
  generator.closeTableRow();
  generator.closeTable();
  generator.endDocument();

  // style:rel-column-width was ignored, .cellTable0 had no 'width' key.
  CPPUNIT_ASSERT_CSS(package.m_cssStreams["OEBPS/styles/stylesheet.css"], ".cellTable0", "width: 50%", true);
}

void EPUBTextGeneratorTest::testRelTableWidth()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);

  librevenge::RVNGPropertyList props;
  generator.startDocument(props);
  librevenge::RVNGPropertyList table;
  table.insert("style:rel-width", librevenge::RVNGPropertyFactory::newStringProp("50%"));
  generator.openTable(table);
  generator.openTableRow(props);
  librevenge::RVNGPropertyList cell;
  cell.insert("style:vertical-align", "");
  generator.openTableCell(cell);
  generator.openParagraph(props);
  generator.openSpan(props);
  generator.closeSpan();
  generator.closeParagraph();
  generator.closeTableCell();
  generator.closeTableRow();
  generator.closeTable();
  generator.endDocument();

  // style:rel-width was ignored, CSS file was empty.
  CPPUNIT_ASSERT_CSS(package.m_cssStreams["OEBPS/styles/stylesheet.css"], ".table0", "width: 50%", true);
  // vertical-align key with no value was written.
  CPPUNIT_ASSERT_CSS(package.m_cssStreams["OEBPS/styles/stylesheet.css"], ".cellTable0", "vertical-align: ", false);
}

void EPUBTextGeneratorTest::testSection()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());
  {
    librevenge::RVNGPropertyList propertyList;
    propertyList.insert("text:outline-level", "1");
    generator.openParagraph(propertyList);
    generator.insertText("Chapter 1");
    generator.closeParagraph();
  }
  {
    librevenge::RVNGPropertyList propertyList;
    propertyList.insert("text:outline-level", "1");
    generator.openSection(propertyList);
    generator.openParagraph(propertyList);
    generator.insertText("Chapter 2");
    generator.closeParagraph();
    generator.closeSection();
  }
  {
    librevenge::RVNGPropertyList propertyList;
    propertyList.insert("text:outline-level", "1");
    generator.openParagraph(propertyList);
    generator.insertText("Chapter 3");
    generator.closeParagraph();
  }
  generator.endDocument();

  CPPUNIT_ASSERT(package.m_streams.find("OEBPS/sections/section0001.xhtml") != package.m_streams.end());
  CPPUNIT_ASSERT(package.m_streams.find("OEBPS/sections/section0002.xhtml") != package.m_streams.end());
  // This failed: 3 chapters did not result in 3 separate files.
  CPPUNIT_ASSERT(package.m_streams.find("OEBPS/sections/section0003.xhtml") != package.m_streams.end());
}

void EPUBTextGeneratorTest::testTextBox()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.openSpan(librevenge::RVNGPropertyList());
  librevenge::RVNGPropertyList frame;
  frame.insert("style:wrap", "none");
  frame.insert("style:vertical-pos", "top");
  frame.insert("style:vertical-rel", "paragraph");
  frame.insert("style:horizontal-pos", "left");
  frame.insert("style:horizontal-rel", "paragraph");
  frame.insert("svg:x", "0cm");
  frame.insert("svg:y", "0cm");
  frame.insert("text:anchor-type", "char");
  generator.openFrame(frame);
  generator.openTextBox(librevenge::RVNGPropertyList());
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.openSpan(librevenge::RVNGPropertyList());
  generator.insertText("in-textbox");
  generator.closeSpan();
  generator.openSpan(librevenge::RVNGPropertyList());
  librevenge::RVNGPropertyList image;
  image.insert("style:wrap", "none");
  image.insert("text:anchor-type", "as-char");
  generator.openFrame(image);
  librevenge::RVNGPropertyList propertyList;
  propertyList.insert("librevenge:mime-type", "image/png");
  propertyList.insert("office:binary-data", librevenge::RVNGBinaryData());
  generator.insertBinaryObject(propertyList);
  generator.closeFrame();
  generator.closeSpan();
  generator.closeParagraph();
  generator.closeTextBox();
  generator.closeFrame();
  generator.closeSpan();
  generator.closeParagraph();
  generator.endDocument();

  // This failed: textbox content was lost.
  CPPUNIT_ASSERT_XPATH_CONTENT(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:div[1]/xhtml:p/xhtml:span[1]", "in-textbox");
  // This failed: there was an unexpected <br> after the <img>.
  CPPUNIT_ASSERT_XPATH(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:div[1]/xhtml:p/xhtml:span[2]/xhtml:br", 0);
  // This failed: wrap type of textbox was lost.
  // Also make sure that the <br> is outside the <p> and <span>, since the
  // <div> before the <br> can't be inside a <p>.
  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:body/xhtml:br", "style", "clear: both;");
}

void EPUBTextGeneratorTest::testEmbeddedFont()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());
  librevenge::RVNGPropertyList embeddedFont;
  embeddedFont.insert("librevenge:name", "Embedded Font");
  embeddedFont.insert("librevenge:mime-type", "truetype");
  embeddedFont.insert("office:binary-data", librevenge::RVNGBinaryData());
  embeddedFont.insert("librevenge:font-style", "normal");
  embeddedFont.insert("librevenge:font-weight", "normal");
  generator.defineEmbeddedFont(embeddedFont);
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.openSpan(librevenge::RVNGPropertyList());
  generator.insertText("hello");
  generator.closeSpan();
  generator.closeParagraph();
  generator.endDocument();

  // Make sure that the binary data gets written and has the correct extension.
  CPPUNIT_ASSERT(package.m_binaryStreams.find("OEBPS/fonts/font0001.otf") != package.m_binaryStreams.end());
  // The binary data gets referenced in the CSS.
  CPPUNIT_ASSERT_CSS(package.m_cssStreams["OEBPS/styles/stylesheet.css"], "@font-face", "src: url(../fonts/font0001.otf)", true);
  // The binary data gets referenced in the manifest and has correct media-type.
  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/content.opf"], "/opf:package/opf:manifest/opf:item[@href='fonts/font0001.otf']", "media-type", "application/vnd.ms-opentype");
}

void EPUBTextGeneratorTest::testCoverImage()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());

  librevenge::RVNGBinaryData binaryData;
  librevenge::RVNGPropertyList coverImage;
  coverImage.insert("office:binary-data", binaryData);
  coverImage.insert("librevenge:mime-type", "image/png");
  librevenge::RVNGPropertyListVector coverImages;
  coverImages.append(coverImage);
  librevenge::RVNGPropertyList metaData;
  metaData.insert("librevenge:cover-images", coverImages);
  generator.setDocumentMetaData(metaData);

  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.insertText("x");
  generator.closeParagraph();
  generator.endDocument();

  // Make sure the data for the cover image is there.
  CPPUNIT_ASSERT(package.m_binaryStreams.find("OEBPS/images/image0001.png") != package.m_binaryStreams.end());
  // With a correct mime-type.
  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/content.opf"], "/opf:package/opf:manifest/opf:item[@href='images/image0001.png']", "media-type", "image/png");
  // And it's marked as a cover image.
  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/content.opf"], "/opf:package/opf:manifest/opf:item[@href='images/image0001.png']", "properties", "cover-image");
}

void EPUBTextGeneratorTest::testFootnote()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package, /*version=*/20);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.insertText("before");
  generator.openFootnote(librevenge::RVNGPropertyList());
  generator.openParagraph(librevenge::RVNGPropertyList());

  // Start the footnote content with a link, this makes sure that the below
  // assert ensures the link to the footnote anchor and the link at the start
  // of the footnote are after each other, i.e. the links are not nested.
  librevenge::RVNGPropertyList link;
  link.insert("xlink:href", "https://www.fsf.org");
  generator.openLink(link);
  generator.insertText("footnote content");
  generator.closeLink();

  generator.closeParagraph();
  generator.closeFootnote();
  generator.insertText("after");
  generator.closeParagraph();
  generator.endDocument();

  // Main sink was saved too late -> footnote anchor was lost.
  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:body/xhtml:p[1]/xhtml:sup/xhtml:a", "href", "#dataF1");
  // Non-const ref was handed out, so elements were added without updating empty state -> footnote content was lost.
  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:body/xhtml:p[2]/xhtml:sup", "id", "dataF1");
}

void EPUBTextGeneratorTest::testFootnote3()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.insertText("before");
  generator.openFootnote(librevenge::RVNGPropertyList());
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.insertText("footnote content");
  generator.closeParagraph();
  generator.closeFootnote();
  generator.insertText("after");
  generator.closeParagraph();
  generator.endDocument();

  // This failed epub:type="noteref" was missing.
  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:body/xhtml:p[1]/xhtml:sup/xhtml:a", "type", "noteref");
  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:body/xhtml:p[1]/xhtml:sup/xhtml:a", "href", "#dataF1");
  // This failed, epub:type="footnote" was missing.
  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:body/xhtml:aside", "type", "footnote");
  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:body/xhtml:aside", "id", "dataF1");
}

void EPUBTextGeneratorTest::testFootnoteAnchor()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.insertText("before");
  librevenge::RVNGPropertyList footnote;
  footnote.insert("librevenge:number", "1");
  generator.openFootnote(footnote);
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.insertText("footnote content");
  generator.closeParagraph();
  generator.closeFootnote();
  generator.insertText("after");
  generator.closeParagraph();
  generator.endDocument();

  // These were F1, not 1, i.e. user-provided footnote anchor text was ignored.
  CPPUNIT_ASSERT_XPATH_CONTENT(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:body/xhtml:p[1]/xhtml:sup/xhtml:a", "1");
  CPPUNIT_ASSERT_XPATH_CONTENT(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:body/xhtml:aside/xhtml:p/xhtml:sup/xhtml:a", "1");
}

void EPUBTextGeneratorTest::testTextPopup()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.openSpan(librevenge::RVNGPropertyList());

  librevenge::RVNGPropertyList link;
  // This will be an image inside a popup.
  link.insert("office:binary-data", librevenge::RVNGBinaryData());
  link.insert("librevenge:mime-type", "image/png");

  generator.openLink(link);
  // The popup will open when clicking on this.
  generator.insertText("link");
  generator.closeLink();
  generator.closeSpan();
  generator.closeParagraph();
  generator.endDocument();

  // <a> around "link" was missing.
  CPPUNIT_ASSERT_XPATH_CONTENT(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:p/xhtml:span/xhtml:a", "link");
  // <aside> was also missing.
  CPPUNIT_ASSERT_XPATH(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:aside/xhtml:img", 1);
}

void EPUBTextGeneratorTest::testFixedLayout()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_LAYOUT, libepubgen::EPUB_LAYOUT_METHOD_FIXED);
  generator.startDocument(librevenge::RVNGPropertyList());

  librevenge::RVNGPropertyList page;
  page.insert("fo:page-width", "1in");
  page.insert("fo:page-height", "2in");

  generator.openPageSpan(page);
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.openSpan(librevenge::RVNGPropertyList());
  generator.insertText("hello");
  generator.closeSpan();
  generator.closeParagraph();
  generator.closePageSpan();
  generator.endDocument();

  // These failed, the bare minimum of a fixed layout was missing.
  CPPUNIT_ASSERT_XPATH_CONTENT(package.m_streams["OEBPS/content.opf"], "/opf:package/opf:metadata/opf:meta[@property='rendition:layout']", "pre-paginated");
  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0001.xhtml"], "/xhtml:html/xhtml:head/xhtml:meta[@name='viewport']", "content", "width=96, height=192");
}

void EPUBTextGeneratorTest::testFixedLayoutBreakExplicit()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_LAYOUT, libepubgen::EPUB_LAYOUT_METHOD_FIXED);
  generator.startDocument(librevenge::RVNGPropertyList());
  librevenge::RVNGPropertyList page;
  page.insert("fo:page-width", "1in");
  page.insert("fo:page-height", "2in");
  generator.openPageSpan(page);
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.openSpan(librevenge::RVNGPropertyList());
  generator.insertText("hello");
  generator.closeSpan();
  generator.closeParagraph();
  librevenge::RVNGPropertyList paragraph;
  paragraph.insert("fo:break-before", "page");
  generator.openParagraph(paragraph);
  generator.closeParagraph();
  generator.closePageSpan();
  generator.endDocument();

  // Make sure a split happens on page break due to fixed layout (even with split on headings).
  CPPUNIT_ASSERT(package.m_streams.find("OEBPS/sections/section0001.xhtml") != package.m_streams.end());
  CPPUNIT_ASSERT(package.m_streams.find("OEBPS/sections/section0002.xhtml") != package.m_streams.end());
  // Make sure the second HTML file has the correct viewport as well.
  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0002.xhtml"], "/xhtml:html/xhtml:head/xhtml:meta[@name='viewport']", "content", "width=96, height=192");
}

void EPUBTextGeneratorTest::testFixedLayoutSpine()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_LAYOUT, libepubgen::EPUB_LAYOUT_METHOD_FIXED);
  generator.startDocument(librevenge::RVNGPropertyList());

  librevenge::RVNGPropertyList page;
  page.insert("fo:page-width", "1in");
  page.insert("fo:page-height", "2in");

  generator.openPageSpan(page);
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.openSpan(librevenge::RVNGPropertyList());
  generator.insertText("hello");
  generator.closeSpan();
  generator.closeParagraph();
  generator.closePageSpan();
  generator.endDocument();

  // This was Section 1 instead.
  CPPUNIT_ASSERT_XPATH_CONTENT(package.m_streams["OEBPS/toc.xhtml"], "//xhtml:li[1]/xhtml:a", "Page 1");
}

void EPUBTextGeneratorTest::testFixedLayoutChapters()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_LAYOUT, libepubgen::EPUB_LAYOUT_METHOD_FIXED);
  generator.startDocument(librevenge::RVNGPropertyList());

  librevenge::RVNGPropertyList chapter1;
  chapter1.insert("librevenge:name", "Heading 1");
  librevenge::RVNGPropertyList chapter2;
  chapter2.insert("librevenge:name", "Heading 2");
  librevenge::RVNGPropertyListVector chapterNames;
  chapterNames.append(chapter1);
  chapterNames.append(chapter2);

  librevenge::RVNGPropertyList page;
  page.insert("fo:page-width", "1in");
  page.insert("fo:page-height", "2in");
  page.insert("librevenge:chapter-names", chapterNames);

  generator.openPageSpan(page);
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.openSpan(librevenge::RVNGPropertyList());
  generator.insertText("hello");
  generator.closeSpan();
  generator.closeParagraph();
  generator.closePageSpan();

  librevenge::RVNGPropertyList page2;
  page2.insert("fo:page-width", "1in");
  page2.insert("fo:page-height", "2in");
  generator.openPageSpan(page2);
  librevenge::RVNGPropertyList paragraph2;
  paragraph2.insert("fo:break-before", "page");
  generator.openParagraph(paragraph2);
  generator.openSpan(librevenge::RVNGPropertyList());
  generator.insertText("hello");
  generator.closeSpan();
  generator.closeParagraph();
  generator.closePageSpan();

  generator.endDocument();

  // This was 'Page 1', i.e. chapter name was ignored in the navigation document.
  CPPUNIT_ASSERT_XPATH_CONTENT(package.m_streams["OEBPS/toc.xhtml"], "//xhtml:li[1]/xhtml:a", "Heading 1");
  CPPUNIT_ASSERT_XPATH_CONTENT(package.m_streams["OEBPS/toc.xhtml"], "//xhtml:li[2]/xhtml:a", "Heading 2");
  // 'Page 2' was provided when the ToC already had chapter names.
  CPPUNIT_ASSERT_XPATH(package.m_streams["OEBPS/toc.xhtml"], "//xhtml:li[3]/xhtml:a", 0);
}

void EPUBTextGeneratorTest::testPageBreak()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_PAGE_BREAK);
  generator.startDocument(librevenge::RVNGPropertyList());
  generator.insertText("hello");
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.closeParagraph();
  librevenge::RVNGPropertyList propertyList;
  propertyList.insert("fo:break-before", "auto");
  generator.openParagraph(propertyList);
  generator.closeParagraph();
  generator.endDocument();

  // Make sure that fo:break-before=auto is not a page break, i.e. we only have a single page.
  CPPUNIT_ASSERT(package.m_streams.find("OEBPS/sections/section0001.xhtml") != package.m_streams.end());
  CPPUNIT_ASSERT(package.m_streams.find("OEBPS/sections/section0002.xhtml") == package.m_streams.end());
}

void EPUBTextGeneratorTest::testPageBreakImage()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_PAGE_BREAK);
  generator.startDocument(librevenge::RVNGPropertyList());
  librevenge::RVNGPropertyList image;
  image.insert("librevenge:mime-type", "image/png");
  image.insert("office:binary-data", librevenge::RVNGBinaryData());
  generator.insertBinaryObject(image);
  generator.openParagraph(librevenge::RVNGPropertyList());
  generator.closeParagraph();
  librevenge::RVNGPropertyList propertyList;
  propertyList.insert("fo:break-before", "page");
  generator.openParagraph(propertyList);
  generator.closeParagraph();
  generator.endDocument();

  // This failed, if the page only contained a single image, starting a new section didn't happen.
  CPPUNIT_ASSERT(package.m_streams.find("OEBPS/sections/section0001.xhtml") != package.m_streams.end());
  CPPUNIT_ASSERT(package.m_streams.find("OEBPS/sections/section0002.xhtml") != package.m_streams.end());
}

void EPUBTextGeneratorTest::testPageSpanProperties()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_PAGE_BREAK);
  generator.startDocument(librevenge::RVNGPropertyList());

  {
    librevenge::RVNGPropertyList page;
    page.insert("style:writing-mode", "tb");
    generator.openPageSpan(page);

    librevenge::RVNGPropertyList para;
    generator.openParagraph(para);
    generator.insertText("Para1");
    generator.closeParagraph();
  }
  generator.endDocument();
  CPPUNIT_ASSERT_CSS(package.m_cssStreams["OEBPS/styles/stylesheet.css"], ".body0", "writing-mode: vertical-rl", true);
  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:body", "class", "body0");
}

void EPUBTextGeneratorTest::testSplitOnPageBreakInPageSpan()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_PAGE_BREAK);
  generator.startDocument(librevenge::RVNGPropertyList());

  {
    librevenge::RVNGPropertyList page;
    page.insert("style:writing-mode", "tb");
    generator.openPageSpan(page);

    librevenge::RVNGPropertyList para;
    generator.openParagraph(para);
    generator.insertText("Para1");
    generator.closeParagraph();
    // Splitting a new html file inside the page span, the writing-mode shall not change.
    para.insert("fo:break-before", "page");
    generator.openParagraph(para);
    generator.insertText("Para2");
    generator.closeParagraph();
    generator.closePageSpan();
  }
  generator.endDocument();
  CPPUNIT_ASSERT_CSS(package.m_cssStreams["OEBPS/styles/stylesheet.css"], ".body0", "writing-mode: vertical-rl", true);
  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:body", "class", "body0");
  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0002.xhtml"], "//xhtml:body", "class", "body0");
}

void EPUBTextGeneratorTest::testSplitOnHeadingInPageSpan()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_HEADING);
  generator.startDocument(librevenge::RVNGPropertyList());

  {
    librevenge::RVNGPropertyList page;
    page.insert("style:writing-mode", "tb");
    generator.openPageSpan(page);

    librevenge::RVNGPropertyList para;
    para.insert("text:outline-level", "1");
    generator.openParagraph(para);
    generator.insertText("Chapter1");
    generator.closeParagraph();
    // Splitting a new html file inside the page span, the writing-mode shall not change.
    generator.openParagraph(para);
    generator.insertText("Chapter2");
    generator.closeParagraph();
    generator.closePageSpan();
  }
  generator.endDocument();
  CPPUNIT_ASSERT_CSS(package.m_cssStreams["OEBPS/styles/stylesheet.css"], ".body0", "writing-mode: vertical-rl", true);
  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:body", "class", "body0");
  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0002.xhtml"], "//xhtml:body", "class", "body0");
}

void EPUBTextGeneratorTest::testSplitOnSizeInPageSpan()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_SIZE);
  generator.setSplitSize(5);
  generator.startDocument(librevenge::RVNGPropertyList());

  {
    librevenge::RVNGPropertyList page;
    page.insert("style:writing-mode", "tb");
    generator.openPageSpan(page);

    librevenge::RVNGPropertyList para;
    generator.openParagraph(para);
    generator.insertText("Hello!");
    generator.closeParagraph();
    // Splitting a new html file inside the page span, the writing-mode shall not change.
    generator.openParagraph(para);
    generator.insertText("Hello!");
    generator.closeParagraph();
    generator.closePageSpan();
  }
  generator.endDocument();
  CPPUNIT_ASSERT_CSS(package.m_cssStreams["OEBPS/styles/stylesheet.css"], ".body0", "writing-mode: vertical-rl", true);
  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:body", "class", "body0");
  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0002.xhtml"], "//xhtml:body", "class", "body0");
}

void EPUBTextGeneratorTest::testManyWritingModes()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.setOption(libepubgen::EPUB_GENERATOR_OPTION_SPLIT, libepubgen::EPUB_SPLIT_METHOD_PAGE_BREAK);
  generator.startDocument(librevenge::RVNGPropertyList());

  {
    librevenge::RVNGPropertyList page;
    page.insert("style:writing-mode", "tb");
    generator.openPageSpan(page);

    librevenge::RVNGPropertyList para;
    generator.openParagraph(para);
    generator.insertText("Para1");
    generator.closeParagraph();
    generator.closePageSpan();
  }
  {
    librevenge::RVNGPropertyList page;
    page.insert("style:writing-mode", "lr");
    generator.openPageSpan(page);

    librevenge::RVNGPropertyList para;
    generator.openParagraph(para);
    generator.insertText("Para1");
    generator.closeParagraph();
    generator.closePageSpan();
  }
  generator.endDocument();
  CPPUNIT_ASSERT_CSS(package.m_cssStreams["OEBPS/styles/stylesheet.css"], ".body0", "writing-mode: vertical-rl", true);
  CPPUNIT_ASSERT_CSS(package.m_cssStreams["OEBPS/styles/stylesheet.css"], ".body1", "writing-mode: horizontal-tb", true);
  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:body", "class", "body0");
  CPPUNIT_ASSERT_XPATH_ATTRIBUTE(package.m_streams["OEBPS/sections/section0002.xhtml"], "//xhtml:body", "class", "body1");
}

void EPUBTextGeneratorTest::testRubyElements()
{
  StringEPUBPackage package;
  libepubgen::EPUBTextGenerator generator(&package);
  generator.startDocument(librevenge::RVNGPropertyList());
  generator.openParagraph(librevenge::RVNGPropertyList());
  {
    librevenge::RVNGPropertyList span;
    span.insert("text:ruby-text", "ruby text");
    generator.openSpan(span);
    generator.insertText("base text");
    generator.closeSpan();
  }
  generator.closeParagraph();
  generator.endDocument();

  // Expects: <ruby><span>base text</span><rt>ruby text</rt></ruby>
  CPPUNIT_ASSERT_XPATH(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:ruby", 1);
  CPPUNIT_ASSERT_XPATH_CONTENT(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:ruby/xhtml:rt", "ruby text");
  CPPUNIT_ASSERT_XPATH_CONTENT(package.m_streams["OEBPS/sections/section0001.xhtml"], "//xhtml:ruby/xhtml:span", "base text");
}


CPPUNIT_TEST_SUITE_REGISTRATION(EPUBTextGeneratorTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
