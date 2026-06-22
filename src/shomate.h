// this needs to go in shomate.c
const float Rshomate = 8314.46;
const float molH2O = 18.015;
const float molCO2 = 44.009;

enum Molecule {
	METHYLENEDIOXY = 0,
	WATER_VAPOUR,
	CARBON_DIOXIDE
};

typedef struct {
	float Cp;
	float Rs;
	float Cv;
	float gam;
	enum Molecule molName;
} CombProduct;

void getShomate(CombProduct *ptr);
