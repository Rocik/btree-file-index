/*
 */

/*
 * File:   PageProxyTests.h
 * Author: rocik
 *
 * Created on Dec 26, 2017, 6:03:47 PM
 */

#ifndef DATAPROXYTESTS_H
#define DATAPROXYTESTS_H

#include <cppunit/extensions/HelperMacros.h>

#include "dataProxy.h"


class DataProxyTests : public CPPUNIT_NS::TestFixture {
	CPPUNIT_TEST_SUITE(DataProxyTests);

	CPPUNIT_TEST(testAddingRecords);
	CPPUNIT_TEST(testAddingMoreRecordsThanThePageHolds);
	CPPUNIT_TEST(testReadingRecordsRandomOrder);
	CPPUNIT_TEST(testUpdatingRecords);

	CPPUNIT_TEST_SUITE_END();

public:
	DataProxyTests();
	virtual ~DataProxyTests();
	void setUp();
	void tearDown();

private:
	void testAddingRecords();
	void testAddingMoreRecordsThanThePageHolds();
	void testReadingRecordsRandomOrder();
	void testUpdatingRecords();
	
	DataProxy* dataProxy;
};

#endif /* DATAPROXYTESTS_H */

