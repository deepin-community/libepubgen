/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "EPUBCounter.h"

namespace libepubgen
{

EPUBCounter::EPUBCounter()
  : m_current(0)
{
}

unsigned EPUBCounter::current() const
{
  return m_current;
}

unsigned EPUBCounter::next()
{
  return ++m_current;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
