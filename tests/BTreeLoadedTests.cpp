/*
 */

/*
 * File:   BTreeLoadedTests.cpp
 * Author: rocik
 *
 * Created on Dec 26, 2017, 3:54:39 PM
 */

#include "BTreeLoadedTests.h"


CPPUNIT_TEST_SUITE_REGISTRATION(BTreeLoadedTests);


BTreeLoadedTests::BTreeLoadedTests() {
	btree = nullptr;
}


BTreeLoadedTests::~BTreeLoadedTests() {
	tearDown();
}


void BTreeLoadedTests::setUp() {
	btree = new BTree("BTreeLoadedTests");
	for (int i = 1; i <= 14; ++i)
		btree->insert( Record(i, 100 + i) );
	
	delete btree;
	btree = new BTree("BTreeLoadedTests");
}


void BTreeLoadedTests::tearDown() {
	if (btree != nullptr) {
		btree->dispose();
		delete btree;
		btree = nullptr;
	}
}


void BTreeLoadedTests::testLoadedRecords() {
	string treeFilename = string("BTreeLoadedTests") + BTREE_FILE_EXTENSION;
	auto pageBytes = make_unique<char[]>(btree->getPageSizeInBytes());
	DiskFile file(treeFilename, btree->getPageSizeInBytes());
	file.readPage(0, pageBytes.get());
	BTreePage page0(3, pageBytes.get());
	file.readPage(1, pageBytes.get());
	BTreePage page1(3, pageBytes.get());
	file.readPage(2, pageBytes.get());
	BTreePage page2(3, pageBytes.get());
	file.readPage(3, pageBytes.get());
	BTreePage page3(3, pageBytes.get());
	
	auto r1 = page0.getRecords();
	auto r2 = page1.getRecords();
	auto r3 = page2.getRecords();
	auto r4 = page3.getRecords();
	
	CPPUNIT_ASSERT_EQUAL(1, r1[0].getKey());
	CPPUNIT_ASSERT_EQUAL(2, r1[1].getKey());
	CPPUNIT_ASSERT_EQUAL(3, r1[2].getKey());
	CPPUNIT_ASSERT_EQUAL(4, r1[3].getKey());
	CPPUNIT_ASSERT_EQUAL(5, r1[4].getKey());
	CPPUNIT_ASSERT_EQUAL(6, r1[5].getKey());
	
	CPPUNIT_ASSERT_EQUAL(8,  r2[0].getKey());
	CPPUNIT_ASSERT_EQUAL(9,  r2[1].getKey());
	CPPUNIT_ASSERT_EQUAL(10, r2[2].getKey());
	
	CPPUNIT_ASSERT_EQUAL(7,  r3[0].getKey());
	CPPUNIT_ASSERT_EQUAL(11, r3[1].getKey());
	
	CPPUNIT_ASSERT_EQUAL(12, r4[0].getKey());
	CPPUNIT_ASSERT_EQUAL(13, r4[1].getKey());
	CPPUNIT_ASSERT_EQUAL(14, r4[2].getKey());
	
	CPPUNIT_ASSERT_EQUAL(0, page2.getFirstChildPage());
	CPPUNIT_ASSERT_EQUAL(1, r3[0].getRightChildPage());
	CPPUNIT_ASSERT_EQUAL(3, r3[1].getRightChildPage());
}

