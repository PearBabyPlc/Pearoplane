#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <ctime>

using namespace std;
#include "formulaegg.h"
#include "lookup.h"
#include "solvers.h"
#include "benchmarks.h"

// aerodynamics model
// will do all the stuff like solve for conditions behind shocks, engines and stuff
// physics.cpp is gonna handle stuff like the 6DOF model, this will interface later 
//
// TODO: structs for engine components, more functions for them, perhaps a models.h file

int main() {
	cout << "Hello. aero.cpp is running!\n";
	cout << "==========Begin benchmark==========";
	clock_t before = clock();
	
	// benchmarking start
	string stations;
	float altitude = 30000.0;
	Station sta1;
	float Ainout = 20.0;
	sta1.A = Ainout;
	sta1.V = 2000.0;
	getAmbientStation(sta1, altitude);
	float Pambient = sta1.P;
	printStation(sta1, stations);

	float spikeAngleDeg = 10.0;
	supersonicIntake(sta1, spikeAngleDeg);
	printStation(sta1, stations);

	float Adiffuser = 2.5;
	subsonicDiffuser(sta1, Adiffuser);
	printStation(sta1, stations);

	float hydrogenConsumption = 0.0;
	float chamberTemp = 3200.0;
	subsonicCombustion(sta1, hydrogenConsumption, chamberTemp);
	stations.append("\nH2 consumed = ");
	stations.append(to_string(hydrogenConsumption));
	stations.append("kg/s\n");
	printStation(sta1, stations);

	convergingThroat(sta1);
	printStation(sta1, stations);

	supersonicEjector(sta1, Ainout, Pambient);
	printStation(sta1, stations);

	// benchmarking end
	clock_t duration = clock() - before;
	cout << stations;
	cout << "==========End benchmark==========";
	
	cout << "\nDuration: " << (float)duration * 1000 / CLOCKS_PER_SEC << " ms";
	return 0;
}

