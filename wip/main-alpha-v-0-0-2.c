// right proper dogs breakfast getting it all in one file lol
#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// main.h
#define DEFAULT_W 1200
#define DEFAULT_H 800
#define SCALE 0.00003
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

// userinput.c
#define DEFAULT_ANGLE_STEP 0.05
#define DEFAULT_ZOOM_STEP 0.5
#define MAX_ZOOM 100000.0
#define MIN_ZOOM 0.01

void updateViewState(ViewState *view) {
	if (IsKeyPressed(KEY_TAB)) {
		view->elevation = 0.0;
		view->azimuth = 0.0;
		view->zoom = 1.0;
	} else {
		if (IsKeyDown(KEY_UP)) view->elevation += DEFAULT_ANGLE_STEP;
		if (IsKeyDown(KEY_DOWN)) view->elevation -= DEFAULT_ANGLE_STEP;
		if (IsKeyDown(KEY_LEFT)) view->azimuth -= DEFAULT_ANGLE_STEP;
		if (IsKeyDown(KEY_RIGHT)) view->azimuth += DEFAULT_ANGLE_STEP;
		if (IsKeyDown(KEY_COMMA)) view->zoom -= DEFAULT_ZOOM_STEP;
		if (IsKeyDown(KEY_PERIOD)) view->zoom += DEFAULT_ZOOM_STEP;

		if (view->elevation <= -180.0) view->elevation += 360.0;
		if (view->elevation >= 180.0) view->elevation -= 360.0;
		if (view->azimuth <= -180.0) view->azimuth += 360.0;
		if (view->azimuth >= 180.0) view->azimuth -= 360.0;
		if (view->zoom > MAX_ZOOM) view->zoom = MAX_ZOOM;
		if (view->zoom < MIN_ZOOM) view->zoom = MIN_ZOOM;
	}
}

void updateAeroTest(AeroTest *ptr) {
	if (IsKeyPressed(KEY_TAB)) {
		ptr->altitude = 0.0;
		ptr->velocity = 0.0;
		ptr->AoA = 0.0;
	} 

	if (IsKeyDown(KEY_UP)) ptr->altitude += 0.5;
	if (IsKeyDown(KEY_DOWN)) ptr->altitude -= 0.5;
	if (IsKeyDown(KEY_LEFT)) ptr->velocity -= 0.01;
	if (IsKeyDown(KEY_RIGHT)) ptr->velocity += 0.01;
	if (IsKeyDown(KEY_W)) ptr->AoA -= 0.001;
	if (IsKeyDown(KEY_S)) ptr->AoA += 0.001;
	if (ptr->altitude < 0.0) ptr->altitude = 0.0;
	if (ptr->velocity < 0.0) ptr->velocity = 0.0;
}

void userKeyInput(ScreenState *scr, ViewState *outside, ViewState *orbital, AeroTest *aeroptr) {
	if (IsKeyPressed(KEY_ZERO)) scr->currentScreen = 0;
	if (IsKeyPressed(KEY_ONE)) scr->currentScreen = 1;
	if (IsKeyPressed(KEY_TWO)) scr->currentScreen = 2;
	if (IsKeyPressed(KEY_THREE)) scr->currentScreen = 3;
	if (IsKeyPressed(KEY_FOUR)) scr->currentScreen = 4;
	if (IsKeyPressed(KEY_FIVE)) scr->currentScreen = 5;

	if (scr->currentScreen == 1) {
		updateViewState(outside);
	}
	if (scr->currentScreen == 2) {
		updateViewState(orbital);
	}

	if (scr->currentScreen ==4) {
		updateAeroTest(aeroptr);
	}
}

// screens.c
void drawMenu() {
	ClearBackground(LIME);
	DrawText("Pearoplane Alpha v0.1 (Menu)", 50, 30, 50, WHITE);
}

void drawOutside(ViewState *view, char *printViewBuf) {
	ClearBackground(BLUE);
	DrawFPS(10, 10);
	//char viewBuf[VIEW_BUF_SIZE];
	sprintf(printViewBuf, "azimuth: %.3f\u00B0\nelevation: %.3f\u00B0\nzoom: %.3f", view->azimuth, view->elevation, view->zoom);
	DrawText(printViewBuf, 10, 40, 20, RAYWHITE);
	DrawText("View: Outside", 980, 20, 30, RAYWHITE);
}

