#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <ctime>
#include <vector>
#include <map>
using namespace std;
#include "formulaegg.h"
#include "lookup.h"
#include "solvers.h"
#include "models.h"

// aerodynamics model
// will do all the stuff like solve for conditions behind shocks, engines and stuff
// physics.cpp is gonna handle stuff like the 6DOF model, this will interface later 
//
// TODO: structs for engine components, more functions for them, perhaps a models.h file

int main() {
	cout << "Hello. aero.cpp is running!\n";
	cout << "\n===Begin benchmark===\n";
	clock_t before = clock();
	
	// benchmarking start
	models::Turbojet testJet;
	string printing;
	models::turbojet(testJet, printing);
	// benchmarking end

	clock_t duration = clock() - before;
	cout << "\n===End benchmark===\n";
	cout << "\nDuration: " << (float)duration * 1000 / CLOCKS_PER_SEC << " ms";
	return 0;
}

