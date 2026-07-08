// 3d rendering experiment, TODO basic flight model (way down the line)
// for now just cube
// need to rewrite and dial in FOV/scaling stuff
// known bug: you gotta hit W or S before any rotation works 
// (probably because of the order of operations and the if cascade i thought was clever oof)

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

// flight definitions only
#define FLIGHT_ALT_STEP 10.0
#define FLIGHT_ROT_STEP 0.01
#define FLIGHT_ZOOM_STEP 0.01
#define DEFAULT_ROT (Vec3){0.0, 0.0, 0.0}
#define CUBE_HALFLEN 200.0

// sky colour
void getSkyColour(Color *sky, float alt) {
	if (alt >= 80000) {
		*sky = (Color){0, 0, 0, 255};
	} else if (alt <= 0) {
		*sky = (Color){130, 210, 250, 255};
	} else {
		*sky = (Color){135 * ((-1 * (alt - 80000)) / 80000),
			205 * (-sqrt(alt / 80000) + 1),
			250 * sqrt((alt - 80000) / -80000),
			255};
	}
}

// ensuring angles are from 0deg-360deg for input
void boundAngle_P360(double *angle) {
	if (*angle < 0.0) *angle = 360.0;
	if (*angle > 360.0) *angle = 0.0;
}

void boundVec3Angles_P360(Vec3 *ptr) {
	boundAngle_P360(&(ptr->x));
	boundAngle_P360(&(ptr->y));
	boundAngle_P360(&(ptr->z));
}

// object vertex stuff
void rotateObjVertX(Vec3 *ptr, double *sinx, double *cosx) {
	// ptr->x = ptr->x;
	double ogY = ptr->y;
	double ogZ = ptr->z;
	ptr->y = (ogY * *cosx) - (ogZ * *sinx);
	ptr->z = (ogY * *sinx) + (ogZ * *cosx);
}

void rotateObjVertY(Vec3 *ptr, double *siny, double *cosy) {
	double ogX = ptr->x;
	double ogZ = ptr->z;
	ptr->x = (ogZ * *siny) + (ogX * *cosy);
	// ptr->y = ptr->y;
	ptr->z = (ptr->y * *cosy) + (ogX * -(*siny));
}

void rotateObjVertZ(Vec3 *ptr, double *sinz, double *cosz) {
	double ogX = ptr->x;
	double ogY = ptr->y;
	ptr->x = (ogX * *cosz) - (ogY * *sinz);
	ptr->y = (ogX * *sinz) + (ogY * *cosz);
	// ptr->z = ptr->z;
}

void ensurePositiveVec3Angles(Vec3 *ptr) {
	if (ptr->x < 0.0) ptr->x += 360.0;
	if (ptr->x > 360.0) ptr->x -= 360.0;
	if (ptr->y < 0.0) ptr->y += 360.0;
	if (ptr->y > 360.0) ptr->y -= 360.0;
	if (ptr->z < 0.0) ptr->z += 360.0;
	if (ptr->z > 360.0) ptr->z -= 360.0;
}

void rotateObjVert(Vec3 *ptr, Vec3 *rot) {
	ensurePositiveVec3Angles(rot);
	if (rot->x != 0.0) {
		double sinx = sin(rot->x * DEG_RAD);
		double cosx = cos(rot->x * DEG_RAD);
		rotateObjVertX(ptr, &sinx, &cosx);
		if (rot->y != 0.0) {
			double siny = sin(rot->y * DEG_RAD);
			double cosy = cos(rot->y * DEG_RAD);
			rotateObjVertY(ptr, &siny, &cosy);
			if (rot->z != 0.0) {
				double sinz = sin(rot->z * DEG_RAD);
				double cosz = cos(rot->z * DEG_RAD);
				rotateObjVertZ(ptr, &sinz, &cosz);
			}
		}
	}
	// intrinsic rotation order has to be x, y, z
	//printf("OBJROT x=%.3f y=%.3f z=%.3f a=%.1f b=%.1f g=%.1f\n", ptr->x, ptr->y, ptr->z, rot->x, rot->y, rot->z);
}

void translateObjVert(Vec3 *ptr, Vec3 *trans) {
	ptr->x += trans->x;
	ptr->y += trans->y;
	ptr->z += trans->z;
}

void centreObjVert(Vec3 *ptr, Vec3 *trans) {
	// do this before intrinsic rotation ig
	ptr->x -= trans->x;
	ptr->y -= trans->y;
	ptr->z -= trans->z;
}

// camera macro and type definitions
#define W_IMAGE 35.0
#define H_IMAGE 18.0
#define W_SCREEN 1400.0
#define H_SCREEN 720.0
#define SCALE_OBJ 1000.0
#define SCALE_PRINT 40.0

typedef struct {
	Vec3 posLens; // pinhole lens
	Vec3 rotLens;
	double f;
	double FOV;
	double dLensImage;
	double dSubject;
} Cam3;

