#include <cstring>

#include "dataProxy.h"
#include "diskFile.h"


DataProxy::DataProxy(string name)
	: pageSizeInBytes(512), recordSizeInBytes(8),
	  recordsPerPage(pageSizeInBytes / recordSizeInBytes) {
	string filename = name + BTREE_DATA_FILE_EXTENSION;
	file = make_unique<DiskFile>(filename, pageSizeInBytes);
	page = make_unique<char[]>(pageSizeInBytes);
	
	pageNumber = 0;
	
	bool hasContent = file->getPagesCount() > 0;
	if (hasContent)
		file->readPage(pageNumber, page.get());
	else
		memset(page.get(), -1, pageSizeInBytes);
}


void DataProxy::dispose() {
	file->dispose();
}


void DataProxy::flush() {
	file->savePage(pageNumber, page.get());
}


Record DataProxy::read(int recordNumber) {
	int bytesOffset = recordNumber * recordSizeInBytes;
	setActivePage(bytesOffset);
	
	int recordNumberOnPage = bytesOffset % pageSizeInBytes;
	char* recordBytes = &page[recordNumberOnPage];
	int key = *reinterpret_cast<int*>(recordBytes);
	int value = *reinterpret_cast<int*>(recordBytes + 4);
	return Record(key, value);
}


bool DataProxy::update(int recordNumber, Record record) {
	if (record.getValue() <= 0)
		return false;
	
	int bytesOffset = recordNumber * recordSizeInBytes;
	setActivePage(bytesOffset);
	
	int recordNumberOnPage = bytesOffset % pageSizeInBytes;
	char* recordBytes = &page[recordNumberOnPage];
	int* key = reinterpret_cast<int*>(recordBytes);
	int* value = reinterpret_cast<int*>(recordBytes + 4);
	*key = record.getKey();
	*value = record.getValue();
	
	return true;
}


int DataProxy::create(Record record) {
	if (record.getValue() <= 0)
		return -1;
	
	int lastPageNumber = file->getPagesCount() * pageSizeInBytes;
	setActivePage(lastPageNumber);
	
	int recordNumberOnPage = findFreeSpaceOnCurrentPage();
	
	if (recordNumberOnPage >= recordsPerPage) {
		// No space on current page
		setNewPage();
		recordNumberOnPage = 0;
	}
	
	char* recordBytes = &page[recordNumberOnPage * recordSizeInBytes];
	int* key = reinterpret_cast<int*>(recordBytes);
	int* value = reinterpret_cast<int*>(recordBytes + 4);
	*key = record.getKey();
	*value = record.getValue();
	
	return pageNumber * recordsPerPage + recordNumberOnPage;
}


int DataProxy::getTotalAllocatedRecords() {
	int pages = file->getPagesCount() + 1;
	return pages * recordsPerPage;
}


void DataProxy::setActivePage(int bytesOffset) {
	if (bytesOffset < 0)
		return;
	
	int newPageNumber = bytesOffset / pageSizeInBytes;
	if (newPageNumber != pageNumber) {
		file->savePage(pageNumber, page.get());
		pageNumber = newPageNumber;
		file->readPage(pageNumber, page.get());
	}
}


void DataProxy::setNewPage() {
	file->savePage(pageNumber, page.get());
	memset(page.get(), -1, pageSizeInBytes);
	pageNumber = file->getPagesCount();
}


int DataProxy::findFreeSpaceOnCurrentPage() {
	int recordNumber = 0;
	
	while (recordNumber < recordsPerPage) {
		char* recordBytes = &page[recordNumber * recordSizeInBytes];
		int* record = reinterpret_cast<int*>(recordBytes);
		
		if (*record < 0)
			break;
		
		recordNumber++;
	}
	
	return recordNumber;
}