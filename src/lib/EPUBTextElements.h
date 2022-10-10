/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_EPUBTEXTELEMENTS_H
#define INCLUDED_EPUBTEXTELEMENTS_H

#include <deque>
#include <memory>

#include <librevenge/librevenge.h>

namespace libepubgen
{

class EPUBTextElement;

class EPUBTextElements
{
  // disable copying
  EPUBTextElements(const EPUBTextElements &);
  EPUBTextElements &operator=(const EPUBTextElements &);

public:
  EPUBTextElements();
  ~EPUBTextElements();

  void append(const EPUBTextElements &elements);

  void write(librevenge::RVNGTextInterface *iface) const;

  void addCloseComment();
  void addCloseEndnote();
  void addCloseFooter();
  void addCloseFootnote();
  void addCloseFrame();
  void addCloseHeader();
  void addCloseLink();
  void addCloseListElement();
  void addCloseOrderedListLevel();
  void addCloseParagraph();
  void addCloseSpan();
  void addCloseTable();
  void addCloseTableCell();
  void addCloseTableRow();
  void addCloseUnorderedListLevel();
  void addInsertBinaryObject(const librevenge::RVNGPropertyList &propList);
  void addInsertCoveredTableCell(const librevenge::RVNGPropertyList &propList);
  void addInsertEquation(const librevenge::RVNGPropertyList &propList);
  void addInsertField(const librevenge::RVNGPropertyList &propList);
  void addInsertLineBreak();
  void addInsertSpace();
  void addInsertTab();
  void addInsertText(const librevenge::RVNGString &text);
  void addOpenComment(const librevenge::RVNGPropertyList &propList);
  void addOpenEndnote(const librevenge::RVNGPropertyList &propList);
  void addOpenFooter(const librevenge::RVNGPropertyList &propList);
  void addOpenFootnote(const librevenge::RVNGPropertyList &propList);
  void addOpenFrame(const librevenge::RVNGPropertyList &propList);
  void addOpenHeader(const librevenge::RVNGPropertyList &propList);
  void addOpenLink(const librevenge::RVNGPropertyList &propList);
  void addOpenListElement(const librevenge::RVNGPropertyList &propList);
  void addOpenOrderedListLevel(const librevenge::RVNGPropertyList &propList);
  void addOpenParagraph(const librevenge::RVNGPropertyList &propList);
  void addOpenSpan(const librevenge::RVNGPropertyList &propList);
  void addOpenTable(const librevenge::RVNGPropertyList &propList);
  void addOpenTableCell(const librevenge::RVNGPropertyList &propList);
  void addOpenTableRow(const librevenge::RVNGPropertyList &propList);
  void addOpenUnorderedListLevel(const librevenge::RVNGPropertyList &propList);

  bool empty() const
  {
    return m_elements.empty();
  }

private:
  std::deque<std::unique_ptr<EPUBTextElement>> m_elements;
};

}

#endif // INCLUDED_EPUBTEXTELEMENTS_H

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
