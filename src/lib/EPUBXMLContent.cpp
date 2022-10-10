/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "EPUBXMLContent.h"

#include <string>

#include <libepubgen/EPUBPackage.h>

namespace libepubgen
{

using librevenge::RVNGPropertyList;
using librevenge::RVNGString;

class EPUBXMLElement
{
public:
  virtual ~EPUBXMLElement() = 0;

  virtual void writeTo(EPUBPackage &package) const = 0;
};

EPUBXMLElement::~EPUBXMLElement()
{
}

namespace
{

class OpenElement : public EPUBXMLElement
{
  // disable copying
  OpenElement(const OpenElement &);
  OpenElement &operator=(const OpenElement &);

public:
  OpenElement(const char *name, const RVNGPropertyList &attributes);

private:
  void writeTo(EPUBPackage &package) const override;

private:
  const std::string m_name;
  const RVNGPropertyList m_attributes;
};

OpenElement::OpenElement(const char *name, const RVNGPropertyList &attributes)
  : m_name(name)
  , m_attributes(attributes)
{
}

void OpenElement::writeTo(EPUBPackage &package) const
{
  package.openElement(m_name.c_str(), m_attributes);
}

}

namespace
{

class CloseElement : public EPUBXMLElement
{
  // disable copying
  CloseElement(const CloseElement &);
  CloseElement &operator=(const CloseElement &);

public:
  explicit CloseElement(const char *name);

private:
  void writeTo(EPUBPackage &package) const override;

private:
  const std::string m_name;
};

CloseElement::CloseElement(const char *name)
  : m_name(name)
{
}

void CloseElement::writeTo(EPUBPackage &package) const
{
  package.closeElement(m_name.c_str());
}

}

namespace
{

class InsertCharacters : public EPUBXMLElement
{
  // disable copying
  InsertCharacters(const InsertCharacters &);
  InsertCharacters &operator=(const InsertCharacters &);

public:
  explicit InsertCharacters(const RVNGString &characters);

private:
  void writeTo(EPUBPackage &package) const override;

private:
  const RVNGString m_characters;
};

InsertCharacters::InsertCharacters(const RVNGString &characters)
  : m_characters(characters)
{
}

void InsertCharacters::writeTo(EPUBPackage &package) const
{
  package.insertCharacters(m_characters);
}

}

EPUBXMLContent::EPUBXMLContent()
  : m_elements()
{
}

void EPUBXMLContent::openElement(const char *const name, const librevenge::RVNGPropertyList &attributes)
{
  m_elements.push_back(std::make_shared<OpenElement>(name, attributes));
}

void EPUBXMLContent::closeElement(const char *const name)
{
  m_elements.push_back(std::make_shared<CloseElement>(name));
}

void EPUBXMLContent::insertEmptyElement(const char *const name, const librevenge::RVNGPropertyList &attributes)
{
  openElement(name, attributes);
  closeElement(name);
}

void EPUBXMLContent::insertCharacters(const librevenge::RVNGString &characters)
{
  m_elements.push_back(std::make_shared<InsertCharacters>(characters));
}

void EPUBXMLContent::append(const EPUBXMLContent &other)
{
  m_elements.insert(m_elements.end(), other.m_elements.begin(), other.m_elements.end());
}

bool EPUBXMLContent::empty() const
{
  return m_elements.empty();
}

void EPUBXMLContent::writeTo(EPUBPackage &package, const char *const name)
{
  package.openXMLFile(name);
  for (const auto &element : m_elements)
    element->writeTo(package);
  package.closeXMLFile();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
