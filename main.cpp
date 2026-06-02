#include <iostream>
#include <cmath>
#include "lookupTables.h"
using namespace std;

int main() {
	cout << "Hello nerd. \n";
	cout << "==============================\n";

	struct getISA::ISA isaTemp;
	for (int i = 0; i < 51; i++) {
		isaTemp.alt = i * 1000;
		getISA::getPTrho(isaTemp);
		cout << "ISA size: " << sizeof(isaTemp) << " bytes\n";
		getISA::readISA(isaTemp);
	}

	cout << "==============================\n";
	cout << "Goodbye nerd.";	
	return 0;
}

