// Raylib headers
#include "raylib.h"
#include "resource_dir.h"

// C standard library
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

// helper.h
typedef char ucBuf[255];

// userinput.h
// credit to: https://www.raylib.com/examples/core/loader.html?name=core_input_actions
typedef enum ActionType {
	NO_ACTION = 0,
	DEV_ASCEND,
	DEV_DESCEND,
	DEV_ACCEL,
	DEV_DECEL,
	DEV_FINE,
	DEV_COARSE,
	DEV_RESET,
	PITCH_UP,
	PITCH_DOWN,
	YAW_LEFT,
	YAW_RIGHT,
	ROLL_LEFT,
	ROLL_RIGHT,
	THROTTLE_UP,
	THROTTLE_DOWN,
	MAX_ACTION
} ActionType;

typedef struct ActionInput {
	int key;
	int button;
} ActionInput;

static int gamepadIndex = 0;
static ActionInput actionInputs[MAX_ACTION] = { 0 };

static bool IsActionPressed(int action);
static bool IsActionReleased(int action);
static bool IsActionDown(int action);

static void SetActionsDefault(void);

static bool IsActionPressed(int action) {
	bool result = false;
	if (action < MAX_ACTION) result = (IsKeyPressed(actionInputs[action].key));
	return result;
}

static bool IsActionReleased(int action) {
	bool result = false;
	if (action < MAX_ACTION) result = (IsKeyReleased(actionInputs[action].key));
	return result;
}

static bool IsActionDown(int action) {
	bool result = false;
	if (action < MAX_ACTION) result = (IsKeyDown(actionInputs[action].key));
	return result;
}

static void SetActionsDefault() {
	actionInputs[DEV_ASCEND].key = KEY_UP;
	actionInputs[DEV_DESCEND].key = KEY_DOWN;
	actionInputs[DEV_ACCEL].key = KEY_RIGHT;
	actionInputs[DEV_DECEL].key = KEY_LEFT;
	actionInputs[DEV_FINE].key = KEY_Z;
	actionInputs[DEV_COARSE].key = KEY_X;
	actionInputs[DEV_RESET].key = KEY_TAB;
	actionInputs[PITCH_UP].key = KEY_S;
	actionInputs[PITCH_DOWN].key = KEY_W;
	actionInputs[YAW_LEFT].key = KEY_A;
	actionInputs[YAW_RIGHT].key = KEY_D;
	actionInputs[ROLL_LEFT].key = KEY_Q;
	actionInputs[ROLL_RIGHT].key = KEY_E;
	actionInputs[THROTTLE_UP].key = KEY_LEFT_SHIFT;
	actionInputs[THROTTLE_DOWN].key = KEY_LEFT_CONTROL;
}

// isa.h
typedef struct {
	float alt;
	float H;
	float T;
	float P;
	float rho;
	//float RH;
} ISA;

const float rEarth = 6356766;
const float g = 9.80665;
const float molISA = 28.9644; //      these fuckin constants
const float RISA = 8314.46; //        kept conflicting with
const float RsISA = RISA / molISA; // formulaegg.c, so renamed

