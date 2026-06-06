#include <iostream>
#include <cmath>
#include <string>
#include <ctime>
using namespace std;

// vehicle.cpp set up for now to take parametric inputs and return a .obj file
// well it will be at some point

int main() {
	clock_t before = clock();
	cout << "Hello. vehicle.cpp is running!\n";

	clock_t duration = clock() - before;
	cout << "\nDuration: " << (float)duration * 1000 / CLOCKS_PER_SEC << " ms";
	return 0;
}

