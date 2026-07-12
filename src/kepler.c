#include <math.h>

#include "orbit.h"
#ifdef KEPLER

typedef struct {
	double x;
	double y;
	double z;
} DVec3;

void keplerOrbitDVec3(DVec3 *ptr, DVec3 *origin, double t, double ecc, double inc, double aPe, double LAN, double SMA) {
	// ensure aPe and LAN are appropriately offset, and all angles are in radians

	// create the ellipse
	double sina = sin(t);
	double cosa = cos(t);
	double ogX = (SMA * cosa * (1.0 - (ecc * ecc))) / (1.0 + (ecc * cosa));
	double ogY = (SMA * sina * (1.0 - (ecc * ecc))) / (1.0 + (ecc * cosa));
	double ogZ = 0.0;
	
	// rotate along X (inclination)
	sina = sin(inc);
	cosa = cos(inc);
	ogY = ogY * cosa;
	ogZ = ogY * sina;

	// rotate along Y (argument of periapsis)
	sina = sin(aPe);
	cosa = cos(aPe);
	ogX = (ogZ * sina) + (ogX * cosa);
	ogZ = (ogY * cosa) + (ogX * -sina);
	
	// rotate along Z (longitude of ascending node)
	sina = sin(LAN);
	cosa = cos(LAN);
	ogX = (ogX * cosa) - (ogY * sina);
	ogY = (ogX * sina) + (ogY * cosa);

	// add to origin and return final vector
	ptr->x = origin->x + ogX;
	ptr->y = origin->y + ogY;
	ptr->z = origin->z + ogZ;
}
// unsure if the rotation matrices are correct, we shall see. probably need to ensure correct reference axes are used

#endif
