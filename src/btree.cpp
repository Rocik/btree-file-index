#include <memory>
#include <cstring>
#include <cmath>

#include "btree.h"
#include "util.h"


BTree::BTree(string name, int degree) {
	headerFilename = name + BTREE_HEADER_FILE_EXTENSION;
	treeFilename = name + BTREE_FILE_EXTENSION;
	
	unique_ptr<char[]> bytes(new char[sizeof(BTreeHeader)]);
	
	DiskFile headerFile(headerFilename, sizeof(BTreeHeader));
	
	if (headerFile.existed()) {
		headerFile.readPage(0, bytes.get());
		load(bytes.get());
	} else {
		create(bytes.get(), degree);
		headerFile.savePage(0, bytes.get());
	}

	dataProxy = make_unique<DataProxy>(name);
}


BTree::~BTree() {
	if (!disposed)
		flushToDisk();
}


void BTree::create(char bytes[], int degree) {
	header.degree = degree;
	header.rootPageNumber = BTREE_IS_EMPTY;
	header.totalPages = 0;
	
	memcpy(bytes, &header, sizeof(BTreeHeader));
	
	file = make_unique<DiskFile>(treeFilename, getPageSizeInBytes());
	pageBytes = make_unique<char[]>(getPageSizeInBytes());
	
	disposed = false;
}


void BTree::dispose() {
	DiskFile headerFile(headerFilename, sizeof(BTreeHeader));
	headerFile.dispose();
	
	file->dispose();
	dataProxy->dispose();
	
	disposed = true;
}


int BTree::getPageSizeInBytes() {
	int headerSize = 3 * sizeof(int);
	int recordsSize = 2 * header.degree * 3 * sizeof(int);
	return recordsSize + headerSize;
}


bool BTree::find(int key, PageRecord** record) {
	int s = header.rootPageNumber;
	if (s == BTREE_IS_EMPTY)
		return false;
	
	// Also works as height
	currentPageId = 0;
	
	for (;;) {
		swapCachedPageOfLevel(currentPageId, s);

		int newS = cachedPages[currentPageId]->find(key);
		if (newS == BTREEPAGE_FOUND_KEY) {
			if (record != nullptr)
				*record = cachedPages[currentPageId]->getLastFoundRecord();
			return true;
		} else if (newS == BTREEPAGE_FOUND_NOTHING) {
			return false;
		} else {
			s = newS;
			currentPageId++;
		}
	}
}


bool BTree::read(int key, shared_ptr<Record>* outRecord) {
	PageRecord* record;
	if (find(key, &record)) {
		Record dataRecord = dataProxy->read(record->getAddress());
		*outRecord = make_shared<Record>(key, dataRecord.getValue());
		return true;
	}
	
	return false;
}


bool BTree::insert(Record record) {
	if (header.rootPageNumber == BTREE_IS_EMPTY) {
		// Create first initial record
		PageRecord pageRecord = makeLeafPageRecord(record);
		auto newPage = make_shared<BTreePage>(header.degree, pageRecord);
		setCachedPage(0, newPage);
		setRoot(getFreePageNumber());
		// The first page must be saved here
		// otherwise the file object won't be able to write to file
		// in case it was asked to initialize file with page on
		// offset different than 0.
		savePage(newPage, 0);
		return true;
	} else {
		if (find(record.getKey(), nullptr))
			return false; // Already exists
		
		PageRecord pageRecord = makeLeafPageRecord(record);
		
		for (;;) {
			if (isSpaceOnCurrentPage()) {
				cachedPages[currentPageId]->insert(pageRecord);
				return true;
			}

			if (compensate(&pageRecord))
				return true;

			int parentId = split(&pageRecord);
			currentPageId = parentId;
		}
	}
}


void BTree::printInKeyOrder() {
	if (header.rootPageNumber == BTREE_IS_EMPTY)
		printf("TREE IS EMPTY\n");
	else {
		printf("RECORDS FILE\n");
		printRawData();
		printf("TREE AND RECORDS IN ORDER\n");
		print(0, header.rootPageNumber);
		printf("\nBTREE ONLY\n");
		printShort(0, header.rootPageNumber);
	}
}


bool BTree::update(Record record) {
	PageRecord* pageRecord;
	if (find(record.getKey(), &pageRecord)) {
		int address = pageRecord->getAddress();
		
		return dataProxy->update(address, record);
	}
	
	return false;
}


