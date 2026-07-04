// this time around, minimum comparmentalisation
// file size doesn't matter for modern computers
// but function call overhead and duplication does
// this version will have everything in one C file

// Raylib headers
#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h"

// C standard library
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Pearbaby headers
#include "main.h"

int main() {
	flight3D();
	orbit2D();
	return 0;
}
