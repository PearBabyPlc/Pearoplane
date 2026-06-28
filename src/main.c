#include "raylib.h"
#include "resource_dir.h"
#include "main.h"

// default definitions
#define DEFAULT_W 1200
#define DEFAULT_H 800

int main() {
	ScreenState screenState = {.screenW = DEFAULT_W, .screenH = DEFAULT_H, .currentScreen = 0};
	ViewState outsideView = {.azimuth = 0.0, .elevation = 0.0, .zoom = 1.0};
	ViewState orbitalView = {.azimuth = 0.0, .elevation = 0.0, .zoom = 1.0};
	int keyPressed = -1;

	InitWindow(DEFAULT_W, DEFAULT_H, "Pearoplane Alpha v0.1");
	//SetTargetFPS(1200);
	while (!WindowShouldClose()) {
		// user input
		keyPressed = GetKeyPressed();
		if (keyPressed != -1) userKeyInput(&screenState, &outsideView, &orbitalView);
		keyPressed = -1;

		// aero model that will only update when necessary to improve performance

		// physics model that will update at a strictly fixed timestep for accuracy

		// draw screen depending on screenState
		BeginDrawing();
		if (screenState.currentScreen == 0) drawMenu();
		if (screenState.currentScreen == 1) drawOutside(&outsideView);
		if (screenState.currentScreen == 2) drawOrbital(&orbitalView);
		if (screenState.currentScreen == 3) drawPanel();
		if (screenState.currentScreen == 4) drawAero();
		if (screenState.currentScreen == 5) drawProp();
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
