#ifndef FILE_H
#define FILE_H

#include <memory>
#include <fstream>

using namespace std;
 

class DiskFile {
public:
	DiskFile(string filename, int pageSize);
	bool existed();
	
	bool readPage(int pageNumber, char* page);
	void savePage(int pageNumber, const char* page);
	void dispose();
	
	int getPagesCount();
	
	static int getStatReads();
	static int getStatWrites();
	static void resetStats();	
private:
	bool newlyCreated;
	
	fstream::openmode binaryInOut;
	fstream::openmode binaryAppend;
	
	const string filename;
	const int pageSizeInBytes;
	
	static int statRead;
	static int statWrite;
};

#endif /* FILE_H */

