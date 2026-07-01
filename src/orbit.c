#include <math.h>
#include "main.h"
#include "physics.h"

#define R_EARTH 6356766.0
#define G_UNIV 6.6743E-11
#define M_EARTH 5.97217E24

double g_at_r(double r) {
	return (G_UNIV * M_EARTH) / (r * r);
}

void PerformOrbit(OrbitState *ptr, double dt) {
	double r = sqrt((ptr->x * ptr->x) + (ptr->y * ptr->y));
	double prevh = ptr->h;
	ptr->h = r - R_EARTH;
	ptr->V = sqrt((ptr->dx * ptr->dx) + (ptr->dy * ptr->dy));
	ptr->g = g_at_r(r);

	double beta = atan(ptr->y / ptr->x);
	if (ptr->x == 0.0) {
		if (ptr->y > 0.0) ptr->dy -= ptr->g * dt;
		if (ptr->y < 0.0) ptr->dy += ptr->g * dt;
	} else {
		if (ptr->x > 0.0) ptr->dy -= sin(beta) * ptr->g * dt;
		if (ptr->x < 0.0) ptr->dy += sin(beta) * ptr->g * dt;
	}
	if (ptr->y == 0.0) {
		if (ptr->x > 0.0) ptr->dx -= ptr->g * dt;
		if (ptr->x < 0.0) ptr->dx += ptr->g * dt;
	} else {
		if (ptr->x > 0.0) ptr->dx -= cos(beta) * ptr->g * dt;
		if (ptr->x < 0.0) ptr->dx += cos(beta) * ptr->g * dt;
	}

	ptr->x += ptr->dx * dt;
	ptr->y += ptr->dy * dt;

	ptr->KE = 0.5 * ptr->V * ptr->V;
	ptr->GPE = ptr->g * ptr->h;
	ptr->TE = ptr->KE + ptr->GPE;
	ptr->timer += (float)dt;
}


