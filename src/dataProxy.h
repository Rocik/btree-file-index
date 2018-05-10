#ifndef DATAPROXY_H
#define DATAPROXY_H

#include <string>
#include <memory>

#include "diskFile.h"
#include "record.h"

using namespace std;

#define BTREE_DATA_FILE_EXTENSION ".dat"


class DataProxy {
public:
	DataProxy(string name);
	void dispose();
	
	void flush();
	
	Record read(int recordNumber);
	bool update(int recordNumber, Record record);
	// Returns recordNumber
	int create(Record record);
	
	int getTotalAllocatedRecords();
	
private:
	void setActivePage(int bytesOffset);
	void setNewPage();
	int findFreeSpaceOnCurrentPage();
	
	unique_ptr<DiskFile> file;
	unique_ptr<char[]> page;
	
	int pageNumber;
	
	const int pageSizeInBytes;
	const int recordSizeInBytes;
	const int recordsPerPage;
};

#endif /* DATAPROXY_H */

