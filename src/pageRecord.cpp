#include "pageRecord.h"
#include "btreePage.h"


PageRecord::PageRecord(int key, int address)
	: key(key), address(address) {
	rightChildPage = NO_CHILD_PAGE;
}


PageRecord::PageRecord(int key, int address, int childPage)
	: key(key), address(address) {
	rightChildPage = childPage;
}


int PageRecord::getKey() {
	return key;
}


int PageRecord::getAddress() {
	return address;
}


int PageRecord::getRightChildPage() {
	return rightChildPage;
}