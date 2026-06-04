#include <iostream>
#include <cmath>
#include <string>
#include <ctime>
#include "constants.h"
#include "formulaegg.h"
#include "lookup.h"
using namespace std;

int main() {
	clock_t before = clock();

	Station sta1;
	sta1.T = 1000;
	sta1.M = 3;
	calImp::getGam(sta1);
	calImp::getCp(sta1);
	sta1.Pt = 600000;
	isen::getTt_fromT(sta1);
	isen::getP_fromPt(sta1);
	cout << "M = " << sta1.M << "\nP = " << sta1.P << "\nT = " << sta1.T << 
		"\ngam = " << sta1.gam << "\n Cp = " << sta1.Cp << 
		"\nPt = " << sta1.Pt << "\nTt = " << sta1.Tt;	

	clock_t duration = clock() - before;
	cout << "\nDuration: " << (float)duration * 1000 / CLOCKS_PER_SEC << " ms";
	return 0;
}

