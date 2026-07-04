// 3d rendering experiment, TODO basic flight model (way down the line)

// Raylib headers
#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h"

// C standard library
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Pearbaby headers
#include "main.h"

// flight definitions only
#define FLIGHT_ALT_STEP 10.0
#define FLIGHT_ROT_STEP 0.01
#define DEFAULT_ROT (Vec3){0.0, 0.0, 0.0}
#define CUBE_HALFLEN 200.0
#ifndef PI
#define PI 3.14159265358979323846
#endif

void getSkyColour(Color *sky, float alt) {
	if (alt >= 80000) {
		*sky = (Color){0, 0, 0, 255};
	} else if (alt <= 0) {
		*sky = (Color){130, 210, 250, 255};
	} else {
		*sky = (Color){135 * ((-1 * (alt - 80000)) / 80000),
			205 * (-sqrt(alt / 80000) + 1),
			250 * sqrt((alt - 80000) / -80000),
			255};
	}
}

void boundAngle_P360(double *angle) {
	if (*angle < 0.0) *angle = 360.0;
	if (*angle > 360.0) *angle = 0.0;
}

void boundVec3Angles_P360(Vec3 *ptr) {
	boundAngle_P360(&(ptr->x));
	boundAngle_P360(&(ptr->y));
	boundAngle_P360(&(ptr->z));
}

void flight3D() {
	int screenW = DEFAULT_W;
	int screenH = DEFAULT_H;
	InitWindow(screenW, screenH, "Pearoplane Alpha v0.1");

	float alt = 0.0;
	Color sky;
	char buffer[255];

	Vec3 cubeRot = DEFAULT_ROT;
	Vec3 cameraRot = DEFAULT_ROT;

	//SetTargetFPS(10);
	while (!WindowShouldClose()) {
		if (IsKeyPressed(KEY_TAB)) {
			alt = 0.0;
			cubeRot = DEFAULT_ROT;
			cameraRot = DEFAULT_ROT;
		}
		if (IsKeyDown(KEY_UP)) alt += FLIGHT_ALT_STEP;
		if (IsKeyDown(KEY_DOWN)) alt -= FLIGHT_ALT_STEP;
		if (IsKeyDown(KEY_A)) cubeRot.x -= FLIGHT_ROT_STEP;
		if (IsKeyDown(KEY_D)) cubeRot.x += FLIGHT_ROT_STEP;
		if (IsKeyDown(KEY_W)) cubeRot.y -= FLIGHT_ROT_STEP;
		if (IsKeyDown(KEY_S)) cubeRot.y += FLIGHT_ROT_STEP;
		if (IsKeyDown(KEY_Q)) cubeRot.z -= FLIGHT_ROT_STEP;
		if (IsKeyDown(KEY_E)) cubeRot.z += FLIGHT_ROT_STEP;
		//displayVec.z = expf(logf(displayVec.z) + ((float)GetMouseWheelMove() * 0.01f));
		boundVec3Angles_P360(&cubeRot);

		getSkyColour(&sky, alt);
		BeginDrawing();
		ClearBackground(sky);
			DrawFPS(10, 10);
		EndDrawing();
	}

	CloseWindow();
}
