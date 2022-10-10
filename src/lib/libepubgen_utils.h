/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LIBEPUBGEN_UTILS_H
#define INCLUDED_LIBEPUBGEN_UTILS_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <algorithm>
#include <memory>

#include <boost/cstdint.hpp>

#if defined(HAVE_FUNC_ATTRIBUTE_FORMAT)
#define EPUBGEN_ATTRIBUTE_PRINTF(fmt, arg) __attribute__((format(printf, fmt, arg)))
#else
#define EPUBGEN_ATTRIBUTE_PRINTF(fmt, arg)
#endif

// do nothing with debug messages in a release compile
#ifdef DEBUG

namespace libepubgen
{
void debugPrint(const char *format, ...) EPUBGEN_ATTRIBUTE_PRINTF(1, 2);
}

#define EPUBGEN_DEBUG_MSG(M) libepubgen::debugPrint M
#define EPUBGEN_DEBUG(M) M

#else

#define EPUBGEN_DEBUG_MSG(M)
#define EPUBGEN_DEBUG(M)

#endif

#define EPUBGEN_NUM_ELEMENTS(array) (sizeof(array) / sizeof(array[0]))

namespace libepubgen
{

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args &&... args)
{
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

class GenericException
{
};

} // namespace libepubgen

#endif // INCLUDED_LIBEPUBGEN_UTILS_H

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
