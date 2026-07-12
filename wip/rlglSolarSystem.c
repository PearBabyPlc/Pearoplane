// not my code for the most part
// from the Raylib examples:
// https://www.raylib.com/examples/models/loader.html?name=models_rlgl_solar_system

#include "raylib.h"
#include "rlgl.h"
#include <math.h>
#include <stdio.h>

#define SCREEN_W 1200
#define SCREEN_H 800
#define SUN_RADIUS 5.0f
#define EARTH_RADIUS 0.5f
#define MOON_RADIUS 0.1f
#define EARTH_SMA 25.0f
#define MOON_SMA 2.5f
#define EMPTY_VEC3 (Vector3){0.0f, 0.0f, 0.0f}

void DrawSphereBasic(Color color) {
	int rings = 64;
	int slices = 64;

	rlCheckRenderBatchLimit((rings + 2)*slices*6);

	rlBegin(RL_TRIANGLES);
		rlColor4ub(color.r, color.g, color.b, color.a);

		for (int i = 0; i < (rings + 2); i++) {
			for (int j = 0; j < slices; j++) {
				rlVertex3f(cosf(DEG2RAD*(270+(180.0f/(rings+1))*i))*sinf(DEG2RAD*(j*360.0f/slices)),
						sinf(DEG2RAD*(270+(180.0f/(rings+1))*i)),
						cosf(DEG2RAD*(270+(180.0f/(rings+1))*i))*cosf(DEG2RAD*(j*360.0f/slices)));
				rlVertex3f(cosf(DEG2RAD*(270+(180.0f/(rings+1))*(i+1)))*sinf(DEG2RAD*((j+1)*360.0f/slices)),
						sinf(DEG2RAD*(270+(180.0f/(rings+1))*(i+1))),
						cosf(DEG2RAD*(270+(180.0f/(rings+1))*(i+1)))*cosf(DEG2RAD*((j+1)*360.0f/slices)));
				rlVertex3f(cosf(DEG2RAD*(270+(180.0f/(rings+1))*(i+1)))*sinf(DEG2RAD*(j*360.0f/slices)),
						sinf(DEG2RAD*(270+(180.0f/(rings+1))*(i+1))),
						cosf(DEG2RAD*(270+(180.0f/(rings+1))*(i+1)))*cosf(DEG2RAD*(j*360.0f/slices)));
				
				rlVertex3f(cosf(DEG2RAD*(270+(180.0f/(rings+1))*i))*sinf(DEG2RAD*(j*360.0f/slices)),
						sinf(DEG2RAD*(270+(180.0f/(rings+1))*i)),
						cosf(DEG2RAD*(270+(180.0f/(rings+1))*i))*cosf(DEG2RAD*(j*360.0f/slices)));

				rlVertex3f(cosf(DEG2RAD*(270+(180.0f/(rings+1))*i))*sinf(DEG2RAD*((j+1)*360.0f/slices)),
						sinf(DEG2RAD*(270+(180.0f/(rings+1))*i)),
						cosf(DEG2RAD*(270+(180.0f/(rings+1))*i))*cosf(DEG2RAD*((j+1)*360.0f/slices)));

				rlVertex3f(cosf(DEG2RAD*(270+(180.0f/(rings+1))*(i+1)))*sinf(DEG2RAD*((j+1)*360.0f/slices)),
						sinf(DEG2RAD*(270+(180.0f/(rings+1))*(i+1))),
						cosf(DEG2RAD*(270+(180.0f/(rings+1))*(i+1)))*cosf(DEG2RAD*((j+1)*360.0f/slices)));
			}
		}
	rlEnd();
}

