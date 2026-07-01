// yeah we gonna need to refactor soon

#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h"
#include <stdlib.h>
#include "main.h"

// default definitions
#define DEFAULT_W 1200
#define DEFAULT_H 800

#include "orbitconfig.h"

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
	SetTargetFPS(250);
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
