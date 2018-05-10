/*
 */

/*
 * File:   PageProxyTests.cpp
 * Author: rocik
 *
 * Created on Dec 26, 2017, 6:03:47 PM
 */

#include "DataProxyTests.h"


CPPUNIT_TEST_SUITE_REGISTRATION(DataProxyTests);


DataProxyTests::DataProxyTests() {
	// Make sure this file does not exists
	dataProxy = new DataProxy("DataProxyTests");
	tearDown();
}


DataProxyTests::~DataProxyTests() { }


void DataProxyTests::setUp() { }


void DataProxyTests::tearDown() {
	if (dataProxy != nullptr) {
		dataProxy->dispose();
		delete dataProxy;
		dataProxy = nullptr;
	}
}


void DataProxyTests::testAddingRecords() {
	dataProxy = new DataProxy("DataProxyTests");
	
	int offset1 = dataProxy->create(Record(123, 123));
	int offset2 = dataProxy->create(Record(234, 234));
	int offset3 = dataProxy->create(Record(345, 345));
	int offset4 = dataProxy->create(Record(1111, 111));
	
	CPPUNIT_ASSERT_EQUAL(0, offset1);
	CPPUNIT_ASSERT_EQUAL(1, offset2);
	CPPUNIT_ASSERT_EQUAL(2, offset3);
	CPPUNIT_ASSERT_EQUAL(3, offset4);
	
	tearDown();
}


void DataProxyTests::testAddingMoreRecordsThanThePageHolds() {
	dataProxy = new DataProxy("DataProxyTests");
	
	int lastOffset;
	for (int i = 0; i < 500; ++i) {
		Record record(i, i + 1);
		lastOffset = dataProxy->create(record);
		CPPUNIT_ASSERT_EQUAL(i, lastOffset);
	}
	
	tearDown();
}


void DataProxyTests::testReadingRecordsRandomOrder() {	
	dataProxy = new DataProxy("DataProxyTests");
	
	for (int i = 0; i < 500; ++i) {
		Record record(i, i + 1);
		dataProxy->create(record);
	}
	
	int v1 = dataProxy->read(52).getValue();
	int v2 = dataProxy->read(351).getValue();
	int v3 = dataProxy->read(7).getValue();
	int v4 = dataProxy->read(12).getValue();
	int v5 = dataProxy->read(488).getValue();
	
	CPPUNIT_ASSERT_EQUAL(53,  v1);
	CPPUNIT_ASSERT_EQUAL(352, v2);
	CPPUNIT_ASSERT_EQUAL(8,   v3);
	CPPUNIT_ASSERT_EQUAL(13,  v4);
	CPPUNIT_ASSERT_EQUAL(489, v5);
	
	tearDown();
}


void DataProxyTests::testUpdatingRecords() {
	const int testValue = 123456;
	
	dataProxy = new DataProxy("DataProxyTests");
	
	for (int i = 0; i < 500; ++i) {
		Record record(i, i + 1);
		dataProxy->create(record);
	}
	
	Record record(1, testValue);
	dataProxy->update(52, record);
	dataProxy->update(351, record);
	dataProxy->update(125, record);
	
	int v1 = dataProxy->read(52).getValue();
	int v2 = dataProxy->read(351).getValue();
	int v3 = dataProxy->read(125).getValue();
	
	CPPUNIT_ASSERT_EQUAL(testValue, v1);
	CPPUNIT_ASSERT_EQUAL(testValue, v2);
	CPPUNIT_ASSERT_EQUAL(testValue, v3);
	
	tearDown();
}