bool BTree::remove(int key) {
	PageRecord* foundRecord;
	if (!find(key, &foundRecord))
		return false; // Already exists

	int keyToRemove;
	shared_ptr<BTreePage> leaf;

	if (cachedPages[currentPageId]->isLeaf()) {
		keyToRemove = key;
		leaf = cachedPages[currentPageId];
	} else {
		// Find smallest key from right subtree
		int rightChild = foundRecord->getRightChildPage();
		swapCachedPageOfLevel(currentPageId + 1, rightChild);
		// Now go all the way left
		int i = 1;
		for(;;) {
			leaf = cachedPages[currentPageId + i];
			if (leaf->isLeaf())
				break;
			int leftChild = leaf->getFirstChildPage();
			swapCachedPageOfLevel(currentPageId + ++i, leftChild);
		}
		// Replace record to remove with a substitute
		PageRecord subPage = leaf->getRecords()[0];
		keyToRemove = subPage.getKey();

		PageRecord newRecord = PageRecord(subPage.getKey(),
										  subPage.getAddress(),
										  foundRecord->getRightChildPage());
		cachedPages[currentPageId]->replaceRecord(key, newRecord);
		currentPageId += i;
	}

	leaf->removeRecord(keyToRemove);
	if (leaf->hasEnoughRecords())
		return true;
	
	for (;;) {
		if (currentPageId == 0) {
			// The only page left is root
			// If root is empty remove it
			if (cachedPages[0]->getRecords().empty()) {
				cachedPages.erase(cachedPages.begin());
				cachedPagesNumbers.erase(cachedPagesNumbers.begin());
				if (cachedPagesNumbers.empty())
					header.rootPageNumber = BTREE_IS_EMPTY;
				else
					header.rootPageNumber = cachedPagesNumbers[0];
			}
			
			return true;
		}
		
		if (compensate())
			return true;

		int parentId = merge();
		if (parentId >= 0)
			currentPageId = parentId;
		else
			return true;
	}
}


void BTree::flushToDisk() {	
	for (int i = 0; i < cachedPages.size(); ++i) {
		savePage(cachedPages[i], cachedPagesNumbers[i]);
	}
	
	// Flush header
	unique_ptr<char[]> bytes(new char[sizeof(BTreeHeader)]);
	memcpy(bytes.get(), &header, sizeof(BTreeHeader));
	
	DiskFile headerFile(headerFilename, sizeof(BTreeHeader));
	headerFile.savePage(0, bytes.get());
	
	dataProxy->flush();
}


void BTree::load(char bytes[]) {
	memcpy(&header, bytes, sizeof(BTreeHeader));
	
	file = make_unique<DiskFile>(treeFilename, getPageSizeInBytes());
	pageBytes = make_unique<char[]>(getPageSizeInBytes());
	
	if (header.rootPageNumber != BTREE_IS_EMPTY) {
		auto rootPage = loadPage(header.rootPageNumber);
		setCachedPage(0, rootPage);
		cachedPagesNumbers.push_back(header.rootPageNumber);
	}
}


void BTree::savePage(shared_ptr<BTreePage> page, int pageAddress) {
	page->fillBinary(pageBytes.get());
	file->savePage(pageAddress, pageBytes.get());
}


PageRecord BTree::makeLeafPageRecord(Record record) {
	int address = dataProxy->create(record);
	PageRecord pageRecord(record.getKey(), address);
	return pageRecord;
}


shared_ptr<BTreePage> BTree::loadPage(int number) {
	file->readPage(number, pageBytes.get());
	return make_shared<BTreePage>(header.degree, pageBytes.get());
}


void BTree::setCachedPage(int h, shared_ptr<BTreePage> page) {
	cachedPages.push_back(page);
	currentPageId = h;
}


void BTree::swapCachedPageOfLevel(int h, int address) {
	// Make sure there is space for the new page
	if (cachedPages.size() <= h) {
		cachedPages.reserve(h + 1);
		cachedPagesNumbers.reserve(h + 1);
		for (int i = cachedPages.size(); i <= h; ++i) {
			cachedPages.push_back(nullptr);
			cachedPagesNumbers.push_back(-1);
		}
	}
	
	// Make sure there is need to swap
	if (address == cachedPagesNumbers[h])
		return;
	
	shared_ptr<BTreePage> page = loadPage(address);
	
	if (cachedPages[h] != nullptr)
		savePage(cachedPages[h], cachedPagesNumbers[h]);
	
	cachedPages[h] = page;
	cachedPagesNumbers[h] = address;
}


