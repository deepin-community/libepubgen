/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "EPUBBinaryContent.h"

#include <libepubgen/EPUBPackage.h>

namespace libepubgen
{

EPUBBinaryContent::EPUBBinaryContent()
  : m_data()
{
}

void EPUBBinaryContent::insertBinaryData(const librevenge::RVNGBinaryData &data)
{
  m_data.append(data);
}

void EPUBBinaryContent::writeTo(EPUBPackage &package, const char *const name)
{
  package.openBinaryFile(name);
  package.insertBinaryData(m_data);
  package.closeBinaryFile();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
