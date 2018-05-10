/*
 */

/*
 * File:   BTreePage.cpp
 * Author: rocik
 *
 * Created on Dec 25, 2017, 9:29:03 PM
 */

#include <vector>
#include <cstring>

#include "BTreePageTests.h"


CPPUNIT_TEST_SUITE_REGISTRATION(BTreePageTests);

#define BINARY_SIZE 4*12
#define MEM_EQUAL 0


BTreePageTests::BTreePageTests() { }


BTreePageTests::~BTreePageTests() { }


void BTreePageTests::setUp() {
	int size = BINARY_SIZE / sizeof(int);
	binarySource = (char*)(new int[size] {
		0, 3,
		
		1,
		2, 3,
		4,
		5, 6,
		7,
		8, 9,
		10
	});
	
	btreePage = new BTreePage(3, binarySource);
	btreePageReplc = new BTreePage(3, binarySource);
	btreePageFull = new BTreePage(3, binarySource);
	btreePageInsrt = new BTreePage(3, binarySource);
	
	PageRecord record(5, 8);
	btreePageRoot = new BTreePage(3, record);
}


void BTreePageTests::tearDown() {
	delete btreePage;
	delete btreePageReplc;
	delete btreePageFull;
	delete btreePageRoot;
	delete btreePageInsrt;
	delete[] binarySource;
}


void BTreePageTests::testPropertiesFromBinary() {
	int parentPage = btreePageRoot->getParentPageNumber();
	bool isFull = btreePage->isFull();
	int firstChildPage = btreePage->getFirstChildPage();
	
	CPPUNIT_ASSERT_EQUAL(NO_PARENT_PAGE, parentPage);
	CPPUNIT_ASSERT_EQUAL(false, isFull);
	CPPUNIT_ASSERT_EQUAL(1, firstChildPage);
}


void BTreePageTests::testRecordsFromBinary() {
	std::vector<PageRecord> records = btreePage->getRecords();
	
	CPPUNIT_ASSERT_EQUAL(3, (int)records.size());
	CPPUNIT_ASSERT_EQUAL(2, records[0].getKey());
	CPPUNIT_ASSERT_EQUAL(3, records[0].getAddress());
	CPPUNIT_ASSERT_EQUAL(4, records[0].getRightChildPage());
	CPPUNIT_ASSERT_EQUAL(5, records[1].getKey());
	CPPUNIT_ASSERT_EQUAL(6, records[1].getAddress());
	CPPUNIT_ASSERT_EQUAL(7, records[1].getRightChildPage());
	CPPUNIT_ASSERT_EQUAL(8, records[2].getKey());
	CPPUNIT_ASSERT_EQUAL(9, records[2].getAddress());
	CPPUNIT_ASSERT_EQUAL(10, records[2].getRightChildPage());
}


void BTreePageTests::testDeserializingToBinary() {
	char binary[BINARY_SIZE];
	
	btreePage->fillBinary(binary);
	int result = memcmp(binary, binarySource, BINARY_SIZE);
	
	CPPUNIT_ASSERT_EQUAL(MEM_EQUAL, result);
}


void BTreePageTests::testRootPageCreation() {
	int binaryDesired[] {
		NO_PARENT_PAGE, 1,
		NO_CHILD_PAGE, 5, 8
	};
	char binary[sizeof(binaryDesired)];
	
	btreePageRoot->fillBinary(binary);
	int result = memcmp(binary, binaryDesired, sizeof(binaryDesired));
	bool isFull = btreePageRoot->isFull();
	int parentPage = btreePageRoot->getParentPageNumber();
	
	CPPUNIT_ASSERT_EQUAL(false, isFull);
	CPPUNIT_ASSERT_EQUAL(NO_PARENT_PAGE, parentPage);
	CPPUNIT_ASSERT_EQUAL(MEM_EQUAL, result);
}


void BTreePageTests::testFullyUsedSpace() {
	CPPUNIT_ASSERT(btreePageFull->isFull() == false);
	
	btreePageFull->insert(PageRecord(11, 12));
	CPPUNIT_ASSERT(btreePageFull->isFull() == false);
	
	btreePageFull->insert(PageRecord(14, 15));
	CPPUNIT_ASSERT(btreePageFull->isFull() == false);
	
	btreePageFull->insert(PageRecord(17, 18));
	CPPUNIT_ASSERT(btreePageFull->isFull());
	
	btreePageFull->insert(PageRecord(20, 21));
	CPPUNIT_ASSERT_THROW(
		btreePageFull->isFull(),
		std::runtime_error);
	
}


void BTreePageTests::testRecordInsertion() {
	std::vector<PageRecord> records;
	PageRecord record(6, 9);
	
	btreePageRoot->insert(record);
	records = btreePageRoot->getRecords();

	CPPUNIT_ASSERT_EQUAL(2, (int)records.size());
	CPPUNIT_ASSERT_EQUAL(5, records[0].getKey());
	CPPUNIT_ASSERT_EQUAL(8, records[0].getAddress());
}


