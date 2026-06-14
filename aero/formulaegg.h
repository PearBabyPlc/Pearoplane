// TABLE OF CONTENTS:
// constants, physics stuff, unit conversion
// fundamental structs such as Stations (air) and Shocks (types, angles)
// calorically imperfect specific heat capacities
// isentropic relations and the ideal gas law
// normal shock relations
// oblique shock relations
// Rayleigh and Fanno flow

// for complete solvers for shockwaves/combustion/etc see solvers.h

// CONSTANTS //
const float g = 9.80665;
const float rEarth = 6356766;
const float Runiv = 8314.46;
const float molAir = 28.9644;
const float RsAir = Runiv / molAir;
const double e = exp(1.0);
const double pi = 3.14159265359;
const float Theta = 3055 + (5 / 9);
const float gamDiatomic = 1.4;
const float CpiAir = 1005;
const float CviAir = CpiAir / gamDiatomic;
const double degInRad = 180 / pi;
const double radInDeg = pi / 180;

// UNIT CONVERSION //
// probably mostly unused but good to have as a reference
void degToRad(float &angle) {
	angle *= radInDeg;
}
void radToDeg(float &angle) {
	angle *= degInRad;
}

// FUNDAMENTAL STRUCTS AND ENUMS //
// required for basically all the formulaegg/solvers void functions to work
struct Station {
	int pos = 0;
	float M = 0;
	float P = 101325;
	float T = 288.15;
	float rho = 1.225;
	float gam = 1.4;
	float Cp = 1005;
	float Pt = 101325;
	float Tt = 288.15;
	float V = 0;
	float A = 0;
	float mdot = 0;
};

void getA_fromMdot(Station &sta) {
	sta.A = sta.mdot / (sta.rho * sta.V);
}

void getMdot_fromA(Station &sta) {
	sta.mdot = sta.rho * sta.V * sta.A;
}

enum ShockType {WEAK, STRONG, NORMAL, DEBUG};

// M decreases, P/T/rho increases, Pt decreases
// (Tt should theoretically remain constant but I'm including some losses)
struct Shock {
	int pos = 0;
	float defDeg = 0.0;
	float shockDeg = 0.0;
	enum ShockType type = DEBUG;
};

// M increases, total conditions constant (isentropic)
struct ExpansionFan {
	int pos = 0;
	float defDeg = 0.0;
	float degMu1 = 0.0;
	float degMu2 = 0.0;
};

// CALORICALLY IMPERFECT SPECIFIC HEAT EQUATIONS //
// for now all this stuff is based on ideal diatomic gas (gam=1.4)
// will work well for air, but not for water vapour/carbon dioxide
// heavy overhaul required at some point
namespace calImp {
	void getGam(Station &sta) {
		double ThetaT = Theta / sta.T;	
		double ThetaTexpsub1 = exp(ThetaT) - 1;
		double ThetaTexpsub1sq = pow(ThetaTexpsub1, 2);
		double ThetaTsq = pow(ThetaT, 2);
		sta.gam = 1 + ((gamDiatomic - 1) / (1 + (gamDiatomic - 1) * (ThetaTsq * (exp(ThetaT) / ThetaTexpsub1sq))));
	}
	void getCp(Station &sta) {
		double ThetaT = Theta / sta.T;
		double ThetaTexpsub1 = exp(ThetaT) - 1;
		double ThetaTexpsub1sq = pow(ThetaTexpsub1, 2);
		double ThetaTsq = pow(ThetaT, 2);
		sta.Cp = CpiAir * (1 + ((gamDiatomic - 1) / gamDiatomic) * (ThetaTsq * (exp(ThetaT) / ThetaTexpsub1sq)));
	}
//	float getCv(Station &sta) {
//		double ThetaT = Theta / sta.T;
//		double ThetaTexpsub1 = exp(ThetaT - 1);
//		double ThetaTexpsub1sq = pow(ThetaTexpsub1, 2);
//		double ThetaTsq = pow(ThetaT, 2);
//		return CviAir * (1 + (gamDiatomic - 1) *  (ThetaTsq * (exp(ThetaT) / ThetaTexpsub1sq)));
//	}
}

