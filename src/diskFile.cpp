#include <iostream>
#include <memory>
#include <cassert>
#include <cstring>

#include "diskFile.h"


int DiskFile::statRead = 0;
int DiskFile::statWrite = 0;
	

DiskFile::DiskFile(string filename, int pageSize)
	: filename(filename), pageSizeInBytes(pageSize) {
	binaryInOut  = fstream::in | fstream::out | fstream::binary;
	binaryAppend = fstream::out | fstream::binary | ifstream::app;
	
	ifstream infile(filename);
	newlyCreated = !infile.good();
		
	resetStats();
}


bool DiskFile::existed() {
	return !newlyCreated;
}


bool DiskFile::readPage(int pageNumber, char* page) {
	unique_ptr<ifstream>
	file = make_unique<ifstream>(filename, ifstream::binary);
	file->seekg(pageNumber * pageSizeInBytes, ios::beg);
	file->read(page, pageSizeInBytes);
	file->close();
	DiskFile::statRead++;
	
	return true;
}


void DiskFile::savePage(int pageNumber, const char* page) {
	unique_ptr<fstream> file;
	
	// This is retarded design of C++ streams:
	// If you want to replace bytes you HAVE to open in fstream binaryInOut
	// but it will not create new file and when you append data the file is erased
	// When you use binaryAppend your data will ALWAYS be written at the end
	
	// This will still not work when skipping some pages at the creation of file
	
	int totalPages = getPagesCount();
	if (pageNumber == totalPages)
		file = make_unique<fstream>(filename, binaryAppend);
	else {
		file = make_unique<fstream>(filename, binaryInOut);
		file->seekp(pageNumber * pageSizeInBytes, ios::beg);
	}
		
	file->write(page, pageSizeInBytes);
	file->flush();
	file->close();
	
	DiskFile::statWrite++;
}


int DiskFile::getPagesCount() {
	ifstream in(filename, ifstream::binary | ifstream::ate);
    return in.tellg() / pageSizeInBytes;
}


void DiskFile::dispose() {
	::remove(filename.c_str());
}


int DiskFile::getStatReads() {
	return DiskFile::statRead;
}


int DiskFile::getStatWrites() {
	return DiskFile::statWrite;
}


void DiskFile::resetStats() {
	DiskFile::statRead = 0;
	DiskFile::statWrite = 0;
}