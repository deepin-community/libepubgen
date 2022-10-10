## -*- Mode: make; tab-width: 4; indent-tabs-mode: tabs -*-

target_test = test

check_PROGRAMS = $(target_test)

AM_CXXFLAGS = \
	-I$(top_srcdir)/inc \
	-I$(top_srcdir)/src/lib \
	$(CPPUNIT_CFLAGS) \
	$(LIBXML_CFLAGS) \
	$(REVENGE_CFLAGS) \
	$(REVENGE_STREAM_CFLAGS) \
	$(DEBUG_CXXFLAGS)

test_LDFLAGS = -L$(top_srcdir)/src/lib
test_LDADD = \
	$(top_builddir)/src/lib/libepubgen_internal.la \
	$(top_builddir)/src/lib/libepubgen-@EPUBGEN_MAJOR_VERSION@.@EPUBGEN_MINOR_VERSION@.la \
	$(CPPUNIT_LIBS) \
	$(LIBXML_LIBS) \
	$(REVENGE_LIBS) \
	$(REVENGE_STREAM_LIBS)

test_SOURCES = \
	EPUBPathTest.cpp \
	EPUBTextGeneratorTest.cpp \
	test.cpp

TESTS = $(target_test)

## vim:set shiftwidth=4 tabstop=4 noexpandtab:
