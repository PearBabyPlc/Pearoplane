#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <ctime>
#include "formulaegg.h"
#include "lookup.h"
using namespace std;

// aerodynamics model
// will do all the stuff like solve for conditions behind shocks, engines and stuff
// physics.cpp is gonna handle stuff like the 6DOF model, this will interface later 

int main() {
	cout << "Hello. aero.cpp is running!\n";
	cout << "Begin benchmark \n\n";
	string benchmark = "\n";
	// benchmarking normal shock relations

	Station staTest;
	clock_t before = clock();
	for (int i = 1; i < 21; i++) {
		float Mtest = (i * 1) + 1;
		staTest.pos = i;
		staTest.M = Mtest;
		staTest.gam = 1.4;
		isen::getPt_fromP(staTest);
		isen::getTt_fromT(staTest);
		isen::getVelocity(staTest);
		float pPt = staTest.Pt;
		float pTt = staTest.Tt;
		normal::updateStationNormal(staTest);
//		benchmark.append("pos=");
//		benchmark.append(to_string(staTest.pos));
//		benchmark.append(" pM=");
//		benchmark.append(to_string(Mtest));
//		benchmark.append(" M=");
//		benchmark.append(to_string(staTest.M));
//		benchmark.append(" Pr=");
//		benchmark.append(to_string(staTest.P / 101325));
//		benchmark.append(" Tr=");
//		benchmark.append(to_string(staTest.T / 288.15));
//		benchmark.append(" rhoR=");
//		benchmark.append(to_string(staTest.rho / 1.225));
//		benchmark.append(" Ptr=");
//		benchmark.append(to_string(staTest.Pt / pPt));
//		benchmark.append(" Ttr=");
//		benchmark.append(to_string(staTest.Tt / pTt));
//		benchmark.append("\n");
	}
	clock_t duration = clock() - before;
	cout << benchmark;
	
	cout << "\n\nDuration: " << (float)duration * 1000 / CLOCKS_PER_SEC << " ms";
	return 0;
}