void getISA(ISA *ptr) {
	ptr->H = (rEarth * ptr->alt) / (rEarth + ptr->alt);

	if (ptr->H < 11000) {
		// troposphere, positive lapse rate (-temp with altitude)
		ptr->T = 288.15 + (-0.0065 * ptr->H);
		double Pexp = (g * molISA) / (RISA * -0.0065);
		double Pfrac = 288.15 / ptr->T;
		ptr->P = 101325.0 * pow(Pfrac, Pexp);
		// refT = 288.15
		// refP = 101325
		// refH = 0
		// refL = -0.0065	
	} else if (ptr->H >= 11000 && ptr->H < 20000) {
		// tropopause, constant temperature (zero lapse rate)
		ptr->T = 216.65;
		double Pexp = (-g * molISA * (ptr->H - 11000)) / (RISA * 216.65);
		ptr->P = 22632.6352091 * exp(Pexp);
		// refT = 216.65;
		// refP = 22632.6
		// refH = 11000
		// refL = ±0
	} else if (ptr->H >= 20000 && ptr->H < 32000) {
		// lower stratosphere, negative lapse rate (+temp with altitude)
		ptr->T = 216.65 + (0.001 * (ptr->H - 20000));
		double Pexp = (g * molISA) / (RISA * 0.001);
		double Pfrac = 216.65 / ptr->T;
		ptr->P = 5475.15769257 * pow(Pfrac, Pexp);
		// refT = 216.65
		// refP = 5475.16
		// refH = 20000
		// refL = +0.001
	} else if (ptr->H >= 32000 && ptr->H < 47000) {
		// upper stratosphere, negative lapse rate
		ptr->T = 228.65 + (0.0028 * (ptr->H - 32000));
		double Pexp = (g * molISA) / (RISA * 0.0028);
		double Pfrac = 228.65 / ptr->T;
		ptr->P = 868.088257003 * pow(Pfrac, Pexp);
		// refT = 228.65
		// refP = 868.088
		// refH = 32000
		// refL = +0.0028
	} else if (ptr->H >= 47000 && ptr->H < 51000) {
		// stratopause, constant temperature (zero lapse rate)
		ptr->T = 270.65;
		double Pexp = (-g * molISA * (ptr->H - 47000)) / (RISA * 270.65);
		ptr->P = 110.919037481 * exp(Pexp);
		// refT = 270.65
		// refP = 110.919
		// refH = 47000
		// refL = ±0
	} else if (ptr->H >= 51000 && ptr->H < 71000) {
		// lower mesosphere, positive lapse rate
		ptr->T = 270.65 + (-0.0028 * (ptr->H - 51000));
		double Pexp = (g * molISA) / (RISA * -0.0028);
		double Pfrac = 270.65 / ptr->T;
		ptr->P = 66.9471267903 * pow(Pfrac, Pexp);
		// refT = 270.65
		// refP = 66.9471
		// refH = 51000
		// refL = -0.0028
	} else if (ptr->H >= 71000 && ptr->H < 84852) {
		// upper mesosphere, positive lapse rate
		ptr->T = 214.65 + (-0.002 * (ptr->H - 71000));
		double Pexp = (g * molISA) / (RISA * -0.002);
		double Pfrac = 214.65 / ptr->T;
		ptr->P = 3.95709671594 * pow(Pfrac, Pexp);
		// refT = 214.65
		// refP = 3.9570967
		// refH = 71000
		// refL = -0.002
	} else if (ptr->H >= 84852 && ptr->H < 90000) {
		// mesopause, constant temperature
		ptr->T = 186.95;
		double Pexp = (-g * molISA * (ptr->H - 84852)) / (RISA * 186.95);
		ptr->P = 0.373462257454 * exp(Pexp);
		// refT = 186.95
		// refP = 0.373462257
		// refH = 84852
		// refL = ±0
	} else if (ptr->H >= 90000) {
		// thermosphere, negative lapse rate
		ptr->T = 186.95 + (0.004 * (ptr->H - 90000));
		double Pexp = (g * molISA) / (RISA * 0.004);
		double Pfrac = 186.95 / ptr->T;
		ptr->P = 0.145778536657 * pow(Pfrac, Pexp);
		// refT = 186.95
		// refP = 0.145778536657
		// refH = 90000
		// refL = +0.004
	}
	
	if (ptr->alt > 307446) {
		// set temp to constant 1000K, and P/rho to zero
		ptr->T = 1000.0; // later idk maybe implement solar radiation
		ptr->P = 0.0;
		ptr->rho = 0.0;
	} else {
		// otherwise calculate rho normally
		ptr->rho = ptr->P / (ptr->T * RsISA);
	}
}

// formulaegg.h
#define STAT_IN Station *sta
#define SHOCK_IN Shock *sho
#define EXFAN_IN Expansion *fan
#define COMB_IN hydrogenCombustion *comb

typedef struct {
	float M;
	float P;
	float T;
	float rho;
	float gam;
	float Cp;
	float Pt;
	float Tt;
	float V;
	float A;
	float mdot;
	unsigned char flow;
	unsigned char pos;
} Station;

enum ShockType {
	SHOCKERR = 0,
	WEAK,
	STRONG,
	NORMAL,
	NO_SHOCK,
	TOO_FAST
};

typedef struct {
	float defDeg;
	float shockDeg;
	enum ShockType type;
	unsigned char flow;
	unsigned char pos;
} Shock;

typedef struct {
	float defDeg;
	float degMu1;
	float degMu2;
	unsigned char flow;
	unsigned char pos;
} Expansion;

