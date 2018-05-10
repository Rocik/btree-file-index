/*
 */

/*
 * File:   UtilitiesTests.cpp
 * Author: rocik
 *
 * Created on Dec 30, 2017, 1:10:41 PM
 */

#include "UtilitiesTests.h"


CPPUNIT_TEST_SUITE_REGISTRATION(UtilitiesTests);


UtilitiesTests::UtilitiesTests() { }


UtilitiesTests::~UtilitiesTests() { }


void UtilitiesTests::setUp() { }


void UtilitiesTests::tearDown() { }


void UtilitiesTests::testInsertRecordWithOrder_emptyVector() {
	vector<PageRecord> records;
	PageRecord newRecord(123, 456);
	
	Util::insertRecordWithOrder(records, newRecord);
	
	CPPUNIT_ASSERT_EQUAL(1, (int)records.size());
	CPPUNIT_ASSERT_EQUAL(123, records[0].getKey());
	CPPUNIT_ASSERT_EQUAL(456, records[0].getAddress());
}


void UtilitiesTests::testInsertRecordWithOrder_oneInside() {
	vector<PageRecord> records;
	records.push_back(PageRecord(100, 2));
	PageRecord loRecord(12, 1);
	PageRecord hiRecord(123, 3);
	
	Util::insertRecordWithOrder(records, hiRecord);
	
	CPPUNIT_ASSERT_EQUAL(2, (int)records.size());
	CPPUNIT_ASSERT_EQUAL(100, records[0].getKey());
	CPPUNIT_ASSERT_EQUAL(2,   records[0].getAddress());
	CPPUNIT_ASSERT_EQUAL(123, records[1].getKey());
	CPPUNIT_ASSERT_EQUAL(3,   records[1].getAddress());
	
	Util::insertRecordWithOrder(records, loRecord);
	
	CPPUNIT_ASSERT_EQUAL(3, (int)records.size());
	CPPUNIT_ASSERT_EQUAL(12,  records[0].getKey());
	CPPUNIT_ASSERT_EQUAL(1,   records[0].getAddress());
	CPPUNIT_ASSERT_EQUAL(100, records[1].getKey());
	CPPUNIT_ASSERT_EQUAL(2,   records[1].getAddress());
	CPPUNIT_ASSERT_EQUAL(123, records[2].getKey());
	CPPUNIT_ASSERT_EQUAL(3,   records[2].getAddress());
}


void UtilitiesTests::testInsertRecordWithOrder_twoInside() {
	vector<PageRecord> records;
	records.push_back(PageRecord(100, 2, 20));
	records.push_back(PageRecord(200, 4, 40));
	PageRecord loRecord(10, 1, 10);
	PageRecord miRecord(150, 3, 30);
	PageRecord hiRecord(300, 5, 50);
	
	Util::insertRecordWithOrder(records, loRecord);
	Util::insertRecordWithOrder(records, hiRecord);
	Util::insertRecordWithOrder(records, miRecord);
	
	CPPUNIT_ASSERT_EQUAL(1, records[0].getAddress());
	CPPUNIT_ASSERT_EQUAL(2, records[1].getAddress());
	CPPUNIT_ASSERT_EQUAL(3, records[2].getAddress());
	CPPUNIT_ASSERT_EQUAL(4, records[3].getAddress());
	CPPUNIT_ASSERT_EQUAL(5, records[4].getAddress());
	CPPUNIT_ASSERT_EQUAL(10, records[0].getRightChildPage());
	CPPUNIT_ASSERT_EQUAL(20, records[1].getRightChildPage());
	CPPUNIT_ASSERT_EQUAL(30, records[2].getRightChildPage());
	CPPUNIT_ASSERT_EQUAL(40, records[3].getRightChildPage());
	CPPUNIT_ASSERT_EQUAL(50, records[4].getRightChildPage());
}


void UtilitiesTests::testResolvePagesOrder() {
	PageRecord r1(10, 100);
	PageRecord r2(20, 200);
	auto p1 = make_shared<BTreePage>(3, r1);
	auto p2 = make_shared<BTreePage>(3, r2);
	auto pages1 = Util::resolvePagesOrder(p1, p2);
	
	CPPUNIT_ASSERT_EQUAL(p1, pages1[0]);
	CPPUNIT_ASSERT_EQUAL(p2, pages1[1]);
	
	auto pages2 = Util::resolvePagesOrder(p2, p1);
	
	CPPUNIT_ASSERT_EQUAL(p1, pages2[0]);
	CPPUNIT_ASSERT_EQUAL(p2, pages2[1]);
}


void UtilitiesTests::testMergeRecordsInOrder() {
	array<shared_ptr<BTreePage>, 2> pages;
	
	PageRecord r1(10, 100);
	PageRecord r2(20, 200);
	PageRecord r3(30, 300);
	pages[0] = make_shared<BTreePage>(3, r1);
	pages[1] = make_shared<BTreePage>(3, r3);
	
	auto result = Util::mergeRecordsInOrder(pages, r2);
	
	CPPUNIT_ASSERT_EQUAL(3, (int)result.size());
	CPPUNIT_ASSERT_EQUAL(10, result[0].getKey());
	CPPUNIT_ASSERT_EQUAL(20, result[1].getKey());
	CPPUNIT_ASSERT_EQUAL(30, result[2].getKey());
}