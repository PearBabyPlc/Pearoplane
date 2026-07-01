#include "raylib.h"
#include "resource_dir.h"
#include <stdio.h>
#include <math.h>

#include "main.h"
#include "aero.h"

#define PI 3.14159265359

const double degToRad = PI / 180.0;
const double radToDeg = 180.0 / PI;

double K_V(float x) {
	return (0.0223483 * x * x) - (0.0074753 * x) + 3.13364;
}

double K_p(float x) {
	return (0.0292594 * x * x * x) - (0.323453 * x * x) + (1.68158 * x) - 0.0525724;
}

double C_L(float AoA, float AR) {
	double KV = K_V(AR);
	double Kp = K_p(AR);
	return (Kp * sin(AoA * degToRad) * cos(AoA * degToRad) * cos(AoA * degToRad)) + (KV * cos(AoA * degToRad) * sin(AoA * degToRad) * sin(AoA * degToRad));
}

double OswaldE(float AR) {
	return 1 / (1.05 + (0.007 * PI * AR));
}

double dynamicViscosity(float T) {
	return ((1.458 * 0.000001) * sqrt(T)) / (1 + (110.4 / T));
}

void aeroTest(AeroTest *ptr) {
	float AR = (ptr->span * ptr->span) / ptr->area;

	ISA isa;
	isa.alt = ptr->altitude;
	getISA(&isa);

	float SoS = sqrt(287.0 * 1.4 * isa.T);
	float M = ptr->velocity / SoS;

	double CL = C_L(ptr->AoA, AR);	

	double E = OswaldE(AR);
	double CDi = (4 * CL * CL) / (PI * AR * E);

	double mu = dynamicViscosity(isa.T);
	double Re = (isa.rho * ptr->velocity * 0.5 * ptr->chord) / mu;
	double Cf = Cf = 0.074 / pow(Re, 0.2);
	double CDw = 0.0;

	if (M > 1.0) {
		double multi = 1 / (1 + 0.15 * (M * M));
		Cf *= pow(multi, 0.58);
		CDw += (CL * CL) / (4 * ((M * M) - 1));
	}

//	double CDv = (Cf * isa.rho * sin(ptr->AoA * degToRad) * ptr->area) / (isa.rho * ptr->area);
//	wrong, wetted area is 2x wing area
	double CDv = (Cf * isa.rho * 2 * ptr->area) / (isa.rho * ptr->area);

	double CD = CDi + CDv + CDw;
	double Q = 0.5 * isa.rho * ptr->velocity * ptr->velocity;
	ptr->lift = CL * ptr->area * Q;
	ptr->drag = CD * ptr->area * Q;
	ptr->LD = ptr->lift / ptr->drag;
	ptr->lift /= 9.80665 * 1000.0;
	ptr->drag /= 1000.0;
}

// altitude velocity area span AoA lift drag LD
