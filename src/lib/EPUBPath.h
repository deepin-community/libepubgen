/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_EPUBPATH_H
#define INCLUDED_EPUBPATH_H

#include <string>
#include <vector>

namespace libepubgen
{

/** Representation of a path in the package.
  */
class EPUBPath
{
  friend bool operator==(const EPUBPath &left, const EPUBPath &right);

public:
  class Relative
  {
    friend class EPUBPath;

  public:
    const std::string str() const;

  private:
    explicit Relative(const std::vector<std::string> &components);

  private:
    std::vector<std::string> m_components;
  };

  explicit EPUBPath(const std::string &path);

  void swap(EPUBPath &other);

  void append(const EPUBPath &subpath);
  void appendComponent(const std::string &pathComponent);

  const std::string str() const;

  const Relative relativeTo(const EPUBPath &base) const;

  void appendTitle(const std::string &title);
  std::string getTitle() const;

  /// Adds chapter name (fixed layout).
  void addChapter(const std::string &chapter);
  const std::vector<std::string> &getChapters() const;
private:
  std::vector<std::string> m_components;
  std::string m_title;
  std::vector<std::string> m_chapters;
};

bool operator==(const EPUBPath &left, const EPUBPath &right);
bool operator!=(const EPUBPath &left, const EPUBPath &right);

const EPUBPath operator/(const EPUBPath &base, const EPUBPath &subpath);
const EPUBPath operator/(const EPUBPath &base, const std::string &pathComponent);

void swap(EPUBPath &left, EPUBPath &right);

}

#endif // INCLUDED_EPUBPATH

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
