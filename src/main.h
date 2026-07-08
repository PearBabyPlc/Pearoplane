// universal definitions
#define DEFAULT_W 1400 // used to be 1200x800, changed to 
#define DEFAULT_H 720  // 35mm film dimensions for easier optics
#define G_UNIV 6.6743E-11
#define M_EARTH 5.97217E24
#define R_EARTH 6356766.0
#define ZOOM_SHIP 0.0005
#define ZOOM_MAP 0.000025

// these may have been defined before, so there's a bunch of ifndef stuff
#ifndef PI
#define PI 3.14159265358979323846
#endif
#ifndef RAD_DEG
#define RAD_DEG 57.295780181884765625
#endif
#ifndef DEG_RAD
#define DEG_RAD 0.01745329238474369049072265625
#endif

typedef struct {
	double x;
	double y;
	double dx;
	double dy;
	double g;
	float Hkm;
	float Vms;
	float timer;
} OrbitState;

typedef struct {
	double x;
	double y;
} Vec2;

typedef struct {
	double x;
	double y;
	double z;
} Vec3;

typedef struct {
	int v1;
	int v2;
} Line2;

typedef struct {
	int v1;
	int v2;
	int v3;
} Poly3;

typedef struct {
	int v1;
	int v2;
	int v3;
	int v4;
} Poly4;

void orbit2D();
void cube3D();
