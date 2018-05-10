/*
 */

/*
 * File:   BTreeTests.cpp
 * Author: rocik
 *
 * Created on Dec 26, 2017, 10:58:24 AM
 */

#include "BTreeFreshStartTests.h"
#include "btree.h"


CPPUNIT_TEST_SUITE_REGISTRATION(BTreeFreshStartTests);


BTreeFreshStartTests::BTreeFreshStartTests() {
	btree = nullptr;
}


BTreeFreshStartTests::~BTreeFreshStartTests() {
	tearDown();
}


void BTreeFreshStartTests::setUp() { }


void BTreeFreshStartTests::tearDown() {
	if (btree != nullptr) {
		btree->dispose();
		delete btree;
		btree = nullptr;
	}
}


void BTreeFreshStartTests::openFresh(char const* filename) {
	string headerFilename = string(filename) + BTREE_HEADER_FILE_EXTENSION;
	string btreeFilename = string(filename) + BTREE_FILE_EXTENSION;
	string dataFilename = string(filename) + BTREE_DATA_FILE_EXTENSION;
	::remove(headerFilename.c_str());
	::remove(btreeFilename.c_str());
	::remove(dataFilename.c_str());
	btree = new BTree(filename);
}


void BTreeFreshStartTests::testFindingNothingInEmptyTree() {
	openFresh("BTreeFreshStartTests_testFindingNothingInEmptyTree");
	
	bool found1 = btree->find(-1, nullptr);
	bool found2 = btree->find(0, nullptr);
	bool found3 = btree->find(1, nullptr);
	PageRecord* record;
	bool found4 = btree->find(1, &record);
	
	CPPUNIT_ASSERT(!found1);
	CPPUNIT_ASSERT(!found2);
	CPPUNIT_ASSERT(!found3);
	CPPUNIT_ASSERT(!found4);
	
	tearDown();
}


void BTreeFreshStartTests::testReadingRecords() {
	openFresh("BTreeFreshStartTests_testReadingRecords");
	
	int values[31];
	
	for (int i = 0; i < 31; ++i)
		btree->insert( Record(i + 1, 100 + i) );
	
	for (int i = 0; i < 31; ++i) {
		shared_ptr<Record> record;
		btree->read(i + 1, &record);
		values[i] = record->getValue();
	}
	
	for (int i = 0; i < 31; ++i)
		CPPUNIT_ASSERT_EQUAL(100 + i, values[i]);
	
	shared_ptr<Record> record;
	bool result = btree->read(50, &record);
	CPPUNIT_ASSERT_EQUAL(false, result);
	
	tearDown();
}


void BTreeFreshStartTests::testFillingRootWithRecords() {
	openFresh("BTreeFreshStartTests_testFillingRootWithRecords");
	
	Record record1(1, 101);
	Record record2(2, 102);
	Record record3(3, 103);
	Record record4(4, 104);
	
	bool inserted1 = btree->insert(record1);
	bool inserted2 = btree->insert(record2);
	bool inserted3 = btree->insert(record1);
	bool inserted4 = btree->insert(record3);
	bool inserted5 = btree->insert(record4);
	bool inserted6 = btree->insert(record2);
	
	CPPUNIT_ASSERT(inserted1);
	CPPUNIT_ASSERT(inserted2);
	CPPUNIT_ASSERT(!inserted3);
	CPPUNIT_ASSERT(inserted4);
	CPPUNIT_ASSERT(inserted5);
	CPPUNIT_ASSERT(!inserted6);
	
	tearDown();
}


void BTreeFreshStartTests::testSplittingRoot() {
	char const* filename = "BTreeFreshStartTests_testSplittingRoot";
	openFresh(filename);
	
	for (int i = 1; i <= 7; ++i)
		btree->insert( Record(i, 100 + i) );
	btree->flushToDisk();
	
	string treeFilename = string(filename) + BTREE_FILE_EXTENSION;
	auto pageBytes = make_unique<char[]>(btree->getPageSizeInBytes());
	DiskFile file(treeFilename, btree->getPageSizeInBytes());
	file.readPage(0, pageBytes.get());
	BTreePage page0(3, pageBytes.get());
	file.readPage(1, pageBytes.get());
	BTreePage page1(3, pageBytes.get());
	file.readPage(2, pageBytes.get());
	BTreePage page2(3, pageBytes.get());
	
	auto r1 = page0.getRecords();
	auto r2 = page1.getRecords();
	auto r3 = page2.getRecords();
	
	CPPUNIT_ASSERT_EQUAL(1, r1[0].getKey());
	CPPUNIT_ASSERT_EQUAL(2, r1[1].getKey());
	CPPUNIT_ASSERT_EQUAL(3, r1[2].getKey());
	
	CPPUNIT_ASSERT_EQUAL(5, r2[0].getKey());
	CPPUNIT_ASSERT_EQUAL(6, r2[1].getKey());
	CPPUNIT_ASSERT_EQUAL(7, r2[2].getKey());
	
	CPPUNIT_ASSERT_EQUAL(4, r3[0].getKey());
	
	CPPUNIT_ASSERT_EQUAL(0, page2.getFirstChildPage());
	CPPUNIT_ASSERT_EQUAL(1, r3[0].getRightChildPage());
	
	tearDown();
}