// ISENTROPIC RELATIONS //
// ideal isentropic relations plus ideal gas law to find rho
namespace isen {
	float getTt_T(float &gam, float &M) {
		return 1 + ((gam - 1) / 2) * pow(M, 2);
	}
	void getTt_fromT(Station &sta) {
		sta.Tt = sta.T * (1 + ((sta.gam - 1) / 2) * pow(sta.M, 2));
	}
	void getPt_fromP(Station &sta) {
		double Ptow1 = 1 + ((sta.gam - 1) / 2) * pow(sta.M, 2);
		double Ptow2 = sta.gam / (sta.gam - 1);
		sta.Pt = sta.P * pow(Ptow1, Ptow2);
	}
	void getT_fromTt(Station &sta) {
		sta.T = sta.Tt / (1 + ((sta.gam - 1) / 2) * pow(sta.M, 2));
	}
	void getP_fromPt(Station &sta) {
		double Ptow1 = 1 + ((sta.gam - 1) / 2) * pow(sta.M, 2);
		double Ptow2 = sta.gam / (sta.gam - 1);
		sta.P = sta.Pt / pow(Ptow1, Ptow2);
	}
	float getA_Astar(Station &sta) {
		double a = (sta.gam + 1) / (2 * (sta.gam - 1));
		double b = (sta.gam + 1) / 2;
		double c = 1 + ((sta.gam - 1) / 2) * pow(sta.M, 2);
		return pow(b, -a) * (pow(c, a) / sta.M);
	}
	void getVelocity(Station &sta) {
		sta.V = sta.M * sqrt(RsAir * sta.gam * sta.T);
	}
	void getMach(Station &sta) {
		sta.M = sta.V / sqrt(RsAir * sta.gam * sta.T);
	}
	// this will require updating at the same time as calImp
	// can't assume the station is constantly diatomic only with combustion products
	void getRho_fromPT(Station &sta) {
		sta.rho = sta.P / (RsAir * sta.T);
	}
	float getDynamicP(Station &sta) {
		return 0.5 * sta.rho * pow(sta.V, 2);
	}
}

// NORMAL SHOCK RELATIONS //
namespace normal {
	void updateStationNormal(Station &sta) {
		float pM = sta.M;
		double aM = (sta.gam - 1) * pow(pM, 2) + 2;
		double bM = 2 * sta.gam * pow(pM, 2) - (sta.gam - 1);
		// it seems like I could probably optimise memory here:
		// initialise as few doubles as possible at the start of functions as needed
		// then use them as kinda virtual registers by updating and accessing them with pointers, and deleting once finished
		// this doesn't seem like it'd work though, since stack memory is defined by scope
		// might be a better idea to divide all this stuff up into inner functions or whatever, returning isn't that inefficient, i'm not duplicating very much data for long
		// this is definitely something to do after trying to collect as many like terms as possible across all the necessary equations, rather than with the current unoptimised versions of them basically copied directly from formulaegg.py
		sta.M = sqrt(aM / bM);
		//float Pr = (2 * sta.gam * pow(pM, 2) - (sta.gam - 1)) / (sta.gam + 1);
		sta.P *= (2 * sta.gam * pow(pM, 2) - (sta.gam - 1)) / (sta.gam + 1);
		float cT = sta.gam + 1; // no need for a double here
		//float Tr = ((2 * sta.gam * pow(pM, 2) - (sta.gam - 1)) * ((sta.gam - 1) * pow(pM, 2) + 2)) / (pow(cT, 2) * pow(pM, 2));
		sta.T *= ((2 * sta.gam * pow(pM, 2) - (sta.gam - 1)) * ((sta.gam - 1) * pow(pM, 2) + 2)) / (pow(cT, 2) * pow(pM, 2));
		//float rhoR = ((sta.gam + 1) * pow(pM, 2)) / ((sta.gam - 1) * pow(pM, 2) + 2);
		sta.rho *= ((sta.gam + 1) * pow(pM, 2)) / ((sta.gam - 1) * pow(pM, 2) + 2);
		double aPt = ((sta.gam + 1) * pow(pM, 2)) / ((sta.gam - 1) * pow(pM, 2) + 2);
		double bPt = (sta.gam + 1) / (2 * sta.gam * pow(pM, 2) - (sta.gam - 1));;
		double cPt = sta.gam / (sta.gam - 1);
		double dPt = 1 / (sta.gam - 1);
		//float Ptr = pow(aPt, cPt) * pow(bPt, dPt);
		sta.Pt *= pow(aPt, cPt) * pow(bPt, dPt);
		//std::cout << "\npM=" << pM << "  M=" << sta.M << " Pr=" << Pr << " Tr=" << Tr << " rhoR=" << rhoR << " Ptr=" << Ptr << " gam=" << sta.gam;
		// just fuckin optimise later
		calImp::getGam(sta);
		isen::getTt_fromT(sta);
		calImp::getCp(sta);
		isen::getVelocity(sta);
	}
}