void initCam3(Cam3 *ptr) {
	ptr->posLens = (Vec3){0.0, 0.0, 0.0};
	ptr->rotLens = (Vec3){0.0, 0.0, 0.0};
	ptr->f = -1.0;
	ptr->FOV = -1.0;
	ptr->dLensImage = -1.0;
	ptr->dSubject = -1.0;
}

// camera functions
void getCam3FOV(Cam3 *ptr) {
	ptr->FOV = 2.0 * RAD_DEG * atan(W_IMAGE / (2.0 * ptr->f));
}

void getCam3FocalLength(Cam3 *ptr) {
	ptr->f = W_IMAGE / (2.0 * tan(0.5 * DEG_RAD * ptr->FOV));
}

void getCam3dLensImage(Cam3 *ptr) {
	ptr->dSubject = -(ptr->posLens.z);
	ptr->dLensImage = 1.0 / ((1.0 / ptr->f) - (1.0 / -(ptr->dSubject)));
}

void translateCamVert(Vec3 *ptr, Vec3 *pos) {
	ptr->x -= pos->x;
	ptr->y -= pos->y;
	ptr->z -= pos->z;
	// unsure if this function is really needed
}

void rotateCamVert(Vec3 *ptr, Vec3 *rot) {
	ensurePositiveVec3Angles(rot);
	double sinx = sin(rot->x * DEG_RAD); // probably optimise this so it only runs the trig functions when needed, like the obj rotation
	double cosx = cos(rot->x * DEG_RAD);
	double siny = sin(rot->y * DEG_RAD);
	double cosy = cos(rot->y * DEG_RAD);
	double sinz = sin(rot->z * DEG_RAD);
	double cosz = cos(rot->z * DEG_RAD);
	double ogX = ptr->x;
	double ogY = ptr->y;
	double ogZ = ptr->z;

	ptr->x = (cosy * ((sinz * ogY) + (cosz * ogX))) - (siny * ogZ);
	ptr->y = (sinx * ((cosy * ogZ) + (siny * ((sinz * ogY) + (cosz * ogX))))) + (cosx * ((cosz * ogY) - (sinz * ogX)));
	ptr->z = (cosx * ((cosy * ogZ) + (siny * ((sinz * ogY) + (cosz * ogX))))) - (sinx * ((cosz * ogY) - (sinz * ogX)));
	//printf("CAMROT x=%.3f y=%.3f z=%.3f a=%.1f b=%.1f g=%.1f\n", ptr->x, ptr->y, ptr->z, rot->x, rot->y, rot->z);
}

// project from 3d to 2d
void projectCamVert(Vec3 *camv, Vec2 *scrv, Cam3 *ptr) {
	scrv->x = ((camv->x * W_SCREEN) / (camv->z * W_IMAGE)) * ptr->dLensImage;
	scrv->y = ((camv->y * H_SCREEN) / (camv->z * H_IMAGE)) * ptr->dLensImage;
	//printf("PROJECT x=%.3f y=%.3f f=%.1f FOV=%.1f dLensImage=%.3f dSubject=%.3f\n", scrv->x, scrv->y, ptr->f, ptr->FOV, ptr->dLensImage, ptr->dSubject);
}

// test cube 
typedef struct {
	Vec3 verts[8];
	Vec3 camVs[8];
	Vec2 screenVs[8];
	Line2 lines[12];
	Vec3 rot;
	Vec3 trans;
} LeCube;

void initLeCube(LeCube *ptr, float sideLen) {
	ptr->rot = DEFAULT_ROT;
	ptr->trans = DEFAULT_ROT;
	float len = sideLen * 0.5f;
	ptr->verts[0] = (Vec3){len, -len, -len};
	ptr->verts[1] = (Vec3){len, len, -len};
	ptr->verts[2] = (Vec3){len, -len, len};
	ptr->verts[3] = (Vec3){len, len, len};
	ptr->verts[4] = (Vec3){-len, -len, -len};
	ptr->verts[5] = (Vec3){-len, len, -len};
	ptr->verts[6] = (Vec3){-len, -len, len};
	ptr->verts[7] = (Vec3){-len, len, len};
	ptr->lines[0] = (Line2){0, 1};
	ptr->lines[1] = (Line2){0, 2};
	ptr->lines[2] = (Line2){0, 4};
	ptr->lines[3] = (Line2){1, 3};
	ptr->lines[4] = (Line2){1, 5};
	ptr->lines[5] = (Line2){2, 3};
	ptr->lines[6] = (Line2){2, 6};
	ptr->lines[7] = (Line2){3, 7};
	ptr->lines[8] = (Line2){4, 5};
	ptr->lines[9] = (Line2){4, 6};
	ptr->lines[10] = (Line2){5, 7};
	ptr->lines[11] = (Line2){6, 7};
}

