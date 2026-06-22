// Raylib headers
#include "raylib.h"
#include "resource_dir.h"

// C standard library
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "math.h"

// Pearbaby headers
#include "helper.h"
#include "userinput.h"
#include "isa.h"
#include "formulaegg.h"
#undef STAT_IN
#undef SHOCK_IN
#undef EXFAN_IN
#undef COMB_IN
#include "panel.h"

// this current version of main.c is very barebones and doesn't display anything
// TODO: design panel, Shomate equation+incorporate into formulaegg, aircraft elements+physics model, orbit, 3d, et al.
const int sW = 1200;
const int sH = 800;
const float DeadSea = -439.8; // lowest elevation on Earth

int main() {
	// setup raylib window
	InitWindow(sW, sH, "Pearoplane Alpha v0.1");
	//SetTargetFPS(120);
	char actionSet = 0;
	SetActionsDefault();
	bool releaseAction = false;
	float skyR;
	float skyG;
	float skyB;

	// setup debug controls
	float altitude = 1.0;
	float velocity = 0.0;
	float sensitivity = 1.0;

	// test
	ucBuf example;
	printf("\n==========\nucBuf size = %zu bytes\n==========\n", sizeof(example));
		
	while (!WindowShouldClose()) {
		// taking user input
		gamepadIndex = 0;
		if (IsActionDown(DEV_RESET)) {
			altitude = 0.0;
			velocity = 0.0;
		}
		if (IsActionDown(DEV_FINE)) {
			sensitivity = 0.01;
		} else if (IsActionDown(DEV_COARSE)) {
			sensitivity = 100.0;
		} else {
			sensitivity = 1.0;
		}
		if (IsActionDown(DEV_ASCEND)) altitude += 1.0 * sensitivity;
		if (IsActionDown(DEV_DESCEND)) {
			if (altitude <= DeadSea) {
				altitude = DeadSea;
			} else {
				altitude -= 1.0 * sensitivity;
			}
		}
		if (IsActionDown(DEV_DECEL)) {
			if (velocity <= 0.1) {
				velocity = 0.0;
			} else {
				velocity -= 0.5 * sensitivity;
			}
		}
		if (IsActionDown(DEV_ACCEL)) velocity += 0.5 * sensitivity;
		releaseAction = false;
		
		// program goes here, i deleted the shock/expansion/combustor demo for now
		// debug terminal experiment here
		
		// drawing the buffers
		BeginDrawing();
		getSkyRGB(&skyR, &skyG, &skyB, altitude);
		ClearBackground((Color){skyR, skyG, skyB, 255});
		//drawPanel(&buffer, &bufferInlet, &bufferComb);
		DrawFPS(10, 10);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
