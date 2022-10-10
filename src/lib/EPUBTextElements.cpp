/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "EPUBTextElements.h"

#include "libepubgen_utils.h"

namespace libepubgen
{

class EPUBTextElement
{
public:
  EPUBTextElement() {}
  virtual ~EPUBTextElement() {}
  virtual void write(librevenge::RVNGTextInterface *iface) const = 0;
};

namespace
{

class CloseCommentElement : public EPUBTextElement
{
public:
  CloseCommentElement() {}
  ~CloseCommentElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
};

class CloseEndnoteElement : public EPUBTextElement
{
public:
  CloseEndnoteElement() {}
  ~CloseEndnoteElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
};

class CloseFooterElement : public EPUBTextElement
{
public:
  CloseFooterElement() {}
  ~CloseFooterElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
};

class CloseFootnoteElement : public EPUBTextElement
{
public:
  CloseFootnoteElement() {}
  ~CloseFootnoteElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
};

class CloseFrameElement : public EPUBTextElement
{
public:
  CloseFrameElement() {}
  ~CloseFrameElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
};

class CloseHeaderElement : public EPUBTextElement
{
public:
  CloseHeaderElement() {}
  ~CloseHeaderElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
};

class CloseLinkElement : public EPUBTextElement
{
public:
  CloseLinkElement() {}
  ~CloseLinkElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
  EPUBTextElement *clone()
  {
    return new CloseLinkElement();
  }
};

class CloseListElementElement : public EPUBTextElement
{
public:
  CloseListElementElement() {}
  ~CloseListElementElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
  EPUBTextElement *clone()
  {
    return new CloseListElementElement();
  }
};

class CloseOrderedListLevelElement : public EPUBTextElement
{
public:
  CloseOrderedListLevelElement() {}
  ~CloseOrderedListLevelElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
  EPUBTextElement *clone()
  {
    return new CloseOrderedListLevelElement();
  }
};

class CloseParagraphElement : public EPUBTextElement
{
public:
  CloseParagraphElement() {}
  ~CloseParagraphElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
  EPUBTextElement *clone()
  {
    return new CloseParagraphElement();
  }
};

class CloseSpanElement : public EPUBTextElement
{
public:
  CloseSpanElement() {}
  ~CloseSpanElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
  EPUBTextElement *clone()
  {
    return new CloseSpanElement();
  }
};

class CloseTableElement : public EPUBTextElement
{
public:
  CloseTableElement() {}
  ~CloseTableElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
  EPUBTextElement *clone()
  {
    return new CloseTableElement();
  }
};

class CloseTableCellElement : public EPUBTextElement
{
public:
  CloseTableCellElement() {}
  ~CloseTableCellElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
  EPUBTextElement *clone()
  {
    return new CloseTableCellElement();
  }
};

class CloseTableRowElement : public EPUBTextElement
{
public:
  CloseTableRowElement() {}
  ~CloseTableRowElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
  EPUBTextElement *clone()
  {
    return new CloseTableRowElement();
  }
};

class CloseUnorderedListLevelElement : public EPUBTextElement
{
public:
  CloseUnorderedListLevelElement() {}
  ~CloseUnorderedListLevelElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
  EPUBTextElement *clone()
  {
    return new CloseUnorderedListLevelElement();
  }
};

class InsertBinaryObjectElement : public EPUBTextElement
{
public:
  InsertBinaryObjectElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~InsertBinaryObjectElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
  EPUBTextElement *clone()
  {
    return new InsertBinaryObjectElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class InsertCoveredTableCellElement : public EPUBTextElement
{
public:
  InsertCoveredTableCellElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~InsertCoveredTableCellElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
  EPUBTextElement *clone()
  {
    return new InsertCoveredTableCellElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class InsertEquationElement : public EPUBTextElement
{
public:
  InsertEquationElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~InsertEquationElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
  EPUBTextElement *clone()
  {
    return new InsertEquationElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class InsertFieldElement : public EPUBTextElement
{
public:
  InsertFieldElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~InsertFieldElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
  EPUBTextElement *clone()
  {
    return new InsertFieldElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class InsertLineBreakElement : public EPUBTextElement
{
public:
  InsertLineBreakElement() {}
  ~InsertLineBreakElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
  EPUBTextElement *clone()
  {
    return new InsertLineBreakElement();
  }
};

class InsertSpaceElement : public EPUBTextElement
{
public:
  InsertSpaceElement() {}
  ~InsertSpaceElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
  EPUBTextElement *clone()
  {
    return new InsertSpaceElement();
  }
};

class InsertTabElement : public EPUBTextElement
{
public:
  InsertTabElement() {}
  ~InsertTabElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
  EPUBTextElement *clone()
  {
    return new InsertTabElement();
  }
};

class InsertTextElement : public EPUBTextElement
{
public:
  InsertTextElement(const librevenge::RVNGString &text) :
    m_text(text) {}
  ~InsertTextElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
  EPUBTextElement *clone()
  {
    return new InsertTextElement(m_text);
  }
private:
  librevenge::RVNGString m_text;
};

class OpenCommentElement : public EPUBTextElement
{
public:
  OpenCommentElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenCommentElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
  EPUBTextElement *clone()
  {
    return new OpenCommentElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenEndnoteElement : public EPUBTextElement
{
public:
  OpenEndnoteElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenEndnoteElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
  EPUBTextElement *clone()
  {
    return new OpenEndnoteElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenFooterElement : public EPUBTextElement
{
public:
  OpenFooterElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenFooterElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
  EPUBTextElement *clone()
  {
    return new OpenFooterElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenFootnoteElement : public EPUBTextElement
{
public:
  OpenFootnoteElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenFootnoteElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
  EPUBTextElement *clone()
  {
    return new OpenFootnoteElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenFrameElement : public EPUBTextElement
{
public:
  OpenFrameElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenFrameElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
  EPUBTextElement *clone()
  {
    return new OpenFrameElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenHeaderElement : public EPUBTextElement
{
public:
  OpenHeaderElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenHeaderElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
  EPUBTextElement *clone()
  {
    return new OpenHeaderElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenLinkElement : public EPUBTextElement
{
public:
  OpenLinkElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenLinkElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenListElementElement : public EPUBTextElement
{
public:
  OpenListElementElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenListElementElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenOrderedListLevelElement : public EPUBTextElement
{
public:
  OpenOrderedListLevelElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenOrderedListLevelElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenParagraphElement : public EPUBTextElement
{
public:
  OpenParagraphElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenParagraphElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenSpanElement : public EPUBTextElement
{
public:
  OpenSpanElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenSpanElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenTableElement : public EPUBTextElement
{
public:
  OpenTableElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenTableElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenTableCellElement : public EPUBTextElement
{
public:
  OpenTableCellElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenTableCellElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenTableRowElement : public EPUBTextElement
{
public:
  OpenTableRowElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenTableRowElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenUnorderedListLevelElement : public EPUBTextElement
{
public:
  OpenUnorderedListLevelElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenUnorderedListLevelElement() override {}
  void write(librevenge::RVNGTextInterface *iface) const override;
private:
  librevenge::RVNGPropertyList m_propList;
};

void CloseCommentElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->closeComment();
}

void CloseEndnoteElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->closeEndnote();
}

void CloseFooterElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->closeFooter();
}

void CloseFootnoteElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->closeFootnote();
}

void CloseFrameElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->closeFrame();
}


void CloseHeaderElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->closeHeader();
}

void CloseLinkElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->closeLink();
}

void CloseListElementElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->closeListElement();
}

void CloseOrderedListLevelElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->closeOrderedListLevel();
}

void CloseParagraphElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->closeParagraph();
}

void CloseSpanElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->closeSpan();
}

void CloseTableElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->closeTable();
}

void CloseTableCellElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->closeTableCell();
}

void CloseTableRowElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->closeTableRow();
}

void CloseUnorderedListLevelElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->closeUnorderedListLevel();
}

void InsertBinaryObjectElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->insertBinaryObject(m_propList);
}

void InsertCoveredTableCellElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->insertCoveredTableCell(m_propList);
}

void InsertEquationElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->insertEquation(m_propList);
}

void InsertFieldElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->insertField(m_propList);
}

void InsertLineBreakElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->insertLineBreak();
}

void InsertSpaceElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->insertSpace();
}

void InsertTabElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->insertTab();
}

void InsertTextElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->insertText(m_text);
}

void OpenCommentElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->openComment(m_propList);
}

void OpenEndnoteElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->openEndnote(m_propList);
}

void OpenFooterElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->openFooter(m_propList);
}

void OpenFootnoteElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->openFootnote(m_propList);
}

void OpenFrameElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->openFrame(m_propList);
}

void OpenHeaderElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->openHeader(m_propList);
}

void OpenLinkElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->openLink(m_propList);
}

void OpenListElementElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->openListElement(m_propList);
}

void OpenOrderedListLevelElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->openOrderedListLevel(m_propList);
}

void OpenParagraphElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->openParagraph(m_propList);
}

void OpenSpanElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->openSpan(m_propList);
}

void OpenTableElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->openTable(m_propList);
}

void OpenTableCellElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->openTableCell(m_propList);
}

void OpenTableRowElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->openTableRow(m_propList);
}

void OpenUnorderedListLevelElement::write(librevenge::RVNGTextInterface *iface) const
{
  if (iface)
    iface->openUnorderedListLevel(m_propList);
}

}

EPUBTextElements::EPUBTextElements()
  : m_elements()
{
}

EPUBTextElements::~EPUBTextElements()
{
}

void EPUBTextElements::write(librevenge::RVNGTextInterface *iface) const
{
  for (const auto &element : m_elements)
    element->write(iface);
}

void EPUBTextElements::addCloseComment()
{
  m_elements.push_back(make_unique<CloseCommentElement>());
}

void EPUBTextElements::addCloseEndnote()
{
  m_elements.push_back(make_unique<CloseEndnoteElement>());
}

void EPUBTextElements::addCloseFooter()
{
  m_elements.push_back(make_unique<CloseFooterElement>());
}

