/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "EPUBSplitGuard.h"

namespace libepubgen
{

static const unsigned DEFAULT_SPLIT_HEADING_LEVEL = 1;
static const unsigned DEFAULT_SPLIT_SIZE = 1 << 16;

EPUBSplitGuard::EPUBSplitGuard(const EPUBSplitMethod method,bool needSplitOnSecondPageSpan)
  : m_method(method)
  , m_splitOnSecondPageSpan(needSplitOnSecondPageSpan)
  , m_htmlEverInPageSpan(false)
  , m_headingLevel(DEFAULT_SPLIT_HEADING_LEVEL)
  , m_currentHeadingLevel(0)
  , m_size(DEFAULT_SPLIT_SIZE)
  , m_currentSize(0)
  , m_nestingLevel(0)
{
}

void EPUBSplitGuard::setHtmlEverInPageSpan(bool value)
{
  m_htmlEverInPageSpan = value;
}

void EPUBSplitGuard::setSplitHeadingLevel(const unsigned level)
{
  m_headingLevel = level;
}

void EPUBSplitGuard::setCurrentHeadingLevel(const unsigned level)
{
  m_currentHeadingLevel = level;
}

void EPUBSplitGuard::setSplitSize(const unsigned size)
{
  m_size = size;
}

void EPUBSplitGuard::setSplitMethod(EPUBSplitMethod method)
{
  m_method = method;
}

void EPUBSplitGuard::setSplitOnSecondPageSpan(bool value)
{
  m_splitOnSecondPageSpan = value;
}

void EPUBSplitGuard::openLevel()
{
  ++m_nestingLevel;
}

void EPUBSplitGuard::closeLevel()
{
  --m_nestingLevel;
}

void EPUBSplitGuard::incrementSize(const unsigned size)
{
  m_currentSize += size;
}

bool EPUBSplitGuard::splitOnPageBreak() const
{
  return canSplit(EPUB_SPLIT_METHOD_PAGE_BREAK);
}

bool EPUBSplitGuard::splitOnHeading(const unsigned level) const
{
  return canSplit(EPUB_SPLIT_METHOD_HEADING) && (m_headingLevel >= level);
}

bool EPUBSplitGuard::splitOnSecondPageSpan() const
{
  return m_splitOnSecondPageSpan && m_htmlEverInPageSpan;
}

bool EPUBSplitGuard::inHeading(bool any) const
{
  if (!m_currentHeadingLevel)
    return false;

  if (any)
    return true;

  return m_headingLevel >= m_currentHeadingLevel;
}

bool EPUBSplitGuard::splitOnSize() const
{
  return canSplit(EPUB_SPLIT_METHOD_SIZE) && (m_size <= m_currentSize);
}

void EPUBSplitGuard::onSplit()
{
  m_currentSize = 0;
}

bool EPUBSplitGuard::canSplit(const EPUBSplitMethod method) const
{
  return (method == m_method) && (0 == m_nestingLevel) && (0 != m_currentSize);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
