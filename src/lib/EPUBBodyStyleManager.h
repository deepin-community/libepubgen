/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_EPUBBODYSTYLEMANAGER_H
#define INCLUDED_EPUBBODYSTYLEMANAGER_H
#include "EPUBSpanStyleManager.h"

namespace libepubgen
{

/** EPUBBodyStyleManager manages the body style converted from properties of the page span. */
class EPUBBodyStyleManager: public EPUBSpanStyleManager
{

public:
  //! constructor
  EPUBBodyStyleManager(std::string classNamePrefix) : EPUBSpanStyleManager(classNamePrefix) {}
  //! destructor
  virtual ~EPUBBodyStyleManager() {}
protected:
  //! convert the properties of the page span into a CSS property map. Currently only style:writing-mode is supported.
  virtual void extractProperties(librevenge::RVNGPropertyList const &pList, EPUBCSSProperties &cssProps) const override;
};

}
#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
