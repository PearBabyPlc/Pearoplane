// this time around, minimum comparmentalisation
// file size doesn't matter for modern computers
// but function call overhead and duplication does
// this version will have each subprogram in a single C file
// main.h will link everything
// for now this is all gonna be testing out subprograms
// such as the orbital model, 3d flight model, 3d rendering,
// engine model, etc...
// idfk what i'm doing

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
	cube3D();
	orbit2D();
	return 0;
}
