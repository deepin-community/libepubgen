/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_EPUBCSSPROPERTIES_H
#define INCLUDED_EPUBCSSPROPERTIES_H

#include <map>
#include <string>

#include <librevenge/librevenge.h>

namespace libepubgen
{

typedef std::map<std::string, std::string> EPUBCSSProperties;

void fillPropertyList(const EPUBCSSProperties &cssProps, librevenge::RVNGPropertyList &props);

}

#endif // INCLUDED_EPUBCSSPROPERTIES

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
