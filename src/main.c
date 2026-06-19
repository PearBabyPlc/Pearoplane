#include "raylib.h"
#include "resource_dir.h"
#include "stdio.h"
#include "stdlib.h"
#include "main.h"

int main() {
	const int sW = 1200;
	const int sH = 800;
	InitWindow(sW, sH, "Pearoplane Alpha v0.1");
	SetTargetFPS(60);

	int i = 0;
	char buffer[32];
	char printString[] = "Test";

	while (!WindowShouldClose()) {
		i += 1;
		sprintf(buffer, "%s\n\n%d", printString, i);
		char *ptrString = buffer;
		
		BeginDrawing();
		drawPanel(ptrString);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
