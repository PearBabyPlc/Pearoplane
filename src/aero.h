typedef struct {
	float alt;
	float H;
	float T;
	float P;
	float rho;
	//float RH;
} ISA;

void getISA(ISA *ptr);

void aeroTest(AeroTest *ptr);

// aero.h will link all aero components and typedefs together (isa, formulaegg, solvers, models)
// main.h links main aero.c file and IO stuff (screen.c, userinput.c)
// basically all files need main.h included first lol shit whatever
// at least I won't have a half dozen daggy headers, function/type definitions incur no memory penalty
//
// main.c loop does UI, then aeroTick() and physicsTick() as necessary, then displays at framerate. 
//
// TODO physics.c and physics.h
//
// main.c included in all model/UI files (to process states)