void EPUBTextElements::addCloseFootnote()
{
  m_elements.push_back(make_unique<CloseFootnoteElement>());
}

void EPUBTextElements::addCloseFrame()
{
  m_elements.push_back(make_unique<CloseFrameElement>());
}

void EPUBTextElements::addCloseHeader()
{
  m_elements.push_back(make_unique<CloseHeaderElement>());
}

void EPUBTextElements::addCloseLink()
{
  m_elements.push_back(make_unique<CloseLinkElement>());
}

void EPUBTextElements::addCloseListElement()
{
  m_elements.push_back(make_unique<CloseListElementElement>());
}

void EPUBTextElements::addCloseOrderedListLevel()
{
  m_elements.push_back(make_unique<CloseOrderedListLevelElement>());
}

void EPUBTextElements::addCloseParagraph()
{
  m_elements.push_back(make_unique<CloseParagraphElement>());
}

void EPUBTextElements::addCloseSpan()
{
  m_elements.push_back(make_unique<CloseSpanElement>());
}

void EPUBTextElements::addCloseTable()
{
  m_elements.push_back(make_unique<CloseTableElement>());
}

void EPUBTextElements::addCloseTableCell()
{
  m_elements.push_back(make_unique<CloseTableCellElement>());
}

void EPUBTextElements::addCloseTableRow()
{
  m_elements.push_back(make_unique<CloseTableRowElement>());
}

void EPUBTextElements::addCloseUnorderedListLevel()
{
  m_elements.push_back(make_unique<CloseUnorderedListLevelElement>());
}

void EPUBTextElements::addInsertBinaryObject(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_unique<InsertBinaryObjectElement>(propList));
}

void EPUBTextElements::addInsertCoveredTableCell(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_unique<InsertCoveredTableCellElement>(propList));
}

void EPUBTextElements::addInsertEquation(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_unique<InsertEquationElement>(propList));
}

void EPUBTextElements::addInsertField(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_unique<InsertFieldElement>(propList));
}

void EPUBTextElements::addInsertLineBreak()
{
  m_elements.push_back(make_unique<InsertLineBreakElement>());
}

void EPUBTextElements::addInsertSpace()
{
  m_elements.push_back(make_unique<InsertSpaceElement>());
}

void EPUBTextElements::addInsertTab()
{
  m_elements.push_back(make_unique<InsertTabElement>());
}

void EPUBTextElements::addInsertText(const librevenge::RVNGString &text)
{
  m_elements.push_back(make_unique<InsertTextElement>(text));
}

void EPUBTextElements::addOpenComment(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_unique<OpenCommentElement>(propList));
}

void EPUBTextElements::addOpenEndnote(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_unique<OpenEndnoteElement>(propList));
}

void EPUBTextElements::addOpenFooter(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_unique<OpenFooterElement>(propList));
}

void EPUBTextElements::addOpenFootnote(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_unique<OpenFootnoteElement>(propList));
}

void EPUBTextElements::addOpenHeader(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_unique<OpenHeaderElement>(propList));
}

void EPUBTextElements::addOpenFrame(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_unique<OpenFrameElement>(propList));
}

void EPUBTextElements::addOpenLink(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_unique<OpenLinkElement>(propList));
}

void EPUBTextElements::addOpenListElement(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_unique<OpenListElementElement>(propList));
}

void EPUBTextElements::addOpenOrderedListLevel(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_unique<OpenOrderedListLevelElement>(propList));
}

void EPUBTextElements::addOpenParagraph(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_unique<OpenParagraphElement>(propList));
}

void EPUBTextElements::addOpenSpan(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_unique<OpenSpanElement>(propList));
}

void EPUBTextElements::addOpenTable(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_unique<OpenTableElement>(propList));
}

void EPUBTextElements::addOpenTableCell(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_unique<OpenTableCellElement>(propList));
}

void EPUBTextElements::addOpenTableRow(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_unique<OpenTableRowElement>(propList));
}

void EPUBTextElements::addOpenUnorderedListLevel(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_unique<OpenUnorderedListLevelElement>(propList));
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