int main() {
	InitWindow(SCREEN_W, SCREEN_H, "rlgl solar system");

	Camera camera = {0};
	camera.position = (Vector3){32.0f, 16.0f, 32.0f};
	//camera.target = (Vector3){0.0f, 0.0f, 0.0f};
	camera.up = (Vector3){0.0f, 1.0f, 0.0f};
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	float rotSpeed = 0.2f;
	float rotEarth = 0.0f;
	float rotEarthOrbit = 0.0f;
	float rotMoon = 0.0f;
	float rotMoonOrbit = 0.0f;
	Vector3 posSun = EMPTY_VEC3;
	Vector3 posEarth = EMPTY_VEC3;
	Vector3 posMoon = EMPTY_VEC3;

	Vector3 XYZorigin = (Vector3){12.0f, 2.0f, 12.0f};
	Vector3 Xline = (Vector3){15.0f, 2.0f, 12.0f};
	Vector3 Yline = (Vector3){12.0f, 5.0f, 12.0f};
	Vector3 Zline = (Vector3){12.0f, 2.0f, 15.0f};

	float frameTime = 0.1f;
	unsigned char isPaused = 0;
	unsigned char camTarg = 0;

	char camTargBuf[255];

	//SetTargetFPS(6);
	while (!WindowShouldClose()) {
		frameTime = GetFrameTime() * 20.0f * (float)isPaused;
		rotEarth += (5.0f * rotSpeed * frameTime);
		rotEarthOrbit += (365/360.0f * (5.0f * rotSpeed * frameTime) * rotSpeed * frameTime);
		rotMoon += (2.0f * rotSpeed * frameTime);
		rotMoonOrbit += (8.0f * rotSpeed * frameTime);
		posEarth = (Vector3){EARTH_SMA * -cosf((180+rotEarthOrbit)*DEG2RAD), 0.0f, EARTH_SMA * sinf((180+rotEarthOrbit)*DEG2RAD)};
		posMoon = (Vector3){MOON_SMA * -cosf((180+rotMoonOrbit)*DEG2RAD) + posEarth.x, 0.0f, MOON_SMA * sinf((180+rotMoonOrbit)*DEG2RAD) + posEarth.z};
		sprintf(camTargBuf, "Earth x=%.1f y=%.1fm z=%.1f\nMoon x=%.1f y=%.1f z=%.1f", posEarth.x, posEarth.y, posEarth.z, posMoon.x, posMoon.y, posMoon.z);

		UpdateCamera(&camera, CAMERA_THIRD_PERSON);
		if (IsKeyPressed(KEY_RIGHT_BRACKET)) camTarg++;
		if (IsKeyPressed(KEY_LEFT_BRACKET)) {
			if (camTarg == 0) camTarg = 2;
			else camTarg--;
		}
		if (camTarg > 2) camTarg = 0;
		if (camTarg == 0) camera.target = posSun;
		if (camTarg == 1) camera.target = posEarth;
		if (camTarg == 2) camera.target = posMoon;
		if (IsKeyPressed(KEY_SPACE)) {
			if (isPaused == 0) isPaused = 1;
			else if (isPaused == 1) isPaused = 0;
		}
		
		BeginDrawing();
		ClearBackground(BLACK);
		BeginMode3D(camera);

		rlPushMatrix();
			rlScalef(SUN_RADIUS, SUN_RADIUS, SUN_RADIUS);
			DrawSphereBasic(GOLD);
		rlPopMatrix();
		rlPushMatrix();
			rlRotatef(rotEarthOrbit, 0.0f, 1.0f, 0.0f);
			rlTranslatef(EARTH_SMA, 0.0f, 0.0f);
			rlPushMatrix();
				rlRotatef(rotEarth, 0.25, 1.0, 0.0);
				rlScalef(EARTH_RADIUS, EARTH_RADIUS, EARTH_RADIUS);
				DrawSphereBasic(BLUE);
			rlPopMatrix();
			rlRotatef(rotMoonOrbit, 0.0f, 1.0f, 0.0f);
			rlTranslatef(MOON_SMA, 0.0f, 0.0f);
			rlRotatef(rotMoon, 0.0f, 1.0f, 0.0f);
			rlScalef(MOON_RADIUS, MOON_RADIUS, MOON_RADIUS);
			DrawSphereBasic(LIGHTGRAY);
		rlPopMatrix();

		DrawLine3D(XYZorigin, Xline, GREEN);
		DrawLine3D(XYZorigin, Yline, GREEN);
		DrawLine3D(XYZorigin, Zline, GREEN);
		DrawCircle3D(EMPTY_VEC3, EARTH_SMA, (Vector3){1, 0, 0}, 90.0f, Fade(RED, 0.5f));
		DrawGrid(10, 10.0f);
		EndMode3D();
		DrawText(camTargBuf, 10, 40, 20, WHITE);
		DrawFPS(10, 10);
		//frameTime = GetFrameTime();
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
