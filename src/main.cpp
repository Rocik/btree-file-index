#include <cstdlib>
#include <iostream>

#include "btree.h"

using namespace std;


static bool running = true;


static void printDiskStats() {
	int reads = DiskFile::getStatReads();
	int writes = DiskFile::getStatWrites();
	char const* plurarReads = reads == 1 ? "" : "s";
	char const* plurarWrites = writes == 1 ? "" : "s";
	
	printf("[FILES] %d read%s and %d write%s\n",
		 reads, plurarReads,
		 writes, plurarWrites);
	
	DiskFile::resetStats();
}


static void loadCommands(istream& stream, BTree& btree);

static void loadTestFile(const char* filename, BTree& btree) {
	fstream stream(filename, ios_base::in);
	loadCommands(stream, btree);
}


static void loadCommands(istream& stream, BTree& btree) {	
	int key, value;
	string str;
	
	if (!running)
		return;
	
	char c;
	do {
		stream >> c;
		
		if (stream.eof())
			return;
		
		switch (c) {
		// Read more commands from file
		// <filename:string>
		case 'f': {
			stream >> str;
			loadTestFile(str.c_str(), btree);
			break;
		}
		// Inserting record
		// <key:int> <value:int>
		case 'w': {
			stream >> key;
			stream >> value;
			
			if (key <= 0) {
				printf("[INSERT] Key %d is incorrect, must be a Natural number", key);
				break;
			}
			
			if (value <= 0) {
				printf("[INSERT] Value %d is incorrect, must be a Natural number", value);
				break;
			}
			
			Record record(key, value);
			if (btree.insert(record))
				printf("[INSERT] Key %d with value %d\n", key, value);
			else
				printf("[INSERT] Key %d could not be inserted, it exists\n", key);
			
			break;
		}
		// Reading record
		// <key:int>
		case 'o': {
			stream >> key;
			
			if (key <= 0) {
				printf("[READ] Key %d is incorrect, must be a Natural number", key);
				break;
			}
			
			shared_ptr<Record> record;
			if (btree.read(key, &record))
				printf("[READ] Found key %d with value %d\n",
					 key, record->getValue());
			else
				printf("[READ] Key %d was not found\n", key);
			
			break;
		}
		// Updating record
		// <key:int> <value:int>
		case 'a': {
			stream >> key;
			stream >> value;
			
			if (key <= 0) {
				printf("[UPDATE] Key %d is incorrect, must be a Natural number", key);
				break;
			}
			
			if (value <= 0) {
				printf("[UPDATE] Value %d is incorrect, must be a Natural number", value);
				break;
			}
			
			Record record(key, value);
			if (btree.update(record))
				printf("[UPDATE] Key %d with value %d\n", key, value);
			else
				printf("[UPDATE] Key %d could not be updated, not found\n", key);
			
			break;
		}
		// Deleting record
		// <key:int>
		case 'u': {
			stream >> key;
			
			if (key <= 0) {
				printf("[DELETE] Key %d is incorrect, must be a Natural number", key);
				break;
			}
			
			if (btree.remove(key))
				printf("[DELETE] Key %d\n", key);
			else
				printf("[DELETE] Key %d could not be deleted, not found\n", key);
			
			break;
		}
		// Viewing whole file and index sorted by key value
		case 'p': {
			btree.printInKeyOrder();
			
			break;
		}
		// Do not save to files and remove any
		case 'd': {
			btree.dispose();
			continue;
		}
		case 'q':
			running = false;
			return;
		default:
			printf("[ERROR] Unknown command: %c (%d)\n", c, c);
			break;
		}
		
		if (stream.eof())
			return;
		
		printDiskStats();
		
	} while (running && c != 0);
}


int main() {
	string name = string("sample");
	
	BTree tree(name, 3);
	//loadTestFile("test_commands.txt", tree);
	loadCommands(cin, tree);
	
	return 0;
}

