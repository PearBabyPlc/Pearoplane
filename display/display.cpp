#include <iostream>
#include <ctime>
using namespace std;

// this is probably gonna be the starting point for the entire main program
// in the meantime display.cpp will just be a testbed for user input and 3d rendering of the aircraft.obj file
// physics.cpp will be integrated in as a header file or something
// i want this flight sim to be as simple as possible and to consolidate memory usage as much as possible
// this may end up being impractical with the 3d rendering openGL 4.1 stuff though
// so these two segments of the sim will be kept separate until it's time to put it all together

int main() {
	clock_t before = clock();
	cout << "Hello. display.cpp is running!\n";

	clock_t duration = clock() - before;
	cout << "\nDuration: " << (float)duration * 1000 / CLOCKS_PER_SEC << " ms";
	return 0;
}

