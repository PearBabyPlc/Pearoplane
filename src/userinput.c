#include "raylib.h"
#include "resource_dir.h"
#include "main.h"

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

