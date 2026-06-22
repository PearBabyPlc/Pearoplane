// macros for a slightly easier time
#define STAT_IN Station *sta
#define SHOCK_IN Shock *sho
#define EXFAN_IN Expansion *fan
#define COMB_IN hydrogenCombustion *comb

// struct definitions
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

// mass flow
void getA_fromMdot(STAT_IN);
void getMdot_fromA(STAT_IN);

// calorically imperfect
void getGam(STAT_IN);
void getCp(STAT_IN);

// isentropic
void getTt_fromT(STAT_IN);
void getPt_fromP(STAT_IN);
void getT_fromTt(STAT_IN);
void getP_fromPt(STAT_IN);
void getVelocity(STAT_IN);
void getMach(STAT_IN);
void getRho_fromPT(STAT_IN);
float getDynamicP(STAT_IN);

// solvers
void solveShock(STAT_IN, SHOCK_IN);
void solveExpansion(STAT_IN, EXFAN_IN);
void solveSubsonicCombustion(STAT_IN, COMB_IN);

// solution to performance issues maybe - for wings change how often PM angles are calculated (more often below Mach 3, less often above)
