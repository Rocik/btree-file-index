#include <cmath>

#include "record.h"


Record::Record(int key, int address)
	: key(key), value(address) {

}


int Record::getKey() {
	return key;
}


int Record::getValue() {
	return value;
}


long long int Record::sumOfDivisors() {	
    int sum = 0;
 
	for (int i = 2; i <= std::sqrt(value); ++i) {
		if (value % i == 0) {
			if (i == value / i)
				sum += i;
			else
				sum += i + value/i;
		}
	}

    return sum + value + 1;
}