bool BTree::isSpaceOnCurrentPage() {
	return cachedPages[currentPageId]->isFull() == false;
}


void BTree::print(int h, int pageNumber, int level) {
	swapCachedPageOfLevel(h, pageNumber);

	string indent(level * 3, ' ');
	printf("%s", indent.c_str());

	printf("Page #%d:\n", pageNumber);

	int firstChild = cachedPages[h]->getFirstChildPage();
	if (firstChild != NO_CHILD_PAGE)
		print(h + 1, firstChild, level + 1);
	
	auto records = cachedPages[h]->getRecords();
	for (PageRecord& record : records) {
		int key = record.getKey();
		int address = record.getAddress();
		int rightChild = record.getRightChildPage();
		
		Record dataRecord = dataProxy->read(address);
		
		int sideIndent = 1 + max(3 - (int)log10(key), 0);
		
		printf("%s", indent.c_str());
		printf("   Key - %d", key);
		printf("%*c  ", sideIndent, ' ');
		printf("Record - %d (%d)\n", dataRecord.getValue(), address);
		
		if (rightChild != NO_CHILD_PAGE)
			print(h + 1, rightChild, level + 1);
	}
}


void BTree::printShort(int h, int pageNumber, int level) {
	swapCachedPageOfLevel(h, pageNumber);

	string indent(level * 3, ' ');
	printf("%s", indent.c_str());

	printf("Page #%d: [", pageNumber);

	int firstChild = cachedPages[h]->getFirstChildPage();
	if (firstChild != NO_CHILD_PAGE)
		printf("%d", firstChild);
	printf("]");
	
	auto records = cachedPages[h]->getRecords();
	for (PageRecord& record : records) {
		int key = record.getKey();
		
		printf(" %3d [", key);
		
		int rightChild = record.getRightChildPage();
		if (rightChild != NO_CHILD_PAGE)
			printf("%d", rightChild);
		printf("]");
	}
	
	printf("\n");
	
	if (firstChild != NO_CHILD_PAGE)
		printShort(h + 1, firstChild, level + 1);
	
	for (PageRecord& record : records) {
		int rightChild = record.getRightChildPage();
		if (rightChild != NO_CHILD_PAGE)
			printShort(h + 1, rightChild, level + 1);
	}
}


void BTree::printRawData() {
	int recordsCount = dataProxy->getTotalAllocatedRecords();
	for (int i = 0; i < recordsCount; ++i) {
		Record record = dataProxy->read(i);
		if (record.getKey() == -1)
			printf("[  empty  ] ");
		else
			printf("[%3d - %3d] ", record.getKey(), record.getValue());
		
		if (i % 10 == 9)
				printf("\n");
	}
	printf("\n");
}


bool BTree::compensate(PageRecord* newRecord) {
	if (currentPageId == 0)
		return false;

	auto currentPage = cachedPages[currentPageId];
	auto parentPage  = cachedPages[currentPageId - 1];
	
	int currentAddress = cachedPagesNumbers[currentPageId];
	auto siblings = parentPage->getNeighborsOf(currentAddress);
	for (int sibling : siblings) {
		shared_ptr<BTreePage> siblingPage = loadPage(sibling);
		
		if (newRecord != nullptr) {
			// Insert
			if (siblingPage->isFull())
				continue;
		} else {
			// Remove
			if (!siblingPage->hasEnoughRecords(true))
				continue;
		}
			
		// Gather all records
		auto pages = Util::resolvePagesOrder(currentPage, siblingPage);

		auto parentRecord = parentPage->getRecordBetween(
											currentAddress, sibling);
		auto allRecords = Util::mergeRecordsInOrder(pages, parentRecord);
		if (newRecord != nullptr)
			Util::insertRecordWithOrder(allRecords, *newRecord);

		// Redistribute records
		pages[0]->clear();
		pages[1]->clear();

		int mid = allRecords.size() / 2;
		// Fix parent right child address
		int parentRightChildPage = parentRecord.getRightChildPage();
		int childsRightChildPage = allRecords[mid].getRightChildPage();
		allRecords[mid] = PageRecord(allRecords[mid].getKey(),
									 allRecords[mid].getAddress(),
									 parentRightChildPage);
		PageRecord correctedOldParentRecord(parentRecord.getKey(),
											parentRecord.getAddress(),
											pages[1]->getFirstChildPage());
		pages[1]->setFirstChildPage(childsRightChildPage);

		for (int i = 0; i < mid; ++i) {
			if (allRecords[i].getKey() == correctedOldParentRecord.getKey())
				allRecords[i] = correctedOldParentRecord;
			pages[0]->insert(allRecords[i]);
		}
		parentPage->replaceRecord(parentRecord.getKey(), allRecords[mid]);
		for (int i = mid + 1; i < allRecords.size(); ++i) {
			if (allRecords[i].getKey() == correctedOldParentRecord.getKey())
				allRecords[i] = correctedOldParentRecord;
			pages[1]->insert(allRecords[i]);
		}

		// Save sibling page to file
		savePage(siblingPage, sibling);

		return true;
	}
	
	return false;
}