// OBLIQUE SHOCK RELATIONS //
// Mach angle, deflection from shock angle, maximum deflection before strong shock
// basic shock from deflection angle solver (update with more efficient search algo one day
// void function to update Station in a sensible order (apply oblique shock relations, find new gamma/Cp, find new total temperature
namespace oblique {
	// all these shock angle things are in radians since trig
	// in the solver necessary unit conversions to degrees for the Shock struct update void func will be included
	float radMachAngle(Station &sta) {
		return asin(1 / sta.M);
	}
	float radDeflectionAngle(Station &sta, float S) {
		double sinS = sin(S);
		double invCota = 1 / (tan(S) * ((((sta.gam + 1) * pow(sta.M, 2) / (2 * (pow(sta.M, 2) * pow(sinS, 2) - 1))) - 1)));
		return atan(invCota);
	}
	float maxDefRad(Station &sta) {
		double onePointFive = pow(sta.M, 2) - 1;
		return (4 / (3 * sqrt(3) * (sta.gam + 1))) * (pow(onePointFive, 1.5) / pow(sta.M, 2));
	}
	float radShockAngle(Station &sta, float A) {
		int minAngleDeg = floor(degInRad * radMachAngle(sta));
		float Adeg = degInRad * A;
		float wipA;
		float &wipAdeg = wipA;
		float iNext;
		float &iTarget = iNext;
		for (int i = minAngleDeg; i < 91; i++) {
			float degi = radInDeg * i;
			float tempi = radDeflectionAngle(sta, degi);
			wipAdeg = degInRad * tempi;
			iTarget = i;
			//std::cout << "\n s=" << i << " a=" << wipAdeg << " targ=" << Adeg << " diff=" << abs(Adeg - wipAdeg);
			if (abs(Adeg - wipAdeg) <= 1.0) {
				break;
			}
		}
		float Sdeg;
		float &wipS = Sdeg;
		for (int i = 0; i < 2001; i++) {
			wipS = iNext + (0.0005 * i);
			float degi = radInDeg * wipS;
			float tempi = radDeflectionAngle(sta, degi);
			wipAdeg = degInRad * tempi;
			//std::cout << "\n s=" << wipS << " a=" << wipAdeg << " targ=" << Adeg << " diff=" << abs(Adeg - wipAdeg);
			if (abs(Adeg - wipAdeg) <= 0.0005) {
				break;
			}
		}
		degToRad(wipS);
		return wipS;
	}
	// don't forget the conversions when calling this in the solver updater
	void updateStationOblique(Station &sta, float S, float A) {
		double sinS = sin(S);
		double sinSsubA = sin(S - A);
		float pM = sta.M;
		double bM = (sta.gam - 1) * pow(pM, 2) * pow(sinS, 2) + 2;
		double cM = 2 * sta.gam * pow(pM, 2) * pow(sinS, 2) - (sta.gam - 1);
		sta.M = sqrt((bM / cM) / pow(sinSsubA, 2));

		sta.P *= (2 * sta.gam * pow(pM, 2) * pow(sinS, 2) - (sta.gam - 1)) / (sta.gam + 1);
		double gamPlusOne = sta.gam + 1;
		sta.T *= ((2 * sta.gam * pow(pM, 2) * pow(sinS, 2) - (sta.gam - 1)) * ((sta.gam - 1) * pow(pM, 2) * pow(sinS, 2) + 2)) / (pow(gamPlusOne, 2) * pow(pM, 2) * pow(sinS, 2));
		sta.rho *= ((sta.gam + 1) * pow(pM, 2) * pow(sinS, 2)) / ((sta.gam - 1) * pow(pM, 2) * pow(sinS, 2) + 2);
		double aPt = ((sta.gam + 1) * pow(pM, 2) * pow(sinS, 2)) / ((sta.gam - 1) * pow(pM, 2) * pow(sinS, 2) + 2);
		double bPt = sta.gam / (sta.gam - 1);
		double cPt = (sta.gam + 1) / (2 * sta.gam * pow(pM, 2) * pow(sinS, 2) - (sta.gam - 1));
		double dPt = 1 / (sta.gam - 1);
		sta.Pt *= pow(aPt, bPt) * pow(cPt, dPt);
		calImp::getGam(sta);
		isen::getTt_fromT(sta);
		calImp::getCp(sta);
		// check to make sure velocity update conserves mass
		// or realistically yet pessimistically decreases it
		// might account for necessary boundary layer bleed/losses
		isen::getVelocity(sta);
	}
}

