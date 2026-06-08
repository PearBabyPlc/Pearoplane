#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <ctime>
#include "constants.h"
#include "formulaegg.h"
#include "lookup.h"
using namespace std;

// aerodynamics model
// will do all the stuff like solve for conditions behind shocks, engines and stuff
// physics.cpp is gonna handle stuff like the 6DOF model, this will interface later 

int main() {
	cout << "Hello. aero.cpp is running!\n";
	cout << "Begin benchmark \n\n";
	clock_t before = clock();
	string benchmark;

	for (int i = 0; i < 101; i++) {
		int random1 = rand() % 101;
		int random2 = rand() % 101;
		int random3 = rand() % 101;
		float Mtest = 3 + (random1 * 0.1);
		float Atest = radInDeg * (random2 / 3);
		float altTest = 1 + (random3 * 400);
		getISA::ISA isaTest;
		isaTest.alt = altTest;
		getISA::getPTrho(isaTest);
		Station staTest;
		staTest.M = Mtest;
		getISA::addISAtoStation(staTest, isaTest);
		calImp::getGam(staTest);
		calImp::getCp(staTest);
		isen::getTt_fromT(staTest);
		isen::getPt_fromP(staTest);
		float Srad = oblique::radShockAngle(staTest, Atest);
		float Sdeg = degInRad * Srad;
		float Adeg = random2 / 3;
		//string benchAdd = "M=" + Mtest + " Adeg=" + Adeg + " alt=" + altTest + " Sdeg=" + Sdeg + "\n";
		benchmark.append("M+");
		benchmark.append(to_string(Mtest));
		benchmark.append(" Adeg=");
		benchmark.append(to_string(Adeg));
		benchmark.append(" alt=");
		benchmark.append(to_string(altTest));
		benchmark.append(" Sdeg=");
		benchmark.append(to_string(Sdeg));
		benchmark.append("\n");

	}
	cout << benchmark;

	clock_t duration = clock() - before;
	cout << "\n\nDuration: " << (float)duration * 1000 / CLOCKS_PER_SEC << " ms";
	return 0;
}

