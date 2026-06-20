// Raylib headers
#include "raylib.h"
#include "resource_dir.h"

// C standard library
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "math.h"

// Pearbaby headers
#include "userinput.h"
#include "isa.h"
#include "formulaegg.h"
#undef EGGIN
#undef SHOCKIN
#include "panel.h"

// keyboard input set to default for now 
// (W up S down A left D right)
// W=ascend, S=descend, A=decelerate, D=accelerate

int main() {
	const int sW = 1200;
	const int sH = 800;
	InitWindow(sW, sH, "Pearoplane Alpha v0.1");
	//SetTargetFPS(120);

	char actionSet = 0;
	SetActionsDefault();
	bool releaseAction = false;

	float altitude = 0.0;
	float velocity = 0.0;
	const float DeadSea = -439.8;
	float sensitivity = 1.0;
	float Q;
	float skyR;
	float skyG;
	float skyB;

	char buffer[255];
	struct ISA testISA;
	struct Station testSta;
	
	while (!WindowShouldClose()) {
		// taking user input
		gamepadIndex = 0;
		
		// single button actions
		if (IsActionDown(DEV_RESET)) {
			altitude = 0.0;
			velocity = 0.0;
		}
		
		// set sensitivity
		if (IsActionDown(DEV_FINE)) {
			sensitivity = 0.01;
		} else if (IsActionDown(DEV_COARSE)) {
			sensitivity = 100.0;
		} else {
			sensitivity = 1.0;
		}

		// process actions
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

		// calculating ISA
		testISA.alt = (float)altitude;
		getISA(&testISA);

		// adding to station
		testSta.T = testISA.T;
		testSta.P = testISA.P;
		testSta.rho = testISA.rho;
		testSta.V = velocity;
		getGam(&testSta);
		getMach(&testSta);
		Q = getDynamicP(&testSta);
		
		// adding to the buffers
		sprintf(buffer, "Alt: %.1f m\nV: %.1f m/s\nT: %.1f\u00B0K\nP: %.1fPa\nD: %f kg/m\u00B3\nMach %.3f\ngamma: %.3f\nQ: %.1fkPa",
				altitude, velocity,
				testISA.T, testISA.P,
				testISA.rho,
				testSta.M, testSta.gam, Q);
		char *ptrString = buffer;
		
		// sky colour
		if (altitude >= 80000) {
			skyR = 0;
			skyG = 0;
			skyB = 0;
		} else if (altitude <= 1) {
			skyR = 130;
			skyG = 210;
			skyB = 250;
		} else {
			skyR = 135 * ((-1 * (altitude - 80000)) / 80000);
			skyG = 205 * (-sqrt(altitude / 80000) + 1);
			skyB = 250 * sqrt((altitude - 80000) / -80000);
		}

		// drawing the buffers
		BeginDrawing();
		ClearBackground((Color){skyR, skyG, skyB, 255});
		drawPanel(ptrString);
		EndDrawing();	
	}

	CloseWindow();
	return 0;
}
