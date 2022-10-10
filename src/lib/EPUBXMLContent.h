/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_EPUBXMLCONTENT_H
#define INCLUDED_EPUBXMLCONTENT_H

#include <deque>
#include <memory>

#include <librevenge/librevenge.h>

namespace libepubgen
{

class EPUBPackage;
class EPUBXMLElement;

class EPUBXMLContent
{
public:
  EPUBXMLContent();

  void openElement(const char *name, const librevenge::RVNGPropertyList &attributes = librevenge::RVNGPropertyList());
  void closeElement(const char *name);

  void insertEmptyElement(const char *name, const librevenge::RVNGPropertyList &attributes = librevenge::RVNGPropertyList());

  void insertCharacters(const librevenge::RVNGString &characters);

  void append(const EPUBXMLContent &other);

  void writeTo(EPUBPackage &package, const char *name);

  bool empty() const;

private:
  std::deque<std::shared_ptr<EPUBXMLElement>> m_elements;
};

}

#endif // INCLUDED_EPUBXMLCONTENT_H

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
