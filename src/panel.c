#include "raylib.h"

void drawPanel(char *ptrString) {
	ClearBackground(BLACK);
	DrawText(ptrString, 100, 100, 50, WHITE);
	DrawFPS(10, 10);
}
