#include "math.h"
#include "formulaegg.h" // included in solvers.c, solvers.h included in models.c, models.h included in aircraft.c, aircraft.h included in main.c

// was having problems with constants conflicting, renamed them in isa.c since they'd used less often
const float Theta = 3055 + (5 / 9);
const float gamDiatomic = 1.4;
const float CpiAir = 1005;
const float Runiv = 8314.46;
const float molAir = 28.9644;
const float RsAir = Runiv / molAir;
const double pi = 3.14159265359;
const double radToDeg = 180.0 / pi;
const double degToRad = pi / 180.0;

// mass flow
void getA_fromMdot(STAT_IN) {
	sta->A = sta->mdot / (sta->rho * sta->V);
}

void getMdot_fromA(STAT_IN) {
	sta->mdot = sta->rho * sta->V * sta->A;
}

// calorically imperfect specific heat stuff
void getGam(STAT_IN) {
	double ThetaT = Theta / sta->T;
	double ThetaTexpsub1 = exp(ThetaT) - 1;
	sta->gam = 1 + ((gamDiatomic - 1) / (1 + (gamDiatomic - 1) * (pow(ThetaT, 2) * (exp(ThetaT) / pow(ThetaTexpsub1, 2)))));
}

void getCp(STAT_IN) {
	double ThetaT = ThetaT / sta->T;
	double ThetaTexpsub1 = exp(ThetaT) - 1;
	sta->Cp = CpiAir * (1 + ((gamDiatomic - 1) / gamDiatomic) * (pow(ThetaT, 2) * (exp(ThetaT) / pow(ThetaTexpsub1, 2))));
}

// isentropic relations
void getTt_fromT(STAT_IN) {
	sta->Tt = sta->T * (1 + ((sta->gam - 1) / 2) * pow(sta->M, 2));
}

void getPt_fromP(STAT_IN) {
	double Ptow1 = 1 + ((sta->gam - 1) / 2) * pow(sta->M, 2);
	double Ptow2 = sta->gam / (sta->gam - 1);
	sta->Pt = sta->P * pow(Ptow1, Ptow2);
}

void getT_fromTt(STAT_IN) {
	sta->Tt = sta->T / (1 + ((sta->gam - 1) / 2) * pow(sta->M, 2));
}

void getP_fromPt(STAT_IN) {
	double Ptow1 = 1 + ((sta->gam - 1) / 2) * pow(sta->M, 2);
	double Ptow2 = sta->gam / (sta->gam - 1);
	sta->P = sta->Pt / pow(Ptow1, Ptow2);
}

void getVelocity(STAT_IN) {
	sta->V = sta->M * sqrt(RsAir * sta->gam * sta->T);
}

void getMach(STAT_IN) {
	sta->M = sta->V / sqrt(RsAir * sta->gam * sta->T);
}

void getRho_fromPT(STAT_IN) {
	sta->rho = sta->P / (RsAir * sta->T);
}

float getDynamicP(STAT_IN) {
	return 0.5 * sta->rho * pow(sta->V, 2);
}

// shock solver (only requires station in and shock in with deflection angle)
void updateStationNormal(STAT_IN, SHOCK_IN) {
	float pM = sta->M;
	sta->M = sqrt(((sta->gam - 1) * pow(pM, 2) + 2) / (2 * sta->gam * pow(pM, 2) - (sta->gam - 1)));
	sta->P *= (2 * sta->gam * pow(pM, 2) - (sta->gam - 1)) / (sta->gam + 1);
	float aT = sta->gam + 1;
	sta->T *= ((2 * sta->gam * pow(pM, 2) - (sta->gam - 1)) * ((sta->gam - 1) * pow(pM, 2) + 2)) / (pow(aT, 2) * pow(pM, 2));
	sta->rho *= ((sta->gam + 1) * pow(pM, 2)) / ((sta->gam - 1) * pow(pM, 2) + 2);
	double aPt = (aT * pow(pM, 2)) / ((sta->gam - 1) * pow(pM, 2) + 2);
	double bPt = aT / (2 * sta->gam * pow(pM, 2) - (sta->gam - 1));
	double cPt = sta->gam / (sta->gam - 1);
	double dPt = 1 / (sta->gam - 1);
	sta->Pt *= pow(aPt, cPt) * pow(bPt, dPt);
	sho->defDeg = 0.0;
	sho->shockDeg = 90.0;
	sho->type = NORMAL;
	getGam(sta);
	getTt_fromT(sta);
	getCp(sta);
	getVelocity(sta);
	getA_fromMdot(sta);
}

float MachAngle(STAT_IN) {
	return asin(1 / sta->M) * radToDeg;
}

