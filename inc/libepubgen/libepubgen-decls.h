/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LIBEPUBGEN_LIBEPUBGEN_DECLS_H
#define INCLUDED_LIBEPUBGEN_LIBEPUBGEN_DECLS_H

#include <librevenge/librevenge.h>

namespace libepubgen
{

/** Possible type of image for embedded images handler.
  */
enum EPUBImageType
{
  EPUB_IMAGE_TYPE_GIF, //< GIF
  EPUB_IMAGE_TYPE_JPEG, //< JPEG
  EPUB_IMAGE_TYPE_PNG, //< PNG
  EPUB_IMAGE_TYPE_SVG //< SVG
};

/** The possible ways to split the content to multiple HTML files.
  */
enum EPUBSplitMethod
{
  EPUB_SPLIT_METHOD_PAGE_BREAK, //< The content will be split on explicit page breaks.
  EPUB_SPLIT_METHOD_HEADING, //< The content will be split on headings.
  EPUB_SPLIT_METHOD_SIZE, //< The content will be split into roughly equal-sized chunks.
  EPUB_SPLIT_METHOD_NONE, //< The whole content will be in a single HTML.
  EPUB_SPLIT_METHOD_DETECT //< The first method that actually results in splitting will be used.
};

/** An opaque type used for communication with the parent generator.
  *
  * Embedded objects are generated into completely separate flows. But there are
  * still items that the generator for the embedded object needs to push into
  * its parent generator; manifest entries, for example. This class is used for
  * that purpose.
  */
class EPUBEmbeddingContact;

/** Handler for embedded images.
  *
  * This must convert the input image to one of the OPS Core Media Types:
  * GIF, JPEG, PNG or SVG.
  *
  * @param[in] input the image's data
  * @param[in] output the same image in one of the OPS Core Media Types
  * @param[in] type type of the output image
  */
typedef bool (*EPUBEmbeddedImage)(const librevenge::RVNGBinaryData &input, librevenge::RVNGBinaryData &output, EPUBImageType &type);

/** Handler for embedded objects.
  *
  * @param[in] input the objects's data
  * @param[in] contact the contact to the parent generator
  */
typedef bool (*EPUBEmbeddedObject)(const librevenge::RVNGBinaryData &data, const EPUBEmbeddingContact &contact);

/** The possible ways to represent styles in CSS/HTML files.
  */
enum EPUBStylesMethod
{
  EPUB_STYLES_METHOD_CSS, //< The styles will be described in a seprarate CSS file.
  EPUB_STYLES_METHOD_INLINE, //< The styles will be described inline.
};

/** The possible ways to lay out HTML files.
  */
enum EPUBLayoutMethod
{
  EPUB_LAYOUT_METHOD_REFLOWABLE, //< Dynamic pagination.
  EPUB_LAYOUT_METHOD_FIXED, //< Exactly one page per HTML file.
};

/** The possible options for a generator.
  */
enum EPUBGeneratorOption
{
  EPUB_GENERATOR_OPTION_SPLIT, //< EPUBSplitMethod.
  EPUB_GENERATOR_OPTION_STYLES, //< EPUBStylesMethod.
  EPUB_GENERATOR_OPTION_LAYOUT //< EPUBLayoutMethod.
};

}

#endif // INCLUDED_LIBEPUBGEN_LIBEPUBGEN_DECLS_H

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