// rotate before translating
void projectLeCube(LeCube *cube, Cam3 *cam) {
	for (int i = 0; i < 8; i++) {
		cube->camVs[i] = cube->verts[i];
		rotateObjVert(&(cube->camVs[i]), &(cube->rot));
		translateObjVert(&(cube->camVs[i]), &(cube->trans));
		translateCamVert(&(cube->camVs[i]), &(cam->posLens));
		//rotateCamVert(&(cube->camVs[i]), &(cam->rotLens));
		projectCamVert(&(cube->camVs[i]), &(cube->screenVs[i]), cam);
		//printf("i=%d X=%.3f Y=%.3f Z=%.3f x=%.3f y=%.3f a=%.1f b=%.1f c=%.1f\n", i, cube->camVs[i].x, cube->camVs[i].y, cube->camVs[i].z, cube->screenVs[i].x, cube->screenVs[i].y, cube->rot.x, cube->rot.y, cube->rot.z);
	}
	//printf("\n");
}

void drawLeCube(LeCube *ptr, double scale) {
	for (int i = 0; i < 12; i++) {
		int li1 = ptr->lines[i].v1;
		int li2 = ptr->lines[i].v2;

		double v1x = scale*ptr->screenVs[li1].x + (0.5 * W_SCREEN);
		double v1y = scale*ptr->screenVs[li1].y + (0.5 * H_SCREEN);
		double v2x = scale*ptr->screenVs[li2].x + (0.5 * W_SCREEN);
		double v2y = scale*ptr->screenVs[li2].y + (0.5 * H_SCREEN);

		//printf("Line n=%d\nv1x=%.6f 1y=%.6f\n2x=%.6f 2y=%.6f\n", i, v1x, v1y, v2x, v2y);
		DrawLine((int)v1x, (int)v1y, (int)v2x, (int)v2y, BLACK);
	}
}

void cube3D() {
	InitWindow(W_SCREEN, H_SCREEN, "Pearoplane Alpha v0.1");

	float alt = 0.0;
	Color sky;
	char buffer[255];
	char otherBuf[1024];

	LeCube daCube;
	initLeCube(&daCube, 0.5);
	for (int i = 0; i < 8; i++) {
		printf("%f %f %f\n", daCube.verts[i].x, daCube.verts[i].y, daCube.verts[i].z);
	}

	Cam3 daCam;
	daCam.posLens = (Vec3){0.0, 0.0, -10.0};
	daCam.rotLens = DEFAULT_ROT;
	daCam.FOV = 10.0;
	getCam3FocalLength(&daCam);
	getCam3dLensImage(&daCam);
	double scale = 1.0;

	//SetTargetFPS(1200);
	while (!WindowShouldClose()) {
		if (IsKeyPressed(KEY_TAB)) {
			alt = 0.0;
			daCube.rot = DEFAULT_ROT;
			scale = 1.0;
			//cameraRot = DEFAULT_ROT;
		}
		if (IsKeyDown(KEY_P)) alt += FLIGHT_ALT_STEP;
		if (IsKeyDown(KEY_L)) alt -= FLIGHT_ALT_STEP;
		if (IsKeyDown(KEY_S)) daCube.rot.x -= FLIGHT_ROT_STEP;
		if (IsKeyDown(KEY_W)) daCube.rot.x += FLIGHT_ROT_STEP;
		if (IsKeyDown(KEY_A)) daCube.rot.y -= FLIGHT_ROT_STEP;
		if (IsKeyDown(KEY_D)) daCube.rot.y += FLIGHT_ROT_STEP;
		if (IsKeyDown(KEY_Q)) daCube.rot.z -= FLIGHT_ROT_STEP;
		if (IsKeyDown(KEY_E)) daCube.rot.z += FLIGHT_ROT_STEP;
		if (IsKeyDown(KEY_LEFT_BRACKET)) scale -= FLIGHT_ZOOM_STEP;
		if (IsKeyDown(KEY_RIGHT_BRACKET)) scale += FLIGHT_ZOOM_STEP;
		if (scale < 0.1) scale = 0.1;
		if (scale > 100.0) scale = 100.0;
		//displayVec.z = expf(logf(displayVec.z) + ((float)GetMouseWheelMove() * 0.01f));
		//boundVec3Angles_P360(&(daCube.rot));
		//printf("scale = %.3f\n", scale);
		projectLeCube(&daCube, &daCam);

		getSkyColour(&sky, alt);
		BeginDrawing();
		ClearBackground(sky);
			DrawFPS(10, 10);
			drawLeCube(&daCube, scale);
			for (int i = 0; i < 8; i++) {
				char buf[10];
				sprintf(buf, "v%d", i);
				double vx = scale*daCube.screenVs[i].x + (0.5 * W_SCREEN);
				double vy = scale*daCube.screenVs[i].y + (0.5 * H_SCREEN);
				DrawText(buf, (int)vx, (int)vy, 20, WHITE);
			}
		EndDrawing();
	}

	CloseWindow();
}