float deflectionAngle(STAT_IN, float S) {
	double sinS = sin(S * degToRad);
	return atan(1 / (tan(S * degToRad) * ((((sta->gam + 1) * pow(sta->M, 2) / (2 * (pow(sta->M, 2) * pow(sinS, 2) - 1))) - 1)))) * radToDeg;
}

float maxDeflectionAngle(STAT_IN) {
	double onePointFive = pow(sta->M, 2) - 1;
	return (4 / (3 * sqrt(3) * (sta->gam + 1))) * (pow(onePointFive, 1.5) / pow(sta->M, 2)) * radToDeg;
}

void shockAngle(STAT_IN, SHOCK_IN) {
	int minAngleDeg = floor(MachAngle(sta));
	float wipA;
	float wipS;
	for (int i = minAngleDeg; i < 91; i++) {
		wipA = deflectionAngle(sta, (float)i);
		wipS = (float)i;
		if (fabsf(sho->defDeg - wipA) <= 1.0) break;
	}
	float S;
	for (int i = 0; i < 2001; i++) {
		S = wipS + (0.0005 * (float)i);
		wipA = deflectionAngle(sta, S);
		if (fabsf(sho->defDeg - wipA) <= 0.0005) break;
	}
	sho->shockDeg = S;
	//printf("\nOBLIQUE A=%.3f S=%.3f wipA=%.3f", sho->defDeg, S, wipA);
}

// ensure both the shock angle and deflection angle are known
void updateStationOblique(STAT_IN, SHOCK_IN) {
	float S = sho->shockDeg * degToRad;
	float A = sho->defDeg * degToRad;
	double sinS = sin(S);
	double sinSsubA = sin(S - A);
	float pM = sta->M;
	sta->M = sqrt((((sta->gam - 1) * pow(pM, 2) * pow(sinS, 2) + 2) / (2 * sta->gam * pow(pM, 2) * pow(sinS, 2) - (sta->gam - 1))) / pow(sinSsubA, 2));
	sta->P *= (2 * sta->gam * pow(pM, 2) * pow(sinS, 2) - (sta->gam - 1)) / (sta->gam + 1);
	double gamPlusOne = sta->gam + 1;
	sta->T *= ((2 * sta->gam * pow(pM, 2) * pow(sinS, 2) - (sta->gam - 1)) * ((sta->gam - 1) * pow(pM, 2) * pow(sinS, 2) + 2)) / (pow(gamPlusOne, 2) * pow(pM, 2) * pow(sinS, 2));
	sta->rho *= (gamPlusOne * pow(pM, 2) * pow(sinS, 2)) / ((sta->gam - 1) * pow(pM, 2) * pow(sinS, 2) + 2);
	double aPt = (gamPlusOne * pow(pM, 2) * pow(sinS, 2)) / ((sta->gam - 1) * pow(pM, 2) * pow(sinS, 2) + 2);
	double bPt = sta->gam / (sta->gam - 1);
	double cPt = gamPlusOne / (2 * sta->gam * pow(pM, 2) * pow(sinS, 2) - (sta->gam - 1));
	double dPt = 1 / (sta->gam - 1);
	sta->Pt *= pow(aPt, bPt) * pow(cPt, dPt);
	getGam(sta);
	getTt_fromT(sta);
	getCp(sta);
	getVelocity(sta);
	getA_fromMdot(sta);
}

void solveShock(STAT_IN, SHOCK_IN) {
	if (sta->M < 1.0) {
		sho->type = NO_SHOCK;
		sho->shockDeg = 0.0;
		//printf("\nNO_SHOCK A=%.3f S=%.3f", sho->defDeg, sho->shockDeg);
	} else if (sta->M >= 1.0 && sta->M <= 40.0) {
		float maxA = maxDeflectionAngle(sta);
		if (sho->defDeg < maxA) {
			shockAngle(sta, sho);
			updateStationOblique(sta, sho);
			sho->type = WEAK;
		} else if (sho->defDeg >= maxA) {
			updateStationNormal(sta, sho);
			//printf("\nNORMAL A=%.3f S=%.3f", maxA, sho->shockDeg);
		}
	} else {
		sho->type = TOO_FAST;
		sho->shockDeg = 12.0;
		updateStationOblique(sta, sho);
		//printf("\nTOO_FAST A=%.3f S=%.3f", sho->defDeg, sho->shockDeg);
	}
}

// TODO Prandtl-Meyer expansion fans
void updateStationPM(STAT_IN, EXFAN_IN) {
}

// TODO Rayleigh flow for combustion
void updateStationRayleigh(STAT_IN) {
}

#undef STAT_IN
#undef SHOCK_IN
#undef EXFAN_IN
