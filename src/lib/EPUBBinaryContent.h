/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_EPUBBINARYCONTENT_H
#define INCLUDED_EPUBBINARYCONTENT_H

#include <librevenge/librevenge.h>

namespace libepubgen
{

class EPUBPackage;

class EPUBBinaryContent
{
public:
  EPUBBinaryContent();

  void insertBinaryData(const librevenge::RVNGBinaryData &data);

  void writeTo(EPUBPackage &package, const char *name);

private:
  librevenge::RVNGBinaryData m_data;
};

}

#endif // INCLUDED_EPUBBINARYCONTENT_H

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
