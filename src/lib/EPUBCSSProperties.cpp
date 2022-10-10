/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "EPUBCSSProperties.h"

namespace libepubgen
{

void fillPropertyList(const EPUBCSSProperties &cssProps, librevenge::RVNGPropertyList &props)
{
  for (auto it = cssProps.begin(); cssProps.end() != it; ++it)
    props.insert(it->first.c_str(), librevenge::RVNGPropertyFactory::newStringProp(it->second.c_str()));
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
