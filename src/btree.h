#ifndef BTREE_H
#define BTREE_H

#include "diskFile.h"
#include "btreePage.h"
#include "dataProxy.h"
#include "record.h"

using namespace std;

#define BTREE_HEADER_FILE_EXTENSION ".hdr"
#define BTREE_FILE_EXTENSION ".btr"

#define BTREE_IS_EMPTY -1


struct BTreeHeader {
	int degree; // minimal amount of records per page
	int rootPageNumber;
	int totalPages;
};


// page number == page address

class BTree {
public:
	BTree(string name, int degree = 3);
	~BTree();
	void create(char bytes[], int degree);
	void dispose();
	
	int getPageSizeInBytes();
	
	bool find(int key, PageRecord** record);
	bool read(int key, shared_ptr<Record>* outRecord);
	bool insert(Record record);
	void printInKeyOrder();
	bool update(Record record);
	bool remove(int key);
	
	void flushToDisk();
	
private:
	void load(char bytes[]);
	void savePage(shared_ptr<BTreePage> page, int pageAddress);
	PageRecord makeLeafPageRecord(Record record);
	shared_ptr<BTreePage> loadPage(int number);
	void setCachedPage(int h, shared_ptr<BTreePage> page);
	void swapCachedPageOfLevel(int h, int pageNumber);
	bool isSpaceOnCurrentPage();
	
	void print(int h, int pageNumber, int level = 0);
	void printShort(int h, int pageNumber, int level = 0);
	void printRawData();
	
	bool compensate(PageRecord* newRecord = nullptr);
	// Return parent page id
	int split(PageRecord* newRecord);
	int merge();
	int getFreePageNumber();
	
	int setRoot(int pageNumber); 
	
	bool disposed;
	
	unique_ptr<DiskFile> file;
	unique_ptr<char[]> pageBytes;
	
	BTreeHeader header;
	unique_ptr<DataProxy> dataProxy;
	// There's always only one page per level
	vector<shared_ptr<BTreePage>> cachedPages;
	vector<int> cachedPagesNumbers;
	int currentPageId;
	
	string headerFilename;
	string treeFilename;
};

#endif /* BTREE_H */

