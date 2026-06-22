#include "math.h"
#include "stdio.h"
#include "helper.h"
#include "formulaegg.h" 

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
const double squirtThree = 1.73205080757;

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
	double pM = sta->M;
	double pMsq = pow(pM, 2);

	sta->M = sqrt(((sta->gam - 1) * pMsq + 2) / (2 * sta->gam * pMsq - (sta->gam - 1)));
	
	sta->P *= (2 * sta->gam * pMsq - (sta->gam - 1)) / (sta->gam + 1);
	
	double aT = sta->gam + 1;
	sta->T *= ((2 * sta->gam * pMsq - (sta->gam - 1)) * ((sta->gam - 1) * pMsq + 2)) / (pow(aT, 2) * pMsq);
	
	sta->rho *= ((sta->gam + 1) * pMsq) / ((sta->gam - 1) * pMsq + 2);
	
	double aPt = (aT * pMsq) / ((sta->gam - 1) * pMsq + 2);
	double bPt = aT / (2 * sta->gam * pMsq - (sta->gam - 1));
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
//	getA_fromMdot(sta); removed for better performance
}

float MachAngle(STAT_IN) {
	return asin(1 / sta->M) * radToDeg;
}

// input Station reference and explicit shock angle in degrees
float deflectionAngle(STAT_IN, float S) {
	double sinS = sin(S * degToRad);
	return atan(1 / (tan(S * degToRad) * ((((sta->gam + 1) * pow(sta->M, 2) / (2 * (pow(sta->M, 2) * pow(sinS, 2) - 1))) - 1)))) * radToDeg;
}

float maxDeflectionAngle(STAT_IN) {
	double onePointFive = pow(sta->M, 2) - 1;
	return (4 / (3 * squirtThree * (sta->gam + 1))) * (pow(onePointFive, 1.5) / pow(sta->M, 2)) * radToDeg;
}

void shockAngle(STAT_IN, SHOCK_IN) {
	int minAngleDeg = (int)MachAngle(sta);
	
	float wipA;
	float wipS;
	for (int i = minAngleDeg; i < 91; i++) {
		wipA = deflectionAngle(sta, i);
		wipS = (float)i;
		if (fabsf(sho->defDeg - wipA) <= 1.0) break;
	}
	
	for (int i = 1; i < 2002; i++) {
		wipS += 0.0005;
		wipA = deflectionAngle(sta, wipS);
		if (fabsf(sho->defDeg - wipA) <= 0.0005) break;
	}
	
	sho->shockDeg = wipS;
}

// ensure both the shock angle and deflection angle are known
void updateStationOblique(STAT_IN, SHOCK_IN) {
	float S = sho->shockDeg * degToRad;
	float A = sho->defDeg * degToRad;

	double sinS = sin(S);
	double sinSsubA = sin(S - A);
	float pM = sta->M;
	double pMsq = pow(pM, 2);

	double sinSsq = pow(sinS, 2);
	sta->M = sqrt((((sta->gam - 1) * pMsq * sinSsq + 2) / (2 * sta->gam * pMsq * sinSsq - (sta->gam - 1))) / pow(sinSsubA, 2));
	
	sta->P *= (2 * sta->gam * pMsq * sinSsq - (sta->gam - 1)) / (sta->gam + 1);
	
	double gamPlusOne = sta->gam + 1;
	sta->T *= ((2 * sta->gam * pMsq * sinSsq - (sta->gam - 1)) * ((sta->gam - 1) * pMsq * sinSsq + 2)) / (pow(gamPlusOne, 2) * pMsq * sinSsq);
	
	sta->rho *= (gamPlusOne * pMsq * sinSsq) / ((sta->gam - 1) * pMsq * sinSsq + 2);
	
	double aPt = (gamPlusOne * pMsq * sinSsq) / ((sta->gam - 1) * pMsq * sinSsq + 2);
	double bPt = sta->gam / (sta->gam - 1);
	double cPt = gamPlusOne / (2 * sta->gam * pMsq * sinSsq - (sta->gam - 1));
	double dPt = 1 / (sta->gam - 1);
	sta->Pt *= pow(aPt, bPt) * pow(cPt, dPt);
	
	getGam(sta);
	getTt_fromT(sta);
	getCp(sta);
	getVelocity(sta);
//	getA_fromMdot(sta); removed for better performance
}

void solveShock(STAT_IN, SHOCK_IN) {
	if (sta->M < 1.0) {
		sho->type = NO_SHOCK;
		sho->shockDeg = 0.0;
	} else if (sta->M >= 1.0 && sta->M <= 40.0) {
		float maxA = maxDeflectionAngle(sta);
		if (sho->defDeg < maxA) {
			shockAngle(sta, sho);
			updateStationOblique(sta, sho);
			sho->type = WEAK;
		} else if (sho->defDeg >= maxA) {
			updateStationNormal(sta, sho);
		}
	} else {
		sho->type = TOO_FAST;
		sho->shockDeg = 12.0;
		updateStationOblique(sta, sho);
	}
}

