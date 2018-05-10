/*
 */

/*
 * File:   BTreeTests.h
 * Author: rocik
 *
 * Created on Dec 26, 2017, 10:58:23 AM
 */

#ifndef BTREEFRESHSTARTTESTS_H
#define BTREEFRESHSTARTTESTS_H

#include <cppunit/extensions/HelperMacros.h>

#include "btree.h"


class BTreeFreshStartTests : public CPPUNIT_NS::TestFixture {
	CPPUNIT_TEST_SUITE(BTreeFreshStartTests);

	CPPUNIT_TEST(testFindingNothingInEmptyTree);
	CPPUNIT_TEST(testReadingRecords);
	CPPUNIT_TEST(testFillingRootWithRecords);
	CPPUNIT_TEST(testSplittingRoot);
	CPPUNIT_TEST(testCompensation);
	CPPUNIT_TEST(testFillingRecordsToGet2RecordsInRoot);
	CPPUNIT_TEST(testAddingRecordRandomOrder);

	CPPUNIT_TEST_SUITE_END();
	
public:
	BTreeFreshStartTests();
	virtual ~BTreeFreshStartTests();
	void setUp();
	void tearDown();

private:
	void openFresh(char const* filename);
	
	void testFindingNothingInEmptyTree();
	void testReadingRecords();
	void testFillingRootWithRecords();
	void testSplittingRoot();
	void testCompensation();
	void testFillingRecordsToGet2RecordsInRoot();
	void testAddingRecordRandomOrder();
	
	BTree* btree;
};

#endif /* BTREEFRESHSTARTTESTS_H */

