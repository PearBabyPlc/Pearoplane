#include "raylib.h"
#include "stdio.h"
#include "formulaegg.h"
#include "panel.h"

void bufferStation(ucBuf *buf, Station *sta) {
	sprintf(*buf, "M: %.3f\nP: %.1f\nT: %.1f\nrho: %.5f\ngam: %.3f\nCp: %.1f\nPt: %.1f\nTt: %.1f\nV: %.1f\nA: %.3f\nmdot: %.1f\nflow: %d\npos: %d",
			sta->M, sta->P, sta->T, sta->rho, sta->gam, sta->Cp, sta->Pt, sta->Tt, sta->V, sta->A, sta->mdot, sta->flow, sta->pos);
}

void drawPanel(ucBuf *ptrString, ucBuf *ptrStringB, ucBuf *ptrN) {
	DrawText(*ptrString, 50, 50, 50, (Color){245, 255, 245, 255});
	DrawText(*ptrStringB, 560, 80, 30, (Color){245, 255, 245, 255});
	DrawText(*ptrN, 800, 80, 30, (Color){245, 255, 245, 255});
	DrawText("Ambient", 560, 40, 40, WHITE);
	DrawText("PostShock", 800, 40, 40, WHITE);
	DrawText("shock deflection angle of 10\u00B0\narrow keys for altitude/velocity\n[tab] to reset\nZ for fine control\nX for coarse", 50, 600, 25, (Color){255, 225, 225, 255});
	DrawFPS(10, 10);
}
