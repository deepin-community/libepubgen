/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "EPUBPath.h"

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <utility>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace libepubgen
{

namespace algorithm = boost::algorithm;

namespace
{

struct FindDots
{
  bool operator()(const std::string &component)
  {
    return ("." == component) || (".." == component);
  }
};

bool findAnyDots(const std::vector<std::string> &components)
{
  return components.end() != find_if(components.begin(), components.end(), FindDots());
}

}

const std::string EPUBPath::Relative::str() const
{
  return algorithm::join(m_components, "/");
}

EPUBPath::Relative::Relative(const std::vector<std::string> &components)
  : m_components(components)
{
}

EPUBPath::EPUBPath(const std::string &path)
  : m_components()
  , m_title()
  , m_chapters()
{
  const std::string trimmed(algorithm::trim_left_copy_if(path, algorithm::is_any_of("/")));
  algorithm::split(m_components, trimmed, algorithm::is_any_of("/"), algorithm::token_compress_on);

  if (m_components.empty() || m_components.back().empty() || findAnyDots(m_components))
    throw std::logic_error("invalid path");
}

void EPUBPath::swap(EPUBPath &other)
{
  m_components.swap(other.m_components);
}

void EPUBPath::append(const EPUBPath &subpath)
{
  m_components.insert(m_components.end(), subpath.m_components.begin(), subpath.m_components.end());
}

void EPUBPath::appendComponent(const std::string &pathComponent)
{
  if (std::string::npos != pathComponent.find('/'))
    throw std::logic_error("the component cannot be path");
  if (("." == pathComponent) || (".." == pathComponent))
    throw std::logic_error("the component cannot be relative");

  m_components.push_back(pathComponent);
}

const std::string EPUBPath::str() const
{
  return algorithm::join(m_components, "/");
}

const EPUBPath::Relative EPUBPath::relativeTo(const EPUBPath &base) const
{
  typedef std::vector<std::string> Path_t;
  typedef Path_t::const_iterator PathIter_t;

  const PathIter_t baseEnd = base.m_components.end() - 1;
  const Path_t::size_type baseSize = base.m_components.size() - 1;

  const std::pair<PathIter_t, PathIter_t> mismatch(
    std::mismatch(
      m_components.begin(),
      // If base has more or the same number components than this, leave out the last component.
      // This ensures we always get path starting with ../, even if there is a full match.
      (baseSize >= m_components.size()) ? (m_components.end() - 1) : (m_components.begin() + baseSize),
      base.m_components.begin()));

  Path_t components;

  std::fill_n(std::back_inserter(components), std::distance(mismatch.second, baseEnd), std::string(".."));
  std::copy(mismatch.first, m_components.end(), std::back_inserter(components));

  return Relative(components);
}

void EPUBPath::appendTitle(const std::string &title)
{
  m_title += title;
}

void EPUBPath::addChapter(const std::string &chapter)
{
  m_chapters.push_back(chapter);
}

const std::vector<std::string> &EPUBPath::getChapters() const
{
  return m_chapters;
}

std::string EPUBPath::getTitle() const
{
  return m_title;
}

bool operator==(const EPUBPath &left, const EPUBPath &right)
{
  return left.m_components == right.m_components;
}

bool operator!=(const EPUBPath &left, const EPUBPath &right)
{
  return !(left == right);
}

const EPUBPath operator/(const EPUBPath &base, const EPUBPath &subpath)
{
  EPUBPath path(base);
  path.append(subpath);
  return path;
}

const EPUBPath operator/(const EPUBPath &base, const std::string &pathComponent)
{
  EPUBPath path(base);
  path.appendComponent(pathComponent);
  return path;
}

void swap(EPUBPath &left, EPUBPath &right)
{
  left.swap(right);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
