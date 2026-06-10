#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <ctime>

using namespace std;
#include "formulaegg.h"
#include "lookup.h"
#include "solvers.h"

// aerodynamics model
// will do all the stuff like solve for conditions behind shocks, engines and stuff
// physics.cpp is gonna handle stuff like the 6DOF model, this will interface later 

int main() {
	cout << "Hello. aero.cpp is running!\n";
	cout << "Begin benchmark \n";
	clock_t before = clock();
	// benchmarking start
	
	Station stat;
	string stations;
	getAmbientStation(stat, 35000, 2000);
	printStation(stat, stations);
	Shock cock;
	cock.defDeg = 10.0;
	while (stat.M > 1.0) {
		solveShock(stat, cock);
		printShock(cock, stations);
		printStation(stat, stations);
	}

	cout << stations;
		
	// benchmarking end
	clock_t duration = clock() - before;
	
	cout << "\nDuration: " << (float)duration * 1000 / CLOCKS_PER_SEC << " ms";
	return 0;
}

