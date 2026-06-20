// struct definitions
struct Station {
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
};

enum ShockType {
	SHOCKERR = 0,
	WEAK,
	STRONG,
	NORMAL
};

struct Shock {
	float defDeg;
	float shockDeg;
	enum ShockType type;
	unsigned char flow;
	unsigned char pos;
};

struct Expansion {
	float defDeg;
	float degMu1;
	float degMu2;
	unsigned char flow;
	unsigned char pos;
};

// lil macro to make things slightly less annoying
#define EGGIN struct Station *sta
#define SHOCKIN struct Shock *sho

// mass flow
void getA_fromMdot(EGGIN);
void getMdot_fromA(EGGIN);

// calorically imperfect
void getGam(EGGIN);
void getCp(EGGIN);

// isentropic
void getTt_fromT(EGGIN);
void getPt_fromP(EGGIN);
void getT_fromTt(EGGIN);
void getP_fromPt(EGGIN);
void getVelocity(EGGIN);
void getMach(EGGIN);
void getRho_fromPT(EGGIN);
float getDynamicP(EGGIN);

// shock relations TODO
//void updateStationNormal(EGGIN, SHOCKIN);
//void updateStationOblique(EGGIN, SHOCKIN);
//void updateStationPM(EGGIN, struct Expansion *fan);
//void updateStationRayleigh(EGGIN);

