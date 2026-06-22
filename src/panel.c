#include "raylib.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "helper.h"
#include "formulaegg.h"
#include "panel.h"

void bufferStation(ucBuf *buf, Station *sta) {
	sprintf(*buf, "M: %.3f\nP: %.1f\nT: %.1f\nrho: %.5f\ngam: %.3f\nCp: %.1f\nPt: %.1f\nTt: %.1f\nV: %.1f\nA: %.3f\nmdot: %.1f\nflow: %d\npos: %d",
			sta->M, sta->P, sta->T, sta->rho, sta->gam, sta->Cp, sta->Pt, sta->Tt, sta->V, sta->A, sta->mdot, sta->flow, sta->pos);
}

void drawPanel(ucBuf *ptrString, ucBuf *ptrStringB, ucBuf *ptrN) {
	DrawText(*ptrString, 200, 80, 30, (Color){245, 255, 245, 255});
	DrawText(*ptrStringB, 500, 80, 30, (Color){245, 255, 245, 255});
	DrawText(*ptrN, 800, 80, 30, (Color){245, 255, 245, 255});
	DrawText("Ambient", 200, 40, 40, WHITE);
	DrawText("Compress", 500, 40, 40, WHITE);
	DrawText("Expand", 800, 40, 40, WHITE);
	DrawText("shock deflection angle of 10\u00B0\narrow keys for altitude/velocity\n[tab] to reset\nZ for fine control\nX for coarse", 50, 600, 25, (Color){255, 225, 225, 255});
	DrawFPS(10, 10);
}

// not at all based on physics, just looks kinda right
void getSkyRGB(float *skyR, float *skyG, float *skyB, float altitude) {
	if (altitude >= 80000) {
		*skyR = 0;
		*skyG = 0;
		*skyB = 0;
	} else if (altitude <= 0) {
		*skyR = 130;
		*skyG = 210;
		*skyB = 250;
	} else {
		*skyR = 135 * ((-1 * (altitude - 80000)) / 80000);
		*skyG = 205 * (-sqrt(altitude / 80000) + 1);
		*skyB = 250 * sqrt((altitude - 80000) / -80000);
	}
}

