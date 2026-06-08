#include <iostream>
#include <ctime>
#include <fstream>
using namespace std;

// this is the physics model
// will ingest vehicle.xml to figure out aerodynamic components like wings and engines and stuff
// then 6DOF stuff will follow

int main() {
	clock_t before = clock();
	cout << "Hello. physics.cpp is running!\n";

	clock_t duration = clock() - before;
	cout << "\nDuration: " << (float)duration * 1000 / CLOCKS_PER_SEC << " ms";
	return 0;
}

