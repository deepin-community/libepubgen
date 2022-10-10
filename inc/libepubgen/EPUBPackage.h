/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LIBEPUBGEN_EPUBPACKAGE_H
#define INCLUDED_LIBEPUBGEN_EPUBPACKAGE_H

#include <librevenge/librevenge.h>
#include <librevenge-stream/librevenge-stream.h>

namespace libepubgen
{

/** An abstraction of EPUB package.
  *
  * This interface must be implemented by the caller.
  *
  * There are 4 types of files that can be inserted into the package: XML, CSS, binary and text.
  * There is really not expected any difference in their handling, as just binary file would be
  * sufficient. The separate types exist partly to make an interface similar to @c
  * libodfgen::OdfDocumentHandler, partly for convenience (no need to convert, e.g., @c
  * librevenge::RVNGString to @c librevenge::RVNGBinaryData in the library itself), partly for
  * simplifying unit testing of the generators.
  */
class EPUBPackage
{
public:
  virtual ~EPUBPackage() {}

  virtual void openXMLFile(const char *name) = 0;

  virtual void openElement(const char *name, const librevenge::RVNGPropertyList &attributes) = 0;
  virtual void closeElement(const char *name) = 0;

  /// Insert characters into an opened XML file.
  virtual void insertCharacters(const librevenge::RVNGString &characters) = 0;

  virtual void closeXMLFile() = 0;

  virtual void openCSSFile(const char *name) = 0;

  virtual void insertRule(const librevenge::RVNGString &selector, const librevenge::RVNGPropertyList &properties) = 0;

  virtual void closeCSSFile() = 0;

  virtual void openBinaryFile(const char *name) = 0;

  virtual void insertBinaryData(const librevenge::RVNGBinaryData &data) = 0;

  virtual void closeBinaryFile() = 0;

  virtual void openTextFile(const char *name) = 0;

  /// Insert text into an opened text file.
  virtual void insertText(const librevenge::RVNGString &characters) = 0;
  virtual void insertLineBreak() = 0;

  virtual void closeTextFile() = 0;
};

} // namespace libepubgen

#endif // INCLUDED_LIBEPUBGEN_EPUBPACKAGE_H

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
