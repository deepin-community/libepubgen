/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_LIBEPUBGEN_LIBEPUBGEN_API_H
#define INCLUDED_LIBEPUBGEN_LIBEPUBGEN_API_H

#ifdef DLL_EXPORT
#ifdef EPUBGEN_BUILD
#define EPUBGENAPI __declspec(dllexport)
#else
#define EPUBGENAPI __declspec(dllimport)
#endif
#else // !DLL_EXPORT
#ifdef LIBEPUBGEN_VISIBILITY
#define EPUBGENAPI __attribute__((visibility("default")))
#else
#define EPUBGENAPI
#endif
#endif

#endif // INCLUDED_LIBEPUBGEN_LIBEPUBGEN_API_H

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
