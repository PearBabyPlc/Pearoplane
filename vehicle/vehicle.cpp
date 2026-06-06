#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>
#include <ctime>

using namespace std;
#include "pearoparse.h"
#include "filestuff.h"

// vehicle.cpp set up for now to take parametric inputs and return a .obj file
// well it will be at some point

int main() {
	clock_t before = clock();
	cout << "Hello. vehicle.cpp is running!\n";

	readVehicleFile();
	parseVehicleFile();

	clock_t duration = clock() - before;
	cout << "\nvehicle.cpp concluded.\nDuration: " << (float)duration * 1000 / CLOCKS_PER_SEC << " ms\n";
	return 0;
}

