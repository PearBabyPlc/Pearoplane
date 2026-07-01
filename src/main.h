#define DEFAULT_W 1200
#define DEFAULT_H 800
#define SCALE 0.000025
#define TRACER_LEN 255

typedef struct {
	unsigned int screenW;
	unsigned int screenH;
	unsigned char currentScreen;
} ScreenState;

typedef struct {
	float azimuth;
	float elevation;
	float zoom;
} ViewState;

typedef struct {
	float altitude;
	float velocity;
	float area;
	float span;
	float chord;
	float AoA;
	float lift;
	float drag;
	float LD;
} AeroTest;

typedef struct {
	double x;
	double y; 
	double dx;
	double dy;
	double h;
	double V;
	double g;
	double KE;
	double GPE;
	double TE;
	float timer;
} OrbitState;

// aero readouts
// aero performance results relayed to physics
// physics readouts
// aircraft state from physics

void updateViewState(ViewState *view);
void userKeyInput(ScreenState *scr, ViewState *outside, ViewState *orbital, AeroTest *aeroptr);

void drawMenu();
void drawOutside(ViewState *view, char *printViewBuf);
void drawOrbital(ViewState *view, char *printViewBuf, OrbitState *orb);
void drawPanel();
void drawAero();
void drawProp();

void aeroTest(AeroTest *ptr);
void PerformOrbit(OrbitState *ptr, double dt);
