#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <array>
#include <memory>

#include "pageRecord.h"
#include "btreePage.h"

using namespace std;


class Util {
public:
	static void 
	insertRecordWithOrder(
			vector<PageRecord>& records,
			PageRecord record);
	
	static array<shared_ptr<BTreePage>, 2>
	resolvePagesOrder(
			shared_ptr<BTreePage> page1,
			shared_ptr<BTreePage> page2);
	
	static vector<PageRecord>
	mergeRecordsInOrder(
			array<shared_ptr<BTreePage>, 2> pages,
			PageRecord middleRecord);
	
private:
	Util() {};
};

#endif /* UTIL_H */

