// probably rewrite, this has been an absolute disaster
#if 0
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

void boundAngle_PM180(double *angle) {
	if (*angle < -180.0) *angle = 180.0;
	if (*angle > 180.0) *angle = -180.0;
}

void boundAngle_PM360(double *angle) {
	if (*angle < -360.0) *angle = 360.0;
	if (*angle > 360.0) *angle = -360.0;
}

void boundVec3Angles_P360(Vec3 *ptr) {
	boundAngle_P360(&(ptr->x));
	boundAngle_P360(&(ptr->y));
	boundAngle_P360(&(ptr->z));
}

void projectVec3_Vec2(Vec3 *in, Vec2 *out, Vec3 *display) {
	double f = display->z / in->z;
	out->x = (f * display->x) + display->x;
	out->y = (f * display->y) + display->y;
}

void rotateVec3X(Vec3 *in, double *sinx, double *cosx) {
	double oldY = in->y;
	in->y *= *cosx;
	in->y -= in->z * *sinx;
	in->z *= *cosx;
	in->z += oldY * *sinx;
}

void rotateVec3Y(Vec3 *in, double *siny, double *cosy) {
	double oldX = in->x;
	in->x *= *cosy;
	in->x += in->z * *siny;
	in->z = in->y * *cosy - oldX * *siny;
}

void rotateVec3Z(Vec3 *in, double *sinz, double *cosz) {
	double oldX = in->x;
	in->x *= *cosz;
	in->x -= in->y * *sinz;
	in->y *= *cosz;
	in->y += oldX * *sinz;
}


void rotateVec3(Vec3 *in, Vec3 *out, Vec3 *rot) {
	double sinx = sin(rot->x);
	double siny = sin(rot->y);
	double sinz = sin(rot->z);
	double cosx = cos(rot->x);
	double cosy = cos(rot->y);
	double cosz = cos(rot->z);
	out->x = in->x;
	out->y = in->y;
	out->z = in->z;
	rotateVec3X(out, &sinx, &cosx);
	rotateVec3Y(out, &siny, &cosy);
	rotateVec3Z(out, &sinz, &cosz);
}

void UNUSEDflight3D() {
	int screenW = DEFAULT_W;
	int screenH = DEFAULT_H;
	InitWindow(screenW, screenH, "Pearoplane Alpha v0.1");

	float alt = 0.0;
	Color sky;
	char buffer[255];

	Vec3 cubeVerts[8];
	Vec3 cubeRot = DEFAULT_ROT;
	Vec3 cameraPos = (Vec3){0.0, 0.0, 0.0};
	Vec3 cameraRot = DEFAULT_ROT;
	
	cubeVerts[0] = (Vec3){-CUBE_HALFLEN, -CUBE_HALFLEN, -CUBE_HALFLEN}; // rear x4
	cubeVerts[1] = (Vec3){-CUBE_HALFLEN, CUBE_HALFLEN, -CUBE_HALFLEN};
	cubeVerts[2] = (Vec3){CUBE_HALFLEN, -CUBE_HALFLEN, -CUBE_HALFLEN};
	cubeVerts[3] = (Vec3){CUBE_HALFLEN, CUBE_HALFLEN, -CUBE_HALFLEN};
	cubeVerts[4] = (Vec3){-CUBE_HALFLEN, -CUBE_HALFLEN, CUBE_HALFLEN}; // front x4
	cubeVerts[5] = (Vec3){-CUBE_HALFLEN, CUBE_HALFLEN, CUBE_HALFLEN};
	cubeVerts[6] = (Vec3){CUBE_HALFLEN, -CUBE_HALFLEN, CUBE_HALFLEN};
	cubeVerts[7] = (Vec3){CUBE_HALFLEN, CUBE_HALFLEN, CUBE_HALFLEN};

	Line2 cubeLines[12];
	cubeLines[0] = (Line2){0, 1};
	cubeLines[1] = (Line2){1, 3};
	cubeLines[2] = (Line2){2, 3};
	cubeLines[3] = (Line2){0, 3};
	cubeLines[4] = (Line2){4, 5};
	cubeLines[5] = (Line2){5, 7};
	cubeLines[6] = (Line2){4, 6};
	cubeLines[7] = (Line2){6, 7};
	cubeLines[8] = (Line2){0, 7};
	cubeLines[9] = (Line2){1, 6};
	cubeLines[10] = (Line2){2, 5};
	cubeLines[11] = (Line2){3, 4};

	Vec3 rotVerts[8];
	Vec2 projVerts[8];
	Vec3 displayVec = {0.0, 0.0, 2.0};
	double scaling = DEFAULT_W / 4.0;
	double xOffset = DEFAULT_W / 2.0;
	double yOffset = DEFAULT_H / 2.0;

	int v1x;
	int v2x;
	int v1y;
	int v2y;

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
		displayVec.z = expf(logf(displayVec.z) + ((float)GetMouseWheelMove() * 0.01f));
		boundVec3Angles_P360(&cubeRot);

		for (int i = 0; i < 8; i++) {
			rotateVec3(&(cubeVerts[i]), &rotVerts[i], &cubeRot);
			projectVec3_Vec2(&(rotVerts[i]), &(projVerts[i]), &displayVec);
			//printf("%f %f %f\n", (float)rotVerts[i].x, (float)rotVerts[i].y, (float)rotVerts[i].z);
		}	

		getSkyColour(&sky, alt);
		sprintf(buffer, "Altitude = %.1f metres\n\nRotation:\n%.1f\n%.1f\n%.1f\n\nZoom = %f", alt, cubeRot.x, cubeRot.y, cubeRot.z, scaling);

		BeginDrawing();
		ClearBackground(sky);
		
		for (int i = 0; i < 8; i++) {
			v1x = projVerts[i].x;
			v1y = projVerts[i].y;
			printf("projVerts: %f %f\n", (float)v1x, (float)v1y);
			v1x *= scaling;
			v1y *= scaling;
			printf("scaled: %f %f\n", (float)v1x, (float)v1y);
			v1x += xOffset;
			v1y += yOffset;
			printf("offset: %f %f\n\n", (float)v1x, (float)v1y);
			DrawRectangle((int)v1x, (int)v1y, 4, 4, GRAY);
			//printf("%f %f\n", (float)projVerts[i].x, (float)projVerts[i].y);
		}

//		for (int i = 0; i < 12; i++) {
//			v1x = (projVerts[cubeLines[i].v1].x * scaling) + xOffset;
//			v1y = (projVerts[cubeLines[i].v1].y * scaling) + yOffset;
//			v2x = (projVerts[cubeLines[i].v2].x * scaling) + xOffset;
//			v2y = (projVerts[cubeLines[i].v2].y * scaling) + yOffset;				DrawLine((int)v1x, (int)v1y, (int)v2x, (int)v2y, BLACK);
//			printf("%f %f\n%f %f\n\n", v1x, v1y, v2x, v2y);
//			// need to do offset stuff			
//		}

		DrawFPS(10, 10);
		DrawText(buffer, 10, 40, 20, WHITE);
		EndDrawing();
	}

	CloseWindow();
}
#endif