void BTreePageTests::testRecordInsertionInside() {
	std::vector<PageRecord> records;
	PageRecord record(3, 20, 123);
	
	btreePageInsrt->insert(record);
	records = btreePageInsrt->getRecords();

	CPPUNIT_ASSERT_EQUAL(4, (int)records.size());
	CPPUNIT_ASSERT_EQUAL(2, records[0].getKey());
	CPPUNIT_ASSERT_EQUAL(3, records[0].getAddress());
	CPPUNIT_ASSERT_EQUAL(3, records[1].getKey());
	CPPUNIT_ASSERT_EQUAL(20, records[1].getAddress());
	CPPUNIT_ASSERT_EQUAL(5, records[2].getKey());
	CPPUNIT_ASSERT_EQUAL(6, records[2].getAddress());
}


void BTreePageTests::testFindingKey() {
	int r1 = btreePage->find(2);
	int r2 = btreePage->find(5);
	int r3 = btreePage->find(8);
	int r4 = btreePage->find(1);
	int r5 = btreePage->find(4);
	int r6 = btreePage->find(6);
	int r7 = btreePage->find(20);	
	
	CPPUNIT_ASSERT_EQUAL(BTREEPAGE_FOUND_KEY, r1);
	CPPUNIT_ASSERT_EQUAL(BTREEPAGE_FOUND_KEY, r2);
	CPPUNIT_ASSERT_EQUAL(BTREEPAGE_FOUND_KEY, r3);
	CPPUNIT_ASSERT_EQUAL(1,  r4);
	CPPUNIT_ASSERT_EQUAL(4,  r5);
	CPPUNIT_ASSERT_EQUAL(7,  r6);
	CPPUNIT_ASSERT_EQUAL(10, r7);
}


void BTreePageTests::testFindingKeyInLeaf() {
	int r1 = btreePageRoot->find(2);
	int r2 = btreePageRoot->find(5);
	int r3 = btreePageRoot->find(50);
	
	CPPUNIT_ASSERT_EQUAL(BTREEPAGE_FOUND_NOTHING, r1);
	CPPUNIT_ASSERT_EQUAL(BTREEPAGE_FOUND_KEY, r2);
	CPPUNIT_ASSERT_EQUAL(BTREEPAGE_FOUND_NOTHING, r3);
}


void BTreePageTests::testGettingNeighbors() {
	auto v1 = btreePage->getNeighborsOf(1);
	auto v2 = btreePage->getNeighborsOf(4);
	auto v3 = btreePage->getNeighborsOf(7);
	auto v4 = btreePage->getNeighborsOf(10);
	auto v5 = btreePage->getNeighborsOf(5);
	
	CPPUNIT_ASSERT_EQUAL(4,  v1[0]);
	CPPUNIT_ASSERT_EQUAL(1,  v2[0]);
	CPPUNIT_ASSERT_EQUAL(7,  v2[1]);
	CPPUNIT_ASSERT_EQUAL(4,  v3[0]);
	CPPUNIT_ASSERT_EQUAL(10, v3[1]);
	CPPUNIT_ASSERT_EQUAL(7,  v4[0]);
	CPPUNIT_ASSERT_EQUAL(0, (int)v5.size());
}


void BTreePageTests::testGettingRecordsByNeighbors() {
	auto r11 = btreePage->getRecordBetween(1, 4);
	auto r12 = btreePage->getRecordBetween(4, 1);
	auto r21 = btreePage->getRecordBetween(4, 7);
	auto r22 = btreePage->getRecordBetween(7, 4);
	auto r31 = btreePage->getRecordBetween(7, 10);
	auto r32 = btreePage->getRecordBetween(10, 7);
	
	CPPUNIT_ASSERT_EQUAL(2, r11.getKey());
	CPPUNIT_ASSERT_EQUAL(2, r12.getKey());
	CPPUNIT_ASSERT_EQUAL(5, r21.getKey());
	CPPUNIT_ASSERT_EQUAL(5, r22.getKey());
	CPPUNIT_ASSERT_EQUAL(8, r31.getKey());
	CPPUNIT_ASSERT_EQUAL(8, r32.getKey());
	CPPUNIT_ASSERT_THROW(
		btreePage->getRecordBetween(1, 0),
		std::runtime_error);
	CPPUNIT_ASSERT_THROW(
		btreePage->getRecordBetween(1, 7),
		std::runtime_error);
	CPPUNIT_ASSERT_THROW(
		btreePage->getRecordBetween(7, 1),
		std::runtime_error);
	CPPUNIT_ASSERT_THROW(
		btreePage->getRecordBetween(4, 10),
		std::runtime_error);
	CPPUNIT_ASSERT_THROW(
		btreePageRoot->getRecordBetween(1, 2),
		std::runtime_error);
}


void BTreePageTests::testReplacingRecord() {
	std::vector<PageRecord> records;
	
	PageRecord record(11, 12);
	btreePageReplc->replaceRecord(2, record);
	
	records = btreePageReplc->getRecords();
	
	CPPUNIT_ASSERT_EQUAL(11, records[0].getKey());
	CPPUNIT_ASSERT_EQUAL(12, records[0].getAddress());
	
	CPPUNIT_ASSERT_THROW(
		btreePageReplc->replaceRecord(123, record),
		std::runtime_error);
}