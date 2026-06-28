#include "raylib.h"
#include "resource_dir.h"
#include "stdio.h"
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

void drawOrbital(ViewState *view, char *printViewBuf) {
	ClearBackground(BLACK);
	DrawFPS(10, 10);
	//char viewBuf[VIEW_BUF_SIZE];
	sprintf(printViewBuf, "azimuth: %.3f\u00B0\nelevation: %.3f\u00B0\nzoom: %.3f\u00B0", view->azimuth, view->elevation, view->zoom);
	DrawText(printViewBuf, 10, 40, 20, RAYWHITE);
	DrawText("View: Orbital", 980, 20, 30, RAYWHITE);

}

void drawPanel() {
	ClearBackground(GRAY);
	DrawText("Panel: Main", 1075, 15, 20, RAYWHITE);

}

void drawAero() {
	ClearBackground(SKYBLUE);
	DrawText("Panel: Aerodynamics", 980, 15, 20, RAYWHITE);
}

void drawProp() {
	ClearBackground(RED);
	DrawText("Panel: Propulsion", 1010, 15, 20, RAYWHITE);
}

