#include "util.h"


void Util::insertRecordWithOrder(vector<PageRecord>& records,
								  PageRecord record) {
	for (int i = 0; i < records.size(); ++i) {
		if (record.getKey() < records[i].getKey()) {
			records.insert(records.begin() + i, record);
			return;
		}
	}
	
	records.push_back(record);
}


array<shared_ptr<BTreePage>, 2> Util::resolvePagesOrder(
											shared_ptr<BTreePage> page1,
											shared_ptr<BTreePage> page2) {
	array<shared_ptr<BTreePage>, 2> pages;
	
	auto page1Records = page1->getRecords();
	auto page2Records = page2->getRecords();
	int page1FirstKey = page1Records[0].getKey();
	int page2FirstKey = page2Records[0].getKey();
	
	if (page1FirstKey < page2FirstKey) {
		pages[0] = page1;
		pages[1] = page2;
	} else {
		pages[0] = page2;
		pages[1] = page1;
	}
	
	return pages;
}


vector<PageRecord> Util::mergeRecordsInOrder(
									array<shared_ptr<BTreePage>, 2> pages,
									PageRecord middleRecord) {
	vector<PageRecord> records;
	//records.reserve(4 * header.degree);
	
	vector<PageRecord> records1 = pages[0]->getRecords();
	vector<PageRecord> records2 = pages[1]->getRecords();
	
	records.insert(records.end(),
				   records1.begin(),
				   records1.end());
	records.push_back(middleRecord);
	records.insert(records.end(),
				   records2.begin(),
				   records2.end());
	
	return records;
}