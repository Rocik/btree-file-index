/*
 */

/*
 * File:   BTreePage.h
 * Author: rocik
 *
 * Created on Dec 25, 2017, 9:29:03 PM
 */

#ifndef BTREEPAGETESTS_H
#define BTREEPAGETESTS_H

#include <cppunit/extensions/HelperMacros.h>

#include "btreePage.h"

class BTreePageTests : public CPPUNIT_NS::TestFixture {
	CPPUNIT_TEST_SUITE(BTreePageTests);

	CPPUNIT_TEST(testPropertiesFromBinary);
	CPPUNIT_TEST(testRecordsFromBinary);
	CPPUNIT_TEST(testDeserializingToBinary);
	CPPUNIT_TEST(testRootPageCreation);
	CPPUNIT_TEST(testFullyUsedSpace);
	CPPUNIT_TEST(testRecordInsertion);
	CPPUNIT_TEST(testRecordInsertionInside);
	CPPUNIT_TEST(testFindingKey);
	CPPUNIT_TEST(testFindingKeyInLeaf);
	CPPUNIT_TEST(testGettingNeighbors);
	CPPUNIT_TEST(testGettingRecordsByNeighbors);
	CPPUNIT_TEST(testReplacingRecord);

	CPPUNIT_TEST_SUITE_END();

public:
	BTreePageTests();
	virtual ~BTreePageTests();
	void setUp();
	void tearDown();

private:
	void testPropertiesFromBinary();
	void testRecordsFromBinary();
	void testDeserializingToBinary();
	void testRootPageCreation();
	void testFullyUsedSpace();
	void testRecordInsertion();
	void testRecordInsertionInside();
	void testFindingKey();
	void testFindingKeyInLeaf();
	void testGettingNeighbors();
	void testGettingRecordsByNeighbors();
	void testReplacingRecord();
	
	char* binarySource;
	BTreePage* btreePage;
	BTreePage* btreePageRoot;
	BTreePage* btreePageFull;
	BTreePage* btreePageReplc;
	BTreePage* btreePageInsrt;
};

#endif /* BTREEPAGETESTS_H */

