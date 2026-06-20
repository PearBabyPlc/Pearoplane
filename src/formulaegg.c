#include "math.h"
#include "formulaegg.h"

// was having problems with constants conflicting, renamed them in isa.c since they'd used less often
const float Theta = 3055 + (5 / 9);
const float gamDiatomic = 1.4;
const float CpiAir = 1005;
const float Runiv = 8314.46;
const float molAir = 28.9644;
const float RsAir = Runiv / molAir;

// mass flow
void getA_fromMdot(EGGIN) {
	sta->A = sta->mdot / (sta->rho * sta->V);
}
void getMdot_fromA(EGGIN) {
	sta->mdot = sta->rho * sta->V * sta->A;
}

// calorically imperfect specific heat stuff
void getGam(EGGIN) {
	double ThetaT = Theta / sta->T;
	double ThetaTexpsub1 = exp(ThetaT) - 1;
	sta->gam = 1 + ((gamDiatomic - 1) / (1 + (gamDiatomic - 1) * (pow(ThetaT, 2) * (exp(ThetaT) / pow(ThetaTexpsub1, 2)))));
}
void getCp(EGGIN) {
	double ThetaT = ThetaT / sta->T;
	double ThetaTexpsub1 = exp(ThetaT) - 1;
	sta->Cp = CpiAir * (1 + ((gamDiatomic - 1) / gamDiatomic) * (pow(ThetaT, 2) * (exp(ThetaT) / pow(ThetaTexpsub1, 2))));
}

// isentropic relations
void getTt_fromT(EGGIN) {
	sta->Tt = sta->T * (1 + ((sta->gam - 1) / 2) * pow(sta->M, 2));
}
void getPt_fromP(EGGIN) {
	double Ptow1 = 1 + ((sta->gam - 1) / 2) * pow(sta->M, 2);
	double Ptow2 = sta->gam / (sta->gam - 1);
	sta->Pt = sta->P * pow(Ptow1, Ptow2);
}
void getT_fromTt(EGGIN) {
	sta->Tt = sta->T / (1 + ((sta->gam - 1) / 2) * pow(sta->M, 2));
}
void getP_fromPt(EGGIN) {
	double Ptow1 = 1 + ((sta->gam - 1) / 2) * pow(sta->M, 2);
	double Ptow2 = sta->gam / (sta->gam - 1);
	sta->P = sta->Pt / pow(Ptow1, Ptow2);
}
void getVelocity(EGGIN) {
	sta->V = sta->M * sqrt(RsAir * sta->gam * sta->T);
}
void getMach(EGGIN) {
	sta->M = sta->V / sqrt(RsAir * sta->gam * sta->T);
}
void getRho_fromPT(EGGIN) {
	sta->rho = sta->P / (RsAir * sta->T);
}
float getDynamicP(EGGIN) {
	return 0.5 * sta->rho * pow(sta->V, 2);
}

// TODO normal shock relations
//void updateStationNormal(EGGIN, SHOCKIN);

// TODO oblique shock relations
//void updateStationOblique(EGGIN, SHOCKIN);

// TODO Prandtl-Meyer expansion fans
//void updateStationPM(EGGIN, struct Expansion *fan);

// TODO Rayleigh flow for combustion
//void updateStationRayleigh(EGGIN);

