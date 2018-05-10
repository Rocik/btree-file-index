#ifndef RECORD_H
#define RECORD_H

class Record {
public:
	Record(int key, int address);
	
	int getKey();
	int getValue();
	
	long long int sumOfDivisors();
	
private:
	int key;
	int value;
};

#endif /* RECORD_H */

