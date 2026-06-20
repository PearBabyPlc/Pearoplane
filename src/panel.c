#include "raylib.h"

void drawPanel(char *ptrString) {
	DrawText(ptrString, 50, 50, 50, (Color){245, 255, 245, 255});
	DrawText("arrow keys for altitude/velocity\n[tab to reset]\nZ for fine control\nX for coarse", 50, 650, 25, (Color){255, 225, 225, 255});
	DrawFPS(10, 10);
}
