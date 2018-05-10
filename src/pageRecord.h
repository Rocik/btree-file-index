#ifndef PAGERECORD_H
#define PAGERECORD_H

class PageRecord {
public:
	PageRecord(int key, int address);
	PageRecord(int key, int address, int childPage);
	
	int getKey();
	int getAddress();
	int getRightChildPage();
	
private:
	int key;
	int address;
	
	int rightChildPage;
};

#endif /* PAGERECORD_H */