int BTree::split(PageRecord* newRecord) {	
	auto currentPage = cachedPages[currentPageId];
	
	vector<PageRecord> records(currentPage->getRecords());
	Util::insertRecordWithOrder(records, *newRecord);
	
	currentPage->clear();
	
	auto newPage = make_shared<BTreePage>(header.degree);
	int newPageAddress = getFreePageNumber();
	
	shared_ptr<BTreePage> parentPage;
	if (currentPageId == 0) {
		int currentPageAddress = header.rootPageNumber;
		// Splitting root page
		parentPage = make_shared<BTreePage>(header.degree);
		parentPage->setFirstChildPage(currentPageAddress);
		parentPage->setParentPageNumber(NO_PARENT_PAGE);
		
		int parentPageAddress = getFreePageNumber();
		
		cachedPages.insert(cachedPages.begin(), parentPage);
		cachedPagesNumbers.insert(cachedPagesNumbers.begin(),
								  parentPageAddress);
		setRoot(parentPageAddress);
		currentPage->setParentPageNumber(parentPageAddress);
	} else {		
		parentPage = cachedPages[currentPageId - 1];
	}
	
	int mid = records.size() / 2;
	
	newPage->setFirstChildPage(records[mid].getRightChildPage());
	newPage->setParentPageNumber(header.rootPageNumber);
	
	for (int i = 0; i < mid; ++i)
		currentPage->insert(records[i]);
	for (int i = mid + 1; i < records.size(); ++i)
		newPage->insert(records[i]);
	
	savePage(newPage, newPageAddress);
	
	// Return median record
	*newRecord = PageRecord(records[mid].getKey(),
						    records[mid].getAddress(),
						    newPageAddress);
	
	return max(0, currentPageId - 1);
}


int BTree::merge() {
	auto currentPage = cachedPages[currentPageId];
	auto parentPage  = cachedPages[currentPageId - 1];
	
	int currentAddress = cachedPagesNumbers[currentPageId];
	int sibling = parentPage->getNeighborsOf(currentAddress)[0];
	auto parentRecord = parentPage->getRecordBetween(currentAddress, sibling);
	
	auto siblingPage = loadPage(sibling);
	auto pages = Util::resolvePagesOrder(currentPage, siblingPage);
	parentRecord = PageRecord(parentRecord.getKey(),
							  parentRecord.getAddress(),
							  pages[0]->getFirstChildPage());
	
	auto allRecords = Util::mergeRecordsInOrder(pages, parentRecord);
	
	pages[0]->clear();
	
	for (int i = 0; i < allRecords.size(); ++i)
		pages[0]->insert(allRecords[i]);
	parentPage->removeRecord(parentRecord.getKey());
	
	savePage(siblingPage, sibling);
	if (siblingPage->getRecords()[0].getKey() == allRecords[0].getKey())
		swapCachedPageOfLevel(currentPageId, sibling);
	
	if (parentPage->hasEnoughRecords())
		return -1;
	
	return max(0, currentPageId - 1);
}


int BTree::getFreePageNumber() {
	int address = header.totalPages;
	header.totalPages++;
	return address;
}


int BTree::setRoot(int pageNumber) {
	header.rootPageNumber = pageNumber;
	if (cachedPagesNumbers.empty())
		cachedPagesNumbers.push_back(header.rootPageNumber);
	else
		cachedPagesNumbers[0] = header.rootPageNumber;
}