enum combType {
	CONG = 0,
	SUBSONIC,
	SCRAMJET
};

typedef struct {
	float maxTemp;
	float limMach;
	float LHV;
	float fuelMdot;
	enum combType type;
	unsigned char flow;
	unsigned char pos;
} hydrogenCombustion;

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

void getA_fromMdot(STAT_IN) {
	sta->A = sta->mdot / (sta->rho * sta->V);
}

void getMdot_fromA(STAT_IN) {
	sta->mdot = sta->rho * sta->V * sta->A;
}

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
}

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

// replace pow(M, 2) = Msq inline with just M * M lol
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

void solveCombustion(STAT_IN, COMB_IN) {
	//comb->maxTt = sta->Tt / Tt_Ttstar(sta, sta->M);
	comb->fuelMdot = 0.0;
	//comb->subsonic = true;
	double LHVdiv = 1 / comb->LHV;

	float Mstep;
	if (comb->type == SUBSONIC) {
		Mstep = 0.0005;
	} else if (comb->type == SCRAMJET) {
		Mstep = -0.0005;
	}
	
	RayleighStar ray;
	while (sta->T <= comb->maxTemp) {
		getRayleighStars(sta, &ray);
		printf("\nTtstar=%.1f fuel=%.3f M=%.2f gam=%.3f Tt=%.1f", ray.Ttstar, comb->fuelMdot, sta->M, sta->gam, sta->Tt);
		sta->M += Mstep;
		updateStationRayleigh(sta, &ray);
		getGam(sta);
		getCp(sta);
		comb->fuelMdot += (sta->Tt - ray.prevTt) * sta->Cp * sta->mdot * LHVdiv;

		//if (sta->M > comb->limMach && comb->type == SUBSONIC) break;
		if (comb->type == SUBSONIC && sta->M > comb->limMach) break;
		if (comb->type == SCRAMJET && sta->M < comb->limMach) break;
	}

	getRho_fromPT(sta);
	getVelocity(sta);
	getA_fromMdot(sta); // leave this in, combustor area matters
}

void solveMixingFlows(STAT_IN, Station *add) {
	float mdotOut = sta->mdot + add->mdot;
	float Aout = sta->A + add->A;
	double Ksta = 0.5 * sta->mdot * sta->V * sta->V;
	double Kadd = 0.5 * sta->mdot * sta->V * sta->V;
	double Kout = Ksta + Kadd;
	float Vout = sqrt((2 * Kout) / mdotOut);
	float Qout = Aout * Vout;
	sta->rho = mdotOut / Qout;
	double uRMSsta = sqrt(3 * RsAir * sta->T);
	double uRMSadd = sqrt(3 * RsAir * add->T);
	double nSta = sta->mdot / molAir;
	double nAdd = add->mdot / molAir;
	double nOut = nSta + nAdd;
	double uRMSout = sqrt(((nSta * uRMSsta * uRMSsta) + (nAdd * uRMSadd * uRMSadd)) / nOut);
	sta->T = (uRMSout * uRMSout) / (3 * RsAir);
	sta->P = RsAir * sta->T * sta->rho;
	sta->A = Aout;
	sta->V = Vout;
	sta->mdot = mdotOut;
	getGam(sta);
	getCp(sta);
	getMach(sta);
	getPt_fromP(sta);
	getTt_fromT(sta);
}

void solveFlowSplit(STAT_IN, Station *byp, float *bypR) {
	*byp = *sta;
	float mdotIn = sta->mdot;
	sta->mdot = mdotIn / (*bypR + 1.0);
	byp->mdot = mdotIn - sta->mdot;
	getA_fromMdot(sta);
	getA_fromMdot(byp);
}

#undef STAT_IN
#undef SHOCK_IN
#undef EXFAN_IN
#undef COMB_IN

// panel.h
void bufferStation(ucBuf *buf, Station *sta) {
	sprintf(*buf, "M: %.3f\nP: %.1f\nT: %.1f\nrho: %.5f\ngam: %.3f\nCp: %.1f\nPt: %.1f\nTt: %.1f\nV: %.1f\nA: %.3f\nmdot: %.1f\nflow: %d\npos: %d",
			sta->M, sta->P, sta->T, sta->rho, sta->gam, sta->Cp, sta->Pt, sta->Tt, sta->V, sta->A, sta->mdot, sta->flow, sta->pos);
}