// PRANDTL MEYER RELATIONS //
// for expansion fans
// will include basic solver and Station update void func
namespace prandtlMeyer {
	float getPMangle(float gam, float M) {
		return sqrt((gam + 1) / (gam - 1)) * atan(sqrt(((gam - 1) / (gam + 1)) * (pow(M, 2) - 1))) - atan(sqrt(pow(M, 2) - 1));
	}
	void updateStationPM(Station &sta, float A) {
		float PMin = getPMangle(sta.gam, sta.M);
		float Mout = sta.M;
		float Aout = A;
		float &wipM = Mout;
		float &wipA = Aout;
		for (int i = 1; i < 501; i++) {
			wipM = sta.M + (i * 0.05);
			wipA = getPMangle(sta.gam, wipM) - PMin;
			//std::cout << "\ni=" << i << " wipM=" << wipM << " wipA=" << wipA << " diff=" << abs(A - wipA);
			if (abs(A - wipA) <= 0.05) {
				break;
			}
		}
		for (int i = 1; i < 501; i++) {
			wipM = Mout + (i * 0.0001);
			wipA = getPMangle(sta.gam, wipM) - PMin;
			//std::cout << "\ni=" << i << " wipM=" << wipM << " wipA=" << wipA << " diff=" << abs(A - wipA);
			if (abs(A - wipA) <= 0.0001) {
				break;
			}
		}
		sta.M = wipM;
		isen::getP_fromPt(sta);
		isen::getT_fromTt(sta);
		isen::getRho_fromPT(sta);
		calImp::getGam(sta);
		calImp::getCp(sta);
		isen::getVelocity(sta);
	}
}

// RAYLEIGH FLOW RELATIONS //
// for combustion
namespace rayleigh {
	double Tt_Ttstar(Station &sta, float &M) {
		double aTtstar = 1 + sta.gam * pow(M, 2);
		return ((2 * (sta.gam + 1) * pow(M, 2)) / pow(aTtstar, 2)) * (1 + ((sta.gam - 1) / 2) * pow(M, 2));
	}
	double Pt_Ptstar(Station &sta, float &M) {
		double aPtstar = (2 / (sta.gam + 1)) * (1 + ((sta.gam - 1) / 2) * pow(M, 2));
		double bPtstar = sta.gam / (sta.gam - 1);
		return ((sta.gam + 1) / (1 + sta.gam * pow(M, 2))) * pow(aPtstar, bPtstar);
	}
	double T_Tstar(Station &sta, float &M) {
		float aTstar = sta.gam + 1;
		double bTstar = 1 + sta.gam * pow(M, 2);
		return (aTstar * pow(M, 2)) / pow(bTstar, 2);
	}
	double P_Pstar(Station &sta, float &M) {
		return (sta.gam + 1) / (1 + sta.gam * pow(M, 2));
	}
	void updateStation_plusMstep(Station &sta, float Mstep) {
		float Ttstar = sta.Tt / Tt_Ttstar(sta, sta.M);
		float Ptstar = sta.Pt / Pt_Ptstar(sta, sta.M);
		float Tstar = sta.T / T_Tstar(sta, sta.M);
		float Pstar = sta.P / P_Pstar(sta, sta.M);
		float Mout = sta.M + Mstep;
		float Ttout = Ttstar * Tt_Ttstar(sta, Mout);
		float Ptout = Ptstar * Pt_Ptstar(sta, Mout);
		float Tout = Tstar * T_Tstar(sta, Mout);
		float Pout = Pstar * P_Pstar(sta, Mout);
		if (Ttstar >= Ttout) {
			sta.M = Mout;
			sta.P = Pout;
			sta.T = Tout;
			calImp::getGam(sta);
			calImp::getCp(sta);
			isen::getRho_fromPT(sta);
			sta.Pt = Ptout;
			sta.Tt = Ttout;
			isen::getVelocity(sta);
		}
	}
}

// FANNO FLOW RELATIONS //
// for friction stuff, when I develop the necessary masochistic cajones
namespace fanno {
}
