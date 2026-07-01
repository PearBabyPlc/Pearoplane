#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h"
#include <stdio.h>
#include <math.h>
#include "main.h"

//#define VIEW_BUF_SIZE 70
// viewBuf: azimuth: -xxx.xxx\u00B0\nelevation: -xxx.xxx\u00B0\nzoom: xx.xxx at most (64char)
// will place on the heap since it's not too important

void drawMenu() {
	ClearBackground(LIME);
	DrawText("Pearoplane Alpha v0.1 (Menu)", 50, 30, 50, WHITE);
}

void drawOutside(ViewState *view, char *printViewBuf) {
	ClearBackground(BLUE);
	DrawFPS(10, 10);
	//char viewBuf[VIEW_BUF_SIZE];
	sprintf(printViewBuf, "azimuth: %.3f\u00B0\nelevation: %.3f\u00B0\nzoom: %.3f", view->azimuth, view->elevation, view->zoom);
	DrawText(printViewBuf, 10, 40, 20, RAYWHITE);
	DrawText("View: Outside", 980, 20, 30, RAYWHITE);
}

#define R_EARTH 6356766.0
void drawOrbital(ViewState *view, char *printViewBuf, OrbitState *ptr) {
	// various prints
	ClearBackground(BLACK);
	DrawFPS(10, 10);
	//char viewBuf[VIEW_BUF_SIZE];
	sprintf(printViewBuf, "azimuth: %.3f\u00B0\nelevation: %.3f\u00B0\ntimewarp: %.3fx", view->azimuth, view->elevation, view->zoom);
	DrawText(printViewBuf, 10, 40, 20, RAYWHITE);
	DrawText("View: Orbital", 980, 20, 30, RAYWHITE);
	char orbBuf[100];
	sprintf(orbBuf, "x=%.3f\ny=%.3f\nh=%.3fkm\nV=%.3fm/s\ng=%.6fm/s^2", ptr->x/1000, ptr->y/1000, ptr->h/1000, ptr->V, ptr->g);
	DrawText(orbBuf, 10, 670, 20, WHITE);
	char energyBuf[100];
	sprintf(energyBuf, "kinetic=%f\ngravpot=%f\ntotal=%f", ptr->KE, ptr->GPE, ptr->TE);
	DrawText(energyBuf, 900, 600, 20, WHITE);
	char timerBuf[32];
	int hours = ptr->timer / 3600;
	int minutes = ((int)ptr->timer % 3600) / 60;
	float seconds = ptr->timer - ((hours * 3600) + (minutes * 60));
	sprintf(timerBuf, "T+%dh%dm%.1fs", hours, minutes, seconds);
	DrawText(timerBuf, 250, 20, 40, RAYWHITE);

	// draw ship and earth
	DrawCircleLines(600, 400, R_EARTH*SCALE, SKYBLUE);
	float shipX = (ptr->x * SCALE) + 600;
	float shipY = -(ptr->y * SCALE) + 400;
	DrawRectangle((int)shipX, (int)shipY, 5, 5, GRAY);
}
#undef R_EARTH

void drawPanel() {
	ClearBackground(GRAY);
	DrawText("Panel: Main", 1075, 15, 20, RAYWHITE);

}

void drawAero(AeroTest *ptr) {
	ClearBackground(SKYBLUE);
	DrawFPS(10, 10);
	DrawText("Panel: Aerodynamics", 980, 15, 20, RAYWHITE);
	
	// altitude velocity area span chord AoA lift drag LD
	aeroTest(ptr);
	
	char viewBuf[255];
	sprintf(viewBuf, "alt %.1f metres\nvel %.1f m/s\narea %.1f m2\nspan %.1f\nchord %.1f m\nAoA %.1f\u00B0\nlift %.1f tons\ndrag %.1f kN\nLD %.3f", ptr->altitude, ptr->velocity, ptr->area, ptr->span, ptr->chord, ptr->AoA, ptr->lift, ptr->drag, ptr->LD);
	DrawText(viewBuf, 100, 50, 20, WHITE);
}

void drawProp() {
	ClearBackground(RED);
	DrawText("Panel: Propulsion", 1010, 15, 20, RAYWHITE);
}
