/*
 */

/*
 * File:   BTreeLoadedTests.h
 * Author: rocik
 *
 * Created on Dec 26, 2017, 3:54:39 PM
 */

#ifndef BTREELOADEDTESTS_H
#define BTREELOADEDTESTS_H

#include <cppunit/extensions/HelperMacros.h>

#include "btree.h"


class BTreeLoadedTests : public CPPUNIT_NS::TestFixture {
	CPPUNIT_TEST_SUITE(BTreeLoadedTests);

	CPPUNIT_TEST(testLoadedRecords);

	CPPUNIT_TEST_SUITE_END();

public:
	BTreeLoadedTests();
	virtual ~BTreeLoadedTests();
	void setUp();
	void tearDown();

private:
	void testLoadedRecords();
	
	BTree* btree;
};

#endif /* BTREELOADEDTESTS_H */