// Prandtl-Meyer expansion fans
// input Station reference and explicit Mach number
double getPMangle(STAT_IN, double M) {
	double Msq = pow(M, 2);
	return (sqrt((sta->gam + 1) / (sta->gam - 1)) * atan(sqrt(((sta->gam - 1) / (sta->gam + 1)) * (Msq - 1))) - atan(sqrt(Msq - 1))) * radToDeg;
}

void solveExpansion(STAT_IN, EXFAN_IN) {
	float PMin = getPMangle(sta, sta->M);
	double wipM = sta->M;
	float wipA;

	for (int i = 0; i < 501; i++) {
		wipM += 0.05;
		wipA = getPMangle(sta, wipM) - PMin;
		//printf("A=%f wipA=%f\n", fan->defDeg, wipA);
		if (fabsf(fan->defDeg - wipA) <= 2.0) break;
	}
	
	// this seems to be the best tuning for unrestricted performance
	// idfk how to improve, there aren't too many big jumps, fps remains above 600
	for (int i = 1; i < 2001; i++) {
		wipM += 0.0005;
		wipA = getPMangle(sta, wipM) - PMin;
		//printf("A=%f wipA=%f\n", fan->defDeg, wipA);
		if (fabsf(fan->defDeg - wipA) <= 0.01) break;
	}

	fan->degMu1 = MachAngle(sta);
	sta->M = wipM;
	fan->degMu2 = MachAngle(sta);
	getP_fromPt(sta);
	getT_fromTt(sta);
	getRho_fromPT(sta);
	getGam(sta);
	getCp(sta);
	getVelocity(sta);
//	getA_fromMdot(sta); removed, not applicable to supersonic aerofoils, also performance
}

// TODO Rayleigh flow for combustion
typedef struct {
	float Pstar;
	float Tstar;
	float Ptstar;
	float Ttstar;
	float prevTt;
} RayleighStar;

void getRayleighStars(STAT_IN, RayleighStar *ray) {
	ray->prevTt = sta->Tt;
	double Msq = pow(sta->M, 2);
	double aT = sta->gam + 1;
	double bT = 1 + sta->gam * Msq;
	double P_Pstar = aT / bT;
	ray->Pstar = sta->P / P_Pstar;
	double cT = pow(bT, 2);
	//T_Tstar = (aT * Msq) / cT;
	ray->Tstar = sta->T / ((aT * Msq) / cT);
	double aPt = (2 / aT) * (1 + ((sta->gam - 1) / 2) * Msq);
	double bPt = sta->gam / (sta->gam - 1);
	//Pt_Ptstar = P_Pstar * pow(aPt, bPt);
	//Tt_Ttstar = ((2 * aT * Msq) / cT) * (1 + ((sta->gam - 1) / 2) * Msq);
	ray->Ptstar = sta->Pt / (P_Pstar * pow(aPt, bPt));
	ray->Ttstar = sta->Tt / (((2 * aT * Msq) / cT) * (1 + ((sta->gam - 1) / 2) * Msq));
}

void updateStationRayleigh(STAT_IN, RayleighStar *ray) {
	double Msq = pow(sta->M, 2);
	double aT = sta->gam + 1;
	double bT = 1 + sta->gam * Msq;
	double P_Pstar = aT / bT;
	sta->P = ray->Pstar * P_Pstar;
	double cT = pow(bT, 2);
	sta->T = ray->Tstar * ((aT * Msq) / cT);
	double aPt = (2 / aT) * (1 + ((sta->gam - 1) / 2) * Msq);
	double bPt = sta->gam / (sta->gam - 1);
	sta->Pt = ray->Ptstar * (P_Pstar * pow(aPt, bPt));
	sta->Tt = ray->Ttstar * (((2 * aT * Msq) / cT) * (1 + ((sta->gam - 1) / 2) * Msq));
}

//	float maxTemp;
//	float limMach;
//	float LHV;
//	float fuelMdot;
//	bool subsonic;
//	unsigned char flow;
//	unsigned char pos;
//hydrogenCombustion;

void solveSubsonicCombustion(STAT_IN, COMB_IN) {
	//comb->maxTt = sta->Tt / Tt_Ttstar(sta, sta->M);
	comb->fuelMdot = 0.0;
	//comb->subsonic = true;
	double LHVdiv = 1 / comb->LHV;
	
	RayleighStar ray;
	while (sta->T <= comb->maxTemp) {
		getRayleighStars(sta, &ray);
		printf("\nTtstar=%.1f fuel=%.3f M=%.2f gam=%.3f Tt=%.1f", ray.Ttstar, comb->fuelMdot, sta->M, sta->gam, sta->Tt);
		sta->M += 0.0005;
		updateStationRayleigh(sta, &ray);
		getGam(sta);
		getCp(sta);
		comb->fuelMdot += (sta->Tt - ray.prevTt) * sta->Cp * sta->mdot * LHVdiv;

		if (sta->M > comb->limMach) break;
	}

	getRho_fromPT(sta);
	getVelocity(sta);
	getA_fromMdot(sta); // leave this in, combustor area matters
}

#undef STAT_IN
#undef SHOCK_IN
#undef EXFAN_IN
#undef COMB_IN