void BTreeFreshStartTests::testCompensation() {
	char const* filename = "BTreeFreshStartTests_testCompensation";
	openFresh(filename);
	
	for (int i = 1; i <= 11; ++i)
		btree->insert( Record(i, 100 + i) );
	btree->flushToDisk();
	
	string treeFilename = string(filename) + BTREE_FILE_EXTENSION;
	auto pageBytes = make_unique<char[]>(btree->getPageSizeInBytes());
	DiskFile file(treeFilename, btree->getPageSizeInBytes());
	file.readPage(0, pageBytes.get());
	BTreePage page0(3, pageBytes.get());
	file.readPage(1, pageBytes.get());
	BTreePage page1(3, pageBytes.get());
	file.readPage(2, pageBytes.get());
	BTreePage page2(3, pageBytes.get());
	
	auto r1 = page0.getRecords();
	auto r2 = page1.getRecords();
	auto r3 = page2.getRecords();
	
	CPPUNIT_ASSERT_EQUAL(1, r1[0].getKey());
	CPPUNIT_ASSERT_EQUAL(2, r1[1].getKey());
	CPPUNIT_ASSERT_EQUAL(3, r1[2].getKey());
	CPPUNIT_ASSERT_EQUAL(4, r1[3].getKey());
	CPPUNIT_ASSERT_EQUAL(5, r1[4].getKey());
	
	CPPUNIT_ASSERT_EQUAL(7, r2[0].getKey());
	CPPUNIT_ASSERT_EQUAL(8, r2[1].getKey());
	CPPUNIT_ASSERT_EQUAL(9, r2[2].getKey());
	CPPUNIT_ASSERT_EQUAL(10, r2[3].getKey());
	CPPUNIT_ASSERT_EQUAL(11, r2[4].getKey());
	
	CPPUNIT_ASSERT_EQUAL(6, r3[0].getKey());
	
	CPPUNIT_ASSERT_EQUAL(0, page2.getFirstChildPage());
	CPPUNIT_ASSERT_EQUAL(1, r3[0].getRightChildPage());
	
	tearDown();
}


void BTreeFreshStartTests::testFillingRecordsToGet2RecordsInRoot() {
	char const* filename = "BTreeFreshStartTests_testFillingRecordsToGet2RecordsInRoot";
	openFresh(filename);
	
	for (int i = 1; i <= 14; ++i)
		btree->insert( Record(i, 100 + i) );
	btree->flushToDisk();
	
	string treeFilename = string(filename) + BTREE_FILE_EXTENSION;
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
	
	tearDown();
}


void BTreeFreshStartTests::testAddingRecordRandomOrder() {
	char const* filename = "BTreeFreshStartTests_testAddingRecordRandomOrder";
	openFresh(filename);
	
	int order[] = {
		12, 4,  7,  30, 15,  8,  9, 10,  1,  2,
		 5, 6,  3,  11, 13, 20, 14, 21, 16, 17,
		31, 29, 28, 27, 18, 19, 26, 22, 25, 24,
		23
	};
	
	for (int i = 0; i < 31; ++i)
		btree->insert( Record(order[i], 100 + order[i]) );
	btree->flushToDisk();
	
	string treeFilename = string(filename) + BTREE_FILE_EXTENSION;
	auto pageBytes = make_unique<char[]>(btree->getPageSizeInBytes());
	DiskFile file(treeFilename, btree->getPageSizeInBytes());
	file.readPage(2, pageBytes.get());
	BTreePage pageRoot(3, pageBytes.get());
	
	auto r1 = pageRoot.getRecords();
	
	// TODO: analyze this
	CPPUNIT_ASSERT_EQUAL(7,  r1[0].getKey());
	CPPUNIT_ASSERT_EQUAL(14, r1[1].getKey());
	CPPUNIT_ASSERT_EQUAL(21, r1[2].getKey());
	CPPUNIT_ASSERT_EQUAL(28, r1[3].getKey());
	
	tearDown();
}