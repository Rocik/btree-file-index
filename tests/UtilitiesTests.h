/*
 */

/*
 * File:   UtilitiesTests.h
 * Author: rocik
 *
 * Created on Dec 30, 2017, 1:10:40 PM
 */

#ifndef UTILITIESTESTS_H
#define UTILITIESTESTS_H

#include <cppunit/extensions/HelperMacros.h>

#include "util.h"


class UtilitiesTests : public CPPUNIT_NS::TestFixture {
	CPPUNIT_TEST_SUITE(UtilitiesTests);

	CPPUNIT_TEST(testInsertRecordWithOrder_emptyVector);
	CPPUNIT_TEST(testInsertRecordWithOrder_oneInside);
	CPPUNIT_TEST(testInsertRecordWithOrder_twoInside);
	CPPUNIT_TEST(testResolvePagesOrder);
	CPPUNIT_TEST(testMergeRecordsInOrder);

	CPPUNIT_TEST_SUITE_END();

public:
	UtilitiesTests();
	virtual ~UtilitiesTests();
	void setUp();
	void tearDown();

private:
	void testInsertRecordWithOrder_emptyVector();
	void testInsertRecordWithOrder_oneInside();
	void testInsertRecordWithOrder_twoInside();
	void testResolvePagesOrder();
	void testMergeRecordsInOrder();
};

#endif /* UTILITIESTESTS_H */

