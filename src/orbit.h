#define KEPLER

void runOrbit();

#ifdef KEPLER
void runKeplerOrbit();

void runOrbit() {
	runKeplerOrbit();
}
#endif
