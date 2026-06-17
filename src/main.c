#include "raylib.h"
#include "resource_dir.h"

int main () {
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(1200, 800, "Pearoplane Alpha v0.1");
	SearchAndSetResourceDir("resources");
	Texture pearbaby = LoadTexture("pearbaby.png");
	
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);
		DrawTexture(pearbaby, 40, 20, WHITE);
		EndDrawing();
	}

	UnloadTexture(pearbaby);
	CloseWindow();
	return 0;
}