#define R_EARTH 6356766.0
void drawOrbital(ViewState *view, char *printViewBuf, OrbitState *ptr) {
	// various prints
	ClearBackground(BLACK);
	DrawFPS(10, 10);
	//char viewBuf[VIEW_BUF_SIZE];
	sprintf(printViewBuf, "azimuth: %.3f\u00B0\nelevation: %.3f\u00B0\ntimewarp: %.3fx", view->azimuth, view->elevation, view->zoom);
	DrawText(printViewBuf, 10, 40, 20, RAYWHITE);
	DrawText("View: Orbital", 980, 20, 30, RAYWHITE);
	char orbBuf[100];
	sprintf(orbBuf, "x=%.3f\ny=%.3f\nh=%.3fkm\nV=%.3fm/s\ng=%.6fm/s^2", ptr->x/1000, ptr->y/1000, ptr->h/1000, ptr->V, ptr->g);
	DrawText(orbBuf, 10, 670, 20, WHITE);
	char energyBuf[100];
	sprintf(energyBuf, "kinetic=%f\ngravpot=%f\ntotal=%f", ptr->KE, ptr->GPE, ptr->TE);
	DrawText(energyBuf, 900, 600, 20, WHITE);
	char timerBuf[32];
	int hours = ptr->timer / 3600;
	int minutes = ((int)ptr->timer % 3600) / 60;
	float seconds = ptr->timer - ((hours * 3600) + (minutes * 60));
	sprintf(timerBuf, "T+%dh%dm%.1fs", hours, minutes, seconds);
	DrawText(timerBuf, 250, 20, 40, RAYWHITE);

	// draw ship and earth
	DrawCircle(600, 400, R_EARTH*SCALE, SKYBLUE);
	float shipX = (ptr->x * SCALE) + 600;
	float shipY = -(ptr->y * SCALE) + 400;
	DrawRectangle((int)shipX, (int)shipY, 8, 8, (Color){255, 255, 255, 150});
}

void drawPanel() {
	ClearBackground(GRAY);
	DrawText("Panel: Main", 1075, 15, 20, RAYWHITE);

}

// needed to declare aeroTest here first
void aeroTest(AeroTest *ptr);
void drawAero(AeroTest *ptr) {
	ClearBackground(SKYBLUE);
	DrawFPS(10, 10);
	DrawText("Panel: Aerodynamics", 980, 15, 20, RAYWHITE);
	
	// altitude velocity area span chord AoA lift drag LD
	aeroTest(ptr);
	
	char viewBuf[255];
	sprintf(viewBuf, "alt %.1f metres\nvel %.1f m/s\narea %.1f m2\nspan %.1f\nchord %.1f m\nAoA %.1f\u00B0\nlift %.1f tons\ndrag %.1f kN\nLD %.3f", ptr->altitude, ptr->velocity, ptr->area, ptr->span, ptr->chord, ptr->AoA, ptr->lift, ptr->drag, ptr->LD);
	DrawText(viewBuf, 100, 50, 20, WHITE);
}

void drawProp() {
	ClearBackground(RED);
	DrawText("Panel: Propulsion", 1010, 15, 20, RAYWHITE);
}

// aero.h
typedef struct {
	float alt;
	float H;
	float T;
	float P;
	float rho;
	//float RH;
} ISA;

// isa.c
const float rEarth = 6356766;
const float g = 9.80665;
const float molISA = 28.9644; //      these fuckin constants
const float RISA = 8314.46; //        kept conflicting with
const float RsISA = RISA / molISA; // formulaegg.c from attempt-one, so renamed

