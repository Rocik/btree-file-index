#ifndef BTREEPAGE_H
#define BTREEPAGE_H

#include <vector>
#include "pageRecord.h"

using namespace std;

#define NO_PARENT_PAGE -1
#define NO_CHILD_PAGE -1

#define BTREEPAGE_FOUND_KEY -10
#define BTREEPAGE_FOUND_NOTHING -20


class BTreePage {
public:
	// For loading page
	BTreePage(int degree, char* bytes);
	// For root page initialization
	BTreePage(int degree, PageRecord record);
	
	BTreePage(int degree);
	
	bool isFull();
	bool hasEnoughRecords(bool needsOneExtra = false);
	bool isLeaf();
	
	void setParentPageNumber(int pageNumber);
	int getParentPageNumber();
	void setFirstChildPage(int childPage);
	int getFirstChildPage();
	vector<PageRecord> getRecords();
	
	vector<int> getNeighborsOf(int address);
	PageRecord getRecordBetween(int address1, int address2);
	
	void insert(PageRecord record);
	void replaceRecord(int oldKey, PageRecord newRecord);
	void removeRecord(int key);
	void clear();
	
	// Try to find a key or its direction to child page
	int find(int key);
	PageRecord* getLastFoundRecord();
	
	void fillBinary(char* bytes);
	
private:	
	void parseBinaryData(char* bytes);
	int readInt(char*& bytes);
	void setInt(char*& bytes, int value);
	
	int parentPageNumber;
	int lastFoundRecordIndex;
	
	const int degree;
	
	int firstChildPage;
	vector<PageRecord> records;
};

#endif /* BTREEPAGE_H */

