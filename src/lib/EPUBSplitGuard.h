/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_EPUBSPLITGUARD_H
#define INCLUDED_EPUBSPLITGUARD_H

#include <libepubgen/libepubgen-decls.h>

namespace libepubgen
{

class EPUBSplitGuard
{
public:
  explicit EPUBSplitGuard(EPUBSplitMethod method,bool splitOnSecondPageSpan);

  void setSplitHeadingLevel(unsigned level);
  void setCurrentHeadingLevel(unsigned level);
  void setSplitSize(unsigned size);
  void setHtmlEverInPageSpan(bool value);
  /// Allows overwriting the value given in the constructor.
  void setSplitMethod(EPUBSplitMethod method);
  void setSplitOnSecondPageSpan(bool value);

  void openLevel();
  void closeLevel();
  void incrementSize(unsigned size);

  bool splitOnPageBreak() const;
  bool splitOnHeading(unsigned level) const;
  bool splitOnSecondPageSpan() const;
  bool inHeading(bool any) const;
  bool splitOnSize() const;

  void onSplit();

private:
  bool canSplit(EPUBSplitMethod method) const;

private:
  EPUBSplitMethod m_method;
  bool m_splitOnSecondPageSpan;
  bool m_htmlEverInPageSpan;
  unsigned m_headingLevel;
  unsigned m_currentHeadingLevel;
  unsigned m_size;
  unsigned m_currentSize;
  unsigned m_nestingLevel;
};

}

#endif // INCLUDED_EPUBSPLITGUARD

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
