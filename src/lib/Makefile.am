## -*- Mode: make; tab-width: 4; indent-tabs-mode: tabs -*-

if PLATFORM_WIN32
version_info = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -avoid-version
else
version_info = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)
endif

noinst_LTLIBRARIES = libepubgen_internal.la
lib_LTLIBRARIES = libepubgen-@EPUBGEN_MAJOR_VERSION@.@EPUBGEN_MINOR_VERSION@.la

AM_CXXFLAGS = \
	-DEPUBGEN_BUILD=1 \
	-I$(top_srcdir)/inc \
	$(REVENGE_CFLAGS) \
	$(REVENGE_STREAM_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(DEBUG_CXXFLAGS)

libepubgen_@EPUBGEN_MAJOR_VERSION@_@EPUBGEN_MINOR_VERSION@_la_LIBADD = \
	$(REVENGE_LIBS) \
	libepubgen_internal.la \
	@LIBEPUBGEN_WIN32_RESOURCE@

libepubgen_@EPUBGEN_MAJOR_VERSION@_@EPUBGEN_MINOR_VERSION@_la_DEPENDENCIES = libepubgen_internal.la @LIBEPUBGEN_WIN32_RESOURCE@
libepubgen_@EPUBGEN_MAJOR_VERSION@_@EPUBGEN_MINOR_VERSION@_la_LDFLAGS = $(version_info) -export-dynamic -no-undefined
libepubgen_@EPUBGEN_MAJOR_VERSION@_@EPUBGEN_MINOR_VERSION@_la_SOURCES = \
	EPUBDrawingGenerator.cpp \
	EPUBPresentationGenerator.cpp \
	EPUBTextGenerator.cpp

libepubgen_internal_la_SOURCES = \
	EPUBBinaryContent.cpp \
	EPUBBinaryContent.h \
	EPUBBodyStyleManager.cpp \
	EPUBBodyStyleManager.h \
	EPUBCounter.cpp \
	EPUBCounter.h \
	EPUBCSSContent.cpp \
	EPUBCSSContent.h \
	EPUBCSSProperties.cpp \
	EPUBCSSProperties.h \
	EPUBFontManager.cpp \
	EPUBFontManager.h \
	EPUBGenerator.cpp \
	EPUBGenerator.h \
	EPUBHTMLGenerator.cpp \
	EPUBHTMLGenerator.h \
	EPUBHTMLManager.cpp \
	EPUBHTMLManager.h \
	EPUBImageManager.cpp \
	EPUBImageManager.h \
	EPUBListStyleManager.cpp \
	EPUBListStyleManager.h \
	EPUBManifest.cpp \
	EPUBManifest.h \
	EPUBPagedGenerator.cpp \
	EPUBPagedGenerator.h \
	EPUBParagraphStyleManager.cpp \
	EPUBParagraphStyleManager.h \
	EPUBPath.cpp \
	EPUBPath.h \
	EPUBSpanStyleManager.cpp \
	EPUBSpanStyleManager.h \
	EPUBSplitGuard.cpp \
	EPUBSplitGuard.h \
	EPUBTableStyleManager.cpp \
	EPUBTableStyleManager.h \
	EPUBTextElements.cpp \
	EPUBTextElements.h \
	EPUBXMLContent.cpp \
	EPUBXMLContent.h \
	libepubgen_utils.cpp \
	libepubgen_utils.h

if OS_WIN32

@LIBEPUBGEN_WIN32_RESOURCE@ : libepubgen.rc $(libepubgen_@EPUBGEN_MAJOR_VERSION@_@EPUBGEN_MINOR_VERSION@_la_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource libepubgen.rc @LIBEPUBGEN_WIN32_RESOURCE@

endif

EXTRA_DIST = \
	libepubgen.rc.in \
	libepubgen.rc

## vim:set shiftwidth=4 tabstop=4 noexpandtab:
