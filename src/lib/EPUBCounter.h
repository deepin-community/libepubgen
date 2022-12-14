/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_EPUBCOUNTER_H
#define INCLUDED_EPUBCOUNTER_H

namespace libepubgen
{

class EPUBCounter
{
  // disable copying
  EPUBCounter(const EPUBCounter &);
  EPUBCounter &operator=(const EPUBCounter &);

public:
  EPUBCounter();

  unsigned current() const;
  unsigned next();

private:
  unsigned m_current;
};

}

#endif // INCLUDED_EPUBCOUNTER_H

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
