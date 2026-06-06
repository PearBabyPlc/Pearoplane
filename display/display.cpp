#include <iostream>
#include <cmath>
#include <string>
#include <ctime>
using namespace std;

// display.cpp is set up as a main file with the function below to allow for execution
// and debugging in the console, just on its own. as the program is written it'll be
// condensed down so there's only one int main() in the whole project. cmake stuff 

int main() {
	clock_t before = clock();
	cout << "Hello. display.cpp is running!\n";

	clock_t duration = clock() - before;
	cout << "\nDuration: " << (float)duration * 1000 / CLOCKS_PER_SEC << " ms";
	return 0;
}

