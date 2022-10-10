/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_EPUBCSSCONTENT_H
#define INCLUDED_EPUBCSSCONTENT_H

#include <utility>
#include <vector>

#include <librevenge/librevenge.h>

namespace libepubgen
{

class EPUBPackage;

class EPUBCSSContent
{
  typedef std::vector<std::pair<librevenge::RVNGString, librevenge::RVNGPropertyList> > Rules_t;

public:
  EPUBCSSContent();

  void insertRule(const librevenge::RVNGString &selector, const librevenge::RVNGPropertyList &properties);

  void writeTo(EPUBPackage &package, const char *name);

private:
  Rules_t m_rules;
};

}

#endif // INCLUDED_EPUBCSSCONTENT_H

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
