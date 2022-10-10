/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "EPUBCSSContent.h"

#include <libepubgen/EPUBPackage.h>

namespace libepubgen
{

EPUBCSSContent::EPUBCSSContent()
  : m_rules()
{
}

void EPUBCSSContent::insertRule(const librevenge::RVNGString &selector, const librevenge::RVNGPropertyList &properties)
{
  m_rules.push_back(std::make_pair(selector, properties));
}

void EPUBCSSContent::writeTo(EPUBPackage &package, const char *const name)
{
  package.openCSSFile(name);

  for (Rules_t::const_iterator it = m_rules.begin(); m_rules.end() != it; ++it)
    package.insertRule(it->first, it->second);

  package.closeCSSFile();
}

} // namespace libepubgen

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
