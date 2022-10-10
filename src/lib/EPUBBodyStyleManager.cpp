/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "EPUBBodyStyleManager.h"

namespace libepubgen
{

using librevenge::RVNGPropertyList;

void EPUBBodyStyleManager::extractProperties(RVNGPropertyList const &pList, EPUBCSSProperties &cssProps) const
{
  if (pList["style:writing-mode"])
  {
    std::string mode = pList["style:writing-mode"]->getStr().cstr();
    if (mode == "tb-rl" || mode == "tb")
      mode = "vertical-rl";
    else if (mode == "tb-lr")
      mode = "vertical-lr";
    else // For the rest: lr, lr-tb, rl, rl-tb
    {
      mode = "horizontal-tb";
      cssProps["direction"] = (mode == "rl-tb" || mode == "rl")?"rtl":"ltr";
    }

    cssProps["-epub-writing-mode"] = mode;
    cssProps["-webkit-writing-mode"] = mode;
    cssProps["writing-mode"] = mode;
  }
}

}
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
