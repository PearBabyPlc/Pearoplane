// universal definitions
#define DEFAULT_W 1200
#define DEFAULT_H 800
#define G_UNIV 6.6743E-11
#define M_EARTH 5.97217E24
#define R_EARTH 6356766.0
#define ZOOM_SHIP 0.0005
#define ZOOM_MAP 0.000025

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
void flight3D();
