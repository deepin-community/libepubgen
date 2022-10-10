/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <string>

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "EPUBPath.h"

#define ASSERT_THROW(expression) \
{ \
    bool threw = false; \
    try \
    { \
        (void) expression; \
    } \
    catch (...) \
    { \
        threw = true; \
    } \
    CPPUNIT_ASSERT_MESSAGE(#expression " did not throw any exception", threw); \
}

namespace test
{

using libepubgen::EPUBPath;

using std::string;

class EPUBPathTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(EPUBPathTest);
  CPPUNIT_TEST(testConstruction);
  CPPUNIT_TEST(testAppend);
  CPPUNIT_TEST(testRelative);
  CPPUNIT_TEST(testOperators);
  CPPUNIT_TEST_SUITE_END();

private:
  void testConstruction();
  void testAppend();
  void testRelative();
  void testOperators();
};

void EPUBPathTest::setUp()
{
}

void EPUBPathTest::tearDown()
{
}

void EPUBPathTest::testConstruction()
{
  CPPUNIT_ASSERT("a" == EPUBPath("a").str());
  CPPUNIT_ASSERT("a/b/c" == EPUBPath("a/b/c").str());
  // normalization
  CPPUNIT_ASSERT("a/b/c" == EPUBPath("/a//b///c").str());

  ASSERT_THROW(EPUBPath("."));
  ASSERT_THROW(EPUBPath(".."));
  ASSERT_THROW(EPUBPath("a/./b"));
  ASSERT_THROW(EPUBPath("a/../b"));
}

void EPUBPathTest::testAppend()
{
  EPUBPath path("a");
  path.appendComponent("b");
  CPPUNIT_ASSERT("a/b" == path.str());
  ASSERT_THROW(path.appendComponent("."));
  CPPUNIT_ASSERT("a/b" == path.str());
  ASSERT_THROW(path.appendComponent(".."));
  CPPUNIT_ASSERT("a/b" == path.str());
  ASSERT_THROW(path.appendComponent("/"));
  CPPUNIT_ASSERT("a/b" == path.str());
  ASSERT_THROW(path.appendComponent("c/d"));
  CPPUNIT_ASSERT("a/b" == path.str());

  path.append(EPUBPath("c/d"));
  CPPUNIT_ASSERT("a/b/c/d" == path.str());
}

void EPUBPathTest::testRelative()
{
  const EPUBPath path("a/b/c");

  CPPUNIT_ASSERT_EQUAL(string("b/c"), path.relativeTo(EPUBPath("a/d")).str());
  CPPUNIT_ASSERT_EQUAL(string("c"), path.relativeTo(EPUBPath("a/b/d")).str());
  CPPUNIT_ASSERT_EQUAL(string("../c"), path.relativeTo(EPUBPath("a/b/d/e")).str());
  CPPUNIT_ASSERT_EQUAL(string("../a/b/c"), path.relativeTo(EPUBPath("d/e")).str());
  CPPUNIT_ASSERT_EQUAL(string("../../a/b/c"), path.relativeTo(EPUBPath("d/e/f")).str());

  // pathological cases
  CPPUNIT_ASSERT_EQUAL(string("b/c"), path.relativeTo(EPUBPath("a/b")).str());
  CPPUNIT_ASSERT_EQUAL(string("c"), path.relativeTo(EPUBPath("a/b/c")).str());
  CPPUNIT_ASSERT_EQUAL(string("../c"), path.relativeTo(EPUBPath("a/b/c/d")).str());
}

void EPUBPathTest::testOperators()
{
  CPPUNIT_ASSERT(EPUBPath("a/b") == EPUBPath("a/b"));
  CPPUNIT_ASSERT(EPUBPath("a/b") != EPUBPath("a/b/c"));
  CPPUNIT_ASSERT(EPUBPath("a/b/c") != EPUBPath("a/b"));
  CPPUNIT_ASSERT(EPUBPath("a/b") != EPUBPath("a/c"));
  CPPUNIT_ASSERT(EPUBPath("a/c") != EPUBPath("a/b"));

  CPPUNIT_ASSERT(EPUBPath("a/b/c") == EPUBPath("a") / "b" / "c");
}

CPPUNIT_TEST_SUITE_REGISTRATION(EPUBPathTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
