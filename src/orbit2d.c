// orbit 2d
// TODO sun and moon idk, probably actual rocket controls
// a 3d version definitely lol

// Raylib headers
#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h"

// C standard library
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// header for linkage
#include "main.h"

// orbit initialisation
#define ORBX 6956766.0
#define ORBY 0.1
#define ORBZ -10000.0
#define ORBDX -0.1
#define ORBDY -8500.0
#define ORBDZ 100.0
#define ORBINIT {.x = ORBX, .y = ORBY, .dx = ORBDX, .dy = ORBDY, .g = 9.8, .Hkm = -69.0f, .Vms = 420.0f, .timer = 0.0f}

// just doing 2d for now
#undef ORBZ
#undef ORBDZ

double g_at_r(double r) {
	return (G_UNIV * M_EARTH) / (r * r);
}

void PerformOrbit(OrbitState *ptr, double dt) {
	double r = sqrt((ptr->x * ptr->x) + (ptr->y * ptr->y));
	ptr->Hkm = (r - R_EARTH) * 0.001;
	ptr->Vms = sqrt((ptr->dx * ptr->dx) + (ptr->dy * ptr->dy));
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
	ptr->timer += (float)dt;
}

void orbit2D() {
	InitWindow(DEFAULT_W, DEFAULT_H, "Pearoplane Alpha v0.1");
	int screenW = DEFAULT_W;
	int screenH = DEFAULT_H;
	
	Vec2 camera = {.x = 0.0, .y = 0.0};
	float shipZoom = 1.0;
	float mapZoom = 1.0f;
	unsigned char camTarget = 1;

	OrbitState orbit = ORBINIT;
	double dt;
	double scale;
	char buffer[500];
	float shipX;
	float shipY;
	float earthX;
	float earthY;
	const float warps[] = {0.1, 0.2, 0.5, 1.0, 2.0, 5.0, 10.0, 50.0, 100.0, 1000.0, 10000.0, 100000.0}; // unstable beyond 100kx
	int warp = 3;

	while (!WindowShouldClose()) {
		if (IsKeyPressed(KEY_M)) {
			if (camTarget == 0) camTarget = 1;
			else camTarget = 0;
		}
		if (IsKeyPressed(KEY_COMMA)) warp -= 1;
		if (IsKeyPressed(KEY_PERIOD)) warp += 1;
		if (warp > 11) warp = 11;
		if (warp < 0) warp = 0;

		dt = (double)GetFrameTime()*warps[warp];
		PerformOrbit(&orbit, dt);

		BeginDrawing();
		ClearBackground(BLACK);

		if (camTarget == 0) {
			shipZoom = expf(logf(shipZoom + ((float)GetMouseWheelMove() * 0.01)));
			if (shipZoom < 0.01) shipZoom = 0.01;
			if (shipZoom > 1000.0) shipZoom = 100.0;
			if (isnan(shipZoom)) shipZoom = 0.01;
			if (IsKeyPressed(KEY_TAB)) {
				shipZoom = 1.0f;
				warp = 3;
			}

			camera.x = orbit.x;
			camera.y = orbit.y;

			scale = shipZoom * ZOOM_SHIP;
			shipX = screenW / 2.0f;
			shipY = screenH / 2.0f;

			earthX = -(orbit.x * scale) + shipX;
			earthY = -(orbit.y * scale) + shipY;

			sprintf(buffer, "Map view\nAltitude: %.1fkm\nVelocity: %.1fm/s\nGravity: %.5fm/s^2\nzoom=%f\nwarp=%f", orbit.Hkm, orbit.Vms, orbit.g, shipZoom, warps[warp]);
		} else if (camTarget == 1) {
			mapZoom = expf(logf(mapZoom + ((float)GetMouseWheelMove() * 0.01)));
			if (mapZoom < 0.01) mapZoom = 0.01;
			if (mapZoom > 1000.0) mapZoom = 100.0;
			if (isnan(mapZoom)) mapZoom = 0.01;
			if (IsKeyPressed(KEY_TAB)) {
				mapZoom = 1.0f;
				warp = 3;
			}


			camera.x = 0.0;
			camera.y = 0.0;

			scale = mapZoom * ZOOM_MAP;
			earthX = screenW / 2.0f;
			earthY = screenH / 2.0f;

			shipX = (orbit.x * scale) + earthX;
			shipY = (orbit.y * scale) + earthY;

			sprintf(buffer, "Map view\nAltitude: %.1fkm\nVelocity: %.1fm/s\nGravity: %.5fm/s^2\nzoom=%f\nwarp=%f", orbit.Hkm, orbit.Vms, orbit.g, mapZoom, warps[warp]);
		}

		DrawCircle((int)earthX, (int)earthY, R_EARTH*scale, SKYBLUE);
		DrawRectangle((int)shipX, (int)shipY, 4, 4, GRAY);
		DrawFPS(10, 10);
		DrawText(buffer, 10, 40, 20, WHITE);

		EndDrawing();
	}

	CloseWindow();
}