void drawPanel(ucBuf *ptrString, ucBuf *ptrStringB, ucBuf *ptrN) {
	DrawText(*ptrString, 200, 80, 30, (Color){245, 255, 245, 255});
	DrawText(*ptrStringB, 500, 80, 30, (Color){245, 255, 245, 255});
	DrawText(*ptrN, 800, 80, 30, (Color){245, 255, 245, 255});
	DrawText("Ambient", 200, 40, 40, WHITE);
	DrawText("Compress", 500, 40, 40, WHITE);
	DrawText("Expand", 800, 40, 40, WHITE);
	DrawText("shock deflection angle of 10\u00B0\narrow keys for altitude/velocity\n[tab] to reset\nZ for fine control\nX for coarse", 50, 600, 25, (Color){255, 225, 225, 255});
	DrawFPS(10, 10);
}

// not at all based on physics, just looks kinda right
void getSkyRGB(float *skyR, float *skyG, float *skyB, float altitude) {
	if (altitude >= 80000) {
		*skyR = 0;
		*skyG = 0;
		*skyB = 0;
	} else if (altitude <= 0) {
		*skyR = 130;
		*skyG = 210;
		*skyB = 250;
	} else {
		*skyR = 135 * ((-1 * (altitude - 80000)) / 80000);
		*skyG = 205 * (-sqrt(altitude / 80000) + 1);
		*skyB = 250 * sqrt((altitude - 80000) / -80000);
	}
}

// original main.c body
const int sW = 1200;
const int sH = 800;
const float DeadSea = -439.8; // lowest elevation on Earth

int main() {
	// setup raylib window
	InitWindow(sW, sH, "Pearoplane Alpha v0.1");
	//SetTargetFPS(120);
	char actionSet = 0;
	SetActionsDefault();
	bool releaseAction = false;
	float skyR;
	float skyG;
	float skyB;

	// setup debug controls
	float altitude = 0.0;
	float velocity = 0.0;
	float sensitivity = 1.0;

	// setting up test station
	ISA ambISA;
	Station ambSta;
	ambSta.A = 10.0;
	ambSta.flow = 1;
	ambSta.pos = 1;
	ucBuf ambBuf;
	char altBuf[50];
		
	while (!WindowShouldClose()) {
		// taking user input
		gamepadIndex = 0;
		if (IsActionDown(DEV_RESET)) {
			altitude = 0.0;
			velocity = 0.0;
		}
		if (IsActionDown(DEV_FINE)) {
			sensitivity = 0.01;
		} else if (IsActionDown(DEV_COARSE)) {
			sensitivity = 100.0;
		} else {
			sensitivity = 1.0;
		}
		if (IsActionDown(DEV_ASCEND)) altitude += 1.0 * sensitivity;
		if (IsActionDown(DEV_DESCEND)) {
			if (altitude <= DeadSea) {
				altitude = DeadSea;
			} else {
				altitude -= 1.0 * sensitivity;
			}
		}
		if (IsActionDown(DEV_DECEL)) {
			if (velocity <= 0.1) {
				velocity = 0.0;
			} else {
				velocity -= 0.5 * sensitivity;
			}
		}
		if (IsActionDown(DEV_ACCEL)) velocity += 0.5 * sensitivity;
		releaseAction = false;
		
		// calculate ambient conditions
		ambISA.alt = altitude;
		ambSta.V = velocity;
		getISA(&ambISA);
		ambSta.P = ambISA.P;
		ambSta.T = ambISA.T;
		ambSta.rho = ambISA.rho;
		getGam(&ambSta);
		getMach(&ambSta);
		getCp(&ambSta);
		getPt_fromP(&ambSta);
		getTt_fromT(&ambSta);
		getMdot_fromA(&ambSta);
		bufferStation(&ambBuf, &ambSta);

		// drawing the buffers
		BeginDrawing();
		getSkyRGB(&skyR, &skyG, &skyB, altitude);
		ClearBackground((Color){skyR, skyG, skyB, 255});
		DrawText(ambBuf, 100, 80, 30, RAYWHITE);
		sprintf(altBuf, "Altitude: %.1fm", altitude);
		DrawText(altBuf, 100, 40, 40, WHITE);
		DrawFPS(10, 10);
		EndDrawing();
	}

	CloseWindow();