void getISA(ISA *ptr) {
	ptr->H = (rEarth * ptr->alt) / (rEarth + ptr->alt);

	if (ptr->H < 11000) {
		// troposphere, positive lapse rate (-temp with altitude)
		ptr->T = 288.15 + (-0.0065 * ptr->H);
		double Pexp = (g * molISA) / (RISA * -0.0065);
		double Pfrac = 288.15 / ptr->T;
		ptr->P = 101325.0 * pow(Pfrac, Pexp);
		// refT = 288.15
		// refP = 101325
		// refH = 0
		// refL = -0.0065	
	} else if (ptr->H >= 11000 && ptr->H < 20000) {
		// tropopause, constant temperature (zero lapse rate)
		ptr->T = 216.65;
		double Pexp = (-g * molISA * (ptr->H - 11000)) / (RISA * 216.65);
		ptr->P = 22632.6352091 * exp(Pexp);
		// refT = 216.65;
		// refP = 22632.6
		// refH = 11000
		// refL = ±0
	} else if (ptr->H >= 20000 && ptr->H < 32000) {
		// lower stratosphere, negative lapse rate (+temp with altitude)
		ptr->T = 216.65 + (0.001 * (ptr->H - 20000));
		double Pexp = (g * molISA) / (RISA * 0.001);
		double Pfrac = 216.65 / ptr->T;
		ptr->P = 5475.15769257 * pow(Pfrac, Pexp);
		// refT = 216.65
		// refP = 5475.16
		// refH = 20000
		// refL = +0.001
	} else if (ptr->H >= 32000 && ptr->H < 47000) {
		// upper stratosphere, negative lapse rate
		ptr->T = 228.65 + (0.0028 * (ptr->H - 32000));
		double Pexp = (g * molISA) / (RISA * 0.0028);
		double Pfrac = 228.65 / ptr->T;
		ptr->P = 868.088257003 * pow(Pfrac, Pexp);
		// refT = 228.65
		// refP = 868.088
		// refH = 32000
		// refL = +0.0028
	} else if (ptr->H >= 47000 && ptr->H < 51000) {
		// stratopause, constant temperature (zero lapse rate)
		ptr->T = 270.65;
		double Pexp = (-g * molISA * (ptr->H - 47000)) / (RISA * 270.65);
		ptr->P = 110.919037481 * exp(Pexp);
		// refT = 270.65
		// refP = 110.919
		// refH = 47000
		// refL = ±0
	} else if (ptr->H >= 51000 && ptr->H < 71000) {
		// lower mesosphere, positive lapse rate
		ptr->T = 270.65 + (-0.0028 * (ptr->H - 51000));
		double Pexp = (g * molISA) / (RISA * -0.0028);
		double Pfrac = 270.65 / ptr->T;
		ptr->P = 66.9471267903 * pow(Pfrac, Pexp);
		// refT = 270.65
		// refP = 66.9471
		// refH = 51000
		// refL = -0.0028
	} else if (ptr->H >= 71000 && ptr->H < 84852) {
		// upper mesosphere, positive lapse rate
		ptr->T = 214.65 + (-0.002 * (ptr->H - 71000));
		double Pexp = (g * molISA) / (RISA * -0.002);
		double Pfrac = 214.65 / ptr->T;
		ptr->P = 3.95709671594 * pow(Pfrac, Pexp);
		// refT = 214.65
		// refP = 3.9570967
		// refH = 71000
		// refL = -0.002
	} else if (ptr->H >= 84852 && ptr->H < 90000) {
		// mesopause, constant temperature
		ptr->T = 186.95;
		double Pexp = (-g * molISA * (ptr->H - 84852)) / (RISA * 186.95);
		ptr->P = 0.373462257454 * exp(Pexp);
		// refT = 186.95
		// refP = 0.373462257
		// refH = 84852
		// refL = ±0
	} else if (ptr->H >= 90000) {
		// thermosphere, negative lapse rate
		ptr->T = 186.95 + (0.004 * (ptr->H - 90000));
		double Pexp = (g * molISA) / (RISA * 0.004);
		double Pfrac = 186.95 / ptr->T;
		ptr->P = 0.145778536657 * pow(Pfrac, Pexp);
		// refT = 186.95
		// refP = 0.145778536657
		// refH = 90000
		// refL = +0.004
	}
	
	if (ptr->alt > 307446) {
		// set temp to constant 1000K, and P/rho to zero
		ptr->T = 1000.0; // later idk maybe implement solar radiation
		ptr->P = 0.0;
		ptr->rho = 0.0;
	} else {
		// otherwise calculate rho normally
		ptr->rho = ptr->P / (ptr->T * RsISA);
	}
}

// liftdrag.c
#ifndef PI
#define PI 3.14159265359
#endif

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

// physics.h never needed to be used lol

// orbit.c
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

// orbitconfig.h
#define ORBX 6956766.0
#define ORBY 0.1
#define ORBDX -0.1
#define ORBDY -8500.0

int main() {
	// initialise states
	ScreenState screenState = {.screenW = DEFAULT_W, .screenH = DEFAULT_H, .currentScreen = 0};
	ViewState outsideView = {.azimuth = 0.0, .elevation = 0.0, .zoom = 1.0};
	ViewState orbitalView = {.azimuth = 0.0, .elevation = 0.0, .zoom = 1.0};

	OrbitState orbit = {.x = ORBX, .y = ORBY, .dx = ORBDX, .dy = ORBDY, .h = -1.0, .V = 7800.0, .g = 9.8, .timer = 0.0};

	AeroTest wings = {.altitude = 0.0, .velocity = 0.0, .area = 2100.0, .span = 50.0, .chord = 78.0, .AoA = 0.0, .lift = -1.0, .drag = -1.0, .LD = -1.0};

	// malloc for unimportant stuff like char buf[] arrays for deborging
	char *printViewBuffer = malloc(70);

	// Raylib window
	InitWindow(DEFAULT_W, DEFAULT_H, "Pearoplane Alpha v0.1");
	//SetTargetFPS(250);
	int keyPressed = -1;
	while (!WindowShouldClose()) {
		// user input
		keyPressed = GetKeyPressed();
		if (keyPressed != -1) userKeyInput(&screenState, &outsideView, &orbitalView, &wings);
		keyPressed = -1;

		// aero model that will only update when necessary to improve performance

		// physics model that will update at a strictly fixed timestep for accuracy
		double dt = GetFrameTime();
		double dtWarped = dt * orbitalView.zoom;
		PerformOrbit(&orbit, dtWarped);

		// draw screen depending on screenState
		BeginDrawing();
		if (screenState.currentScreen == 0) drawMenu();
		if (screenState.currentScreen == 1) drawOutside(&outsideView, printViewBuffer);
		if (screenState.currentScreen == 2) drawOrbital(&orbitalView, printViewBuffer, &orbit);
		if (screenState.currentScreen == 3) drawPanel();
		if (screenState.currentScreen == 4) drawAero(&wings);
		if (screenState.currentScreen == 5) drawProp();
		EndDrawing();
	}
	CloseWindow();

	free(printViewBuffer);
	return 0;
}
