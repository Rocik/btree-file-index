#include <stdexcept>
#include "btreePage.h"
#include "util.h"


BTreePage::BTreePage(int degree, char* bytes)
	: BTreePage(degree) {
	parseBinaryData(bytes);
}


BTreePage::BTreePage(int degree, PageRecord record)
	: BTreePage(degree) {
	parentPageNumber = NO_PARENT_PAGE;
	records.push_back(record);
}


BTreePage::BTreePage(int degree)
	: degree(degree) {
	setFirstChildPage(NO_CHILD_PAGE);
	records.reserve(2 * degree);
	lastFoundRecordIndex = -1;
}


bool BTreePage::isFull() {
	if (records.size() > 2 * degree)
		throw runtime_error("There is more records than allowed.");
	return records.size() == 2 * degree;
}


bool BTreePage::hasEnoughRecords(bool needsOneExtra) {
	if (needsOneExtra)
		return records.size() > degree;
	else
		return records.size() >= degree;
}


bool BTreePage::isLeaf() {
	return firstChildPage == NO_CHILD_PAGE;
}


void BTreePage::setParentPageNumber(int address) {
	parentPageNumber = address;
}


int BTreePage::getParentPageNumber() {
	return parentPageNumber;	
}


void BTreePage::setFirstChildPage(int childPage) {
	firstChildPage = childPage;
}


int BTreePage::getFirstChildPage() {
	return firstChildPage;
}


vector<PageRecord> BTreePage::getRecords() {
	return records;
}


vector<int> BTreePage::getNeighborsOf(int address) {
	vector<int> neighbors;
	
	for (int i = 0; i < records.size(); ++i) {
		if (getFirstChildPage() == NO_CHILD_PAGE)
			break;
		
		if (getFirstChildPage() == address) {
			neighbors.push_back(records[0].getRightChildPage());
			
			break;
		}
		
		if (records[i].getRightChildPage() == address) {
			if (i == 0)
				neighbors.push_back(getFirstChildPage());
			else
				neighbors.push_back(records[i - 1].getRightChildPage());
			if (i != records.size() - 1)
				neighbors.push_back(records[i + 1].getRightChildPage());
			break;
		}
	}
	
	return neighbors;
}


PageRecord BTreePage::getRecordBetween(int address1, int address2) {
	if (getFirstChildPage() == NO_CHILD_PAGE)
		throw runtime_error("Cannot get record by addresses on a leaf.");
		
	if (getFirstChildPage() == address1) {
		if (records[0].getRightChildPage() == address2)
			return records[0];
	}

	if (getFirstChildPage() == address2) {
		if (records[0].getRightChildPage() == address1)
			return records[0];
	}
	
	if (records[0].getRightChildPage() == address1) {
		if (getFirstChildPage() == address2)
			return records[0];
	}

	if (records[0].getRightChildPage() == address2) {
		if (getFirstChildPage() == address1)
			return records[0];
	}
	
	for (int i = 0; i < records.size() - 1; ++i) {		
		if (records[i].getRightChildPage() == address1) {
			if (records[i + 1].getRightChildPage() == address2)
				return records[i + 1];
			break;
		}
		
		if (records[i].getRightChildPage() == address2) {
			if (records[i + 1].getRightChildPage() == address1)
				return records[i + 1];
			break;
		}
	}
	
	throw runtime_error("Given addresses are not neighbors.");
}


void BTreePage::insert(PageRecord record) {
	Util::insertRecordWithOrder(records, record);
}


void BTreePage::replaceRecord(int oldKey, PageRecord newRecord) {
	if (find(oldKey) == BTREEPAGE_FOUND_KEY) {
		records[lastFoundRecordIndex] = newRecord;
		return;
	}
	
	throw runtime_error("Given key cannot be replaced because it was not found.");
}


void BTreePage::removeRecord(int key) {
	for (int i = 0; i < records.size(); ++i) {
		if (records[i].getKey() == key) {
			records.erase(records.begin() + i);
			break;
		}
	}
}


void BTreePage::clear() {
	records.clear();
}


int BTreePage::find(int key) {
	lastFoundRecordIndex = -1;
	
	int n = records.size();
	
	int i = 0;
    while(i < n) {
        int mid = (i + n) / 2;
		
        if (key == records[mid].getKey()
		&& (mid == 0 || records[mid-1].getKey() < key)) {
			lastFoundRecordIndex = mid;
            return BTREEPAGE_FOUND_KEY;
		} if (key <= records[mid].getKey())
             n = mid;
        else
             i = mid + 1;
    };
	
	if (getFirstChildPage() != NO_CHILD_PAGE) {
		if (i >= records.size())
			return records.back().getRightChildPage();

		if (key < records[i].getKey()) {
			if (i == 0)
				return getFirstChildPage();
			else
				return records[i - 1].getRightChildPage();
		}

		if (key > records[i].getKey())
			return records[i].getRightChildPage();
	}
	
    return BTREEPAGE_FOUND_NOTHING;
}


PageRecord* BTreePage::getLastFoundRecord() {
	return &records[lastFoundRecordIndex];
}


void BTreePage::fillBinary(char* bytes) {
	setInt(bytes, parentPageNumber);
	setInt(bytes, records.size());
	
	setInt(bytes, getFirstChildPage());
	
	for (int i = 0; i < records.size(); i++) {
		setInt(bytes, records[i].getKey());
		setInt(bytes, records[i].getAddress());
		setInt(bytes, records[i].getRightChildPage());
	}
}


void BTreePage::parseBinaryData(char* bytes) {
	parentPageNumber = readInt(bytes);
	int recordsAmount = readInt(bytes);
	
	int firstChildPage = readInt(bytes);
	setFirstChildPage(firstChildPage);
	
	for (int i = 0; i < recordsAmount; i++) {
		int key = readInt(bytes);
		int address = readInt(bytes);
		int childPage = readInt(bytes);
		PageRecord record = PageRecord(key, address, childPage);
		records.push_back(record);
	}
}


int BTreePage::readInt(char*& bytes) {
	int value = *reinterpret_cast<int*>(bytes);
	bytes += sizeof(int);
	return value;
}


void BTreePage::setInt(char*& bytes, int value) {
	*reinterpret_cast<int*>(bytes) = value;
	bytes += sizeof(int);
}