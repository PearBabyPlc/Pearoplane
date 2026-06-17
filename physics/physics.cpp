#include <iostream>
#include <ctime>
#include <string>
#include <cmath>
#include <chrono>
#include <thread>
#include <fstream>
using namespace std;

// this is the physics model
// at the moment im attempting to create a basic orbit model, it's not going very well 
// then 6DOF stuff will follow

const float rEarth = 6356766.0;
const float g0 = 9.80665;
const double pi = 2 * acos(0.0);
//const int massKg = 1000;
//const float dt = 0.1;

double get_hyp(double &x, double &y) {
	return sqrt(pow(x, 2) + pow(y, 2));
}

double get_g(double &h) {
	double tempG = rEarth / h;
	return g0 * pow(tempG, 2);
}

// TODO just completely fuckin rewrite
void orbitSim(double &x, double &y, double &v, long &attitude, string &xs, string &ys) {
	x = 0.0;
	double h = get_hyp(x, y);
	double prevX;
	double prevY;
	double dx;
	double dy;
	long latitude = 0.0;
	double g = get_g(h);
	int i = 0;
	xs = "[";
	ys = "[";
	while (h >= 0.0) {
		h = get_hyp(x, y);
		g = get_g(h);
		latitude = cos(y) * h;
		prevX = x;
		prevY = y;
		x += (v * cos(attitude - latitude)) - (g * sin(latitude));
		y += (v * sin(attitude - latitude)) - (g * cos(latitude));
		dx = x - prevX;
		dy = y - prevY;
		v = get_hyp(dx, dy);

		cout << "x=" << x / 1000 << " y=" << y / 1000 << " v=" << v << " alt=" << h - rEarth << " g=" << g << "\n";
		i += 1;
		xs.append(to_string(x));
		ys.append(to_string(y));
		if (i > 2000) {
			xs.append("]");
			ys.append("]");
			break;
		} else {
			xs.append(", ");
			ys.append(", ");
		}
	}
}

int main() {
	clock_t before = clock();
	cout << "Hello. physics.cpp is running!\n\n";

	double x = 0.0;
	double y = 600000.0 + rEarth;
	double v = 7800.0;
	long att = 5.0 * (180.0 / pi);
	string xs;
	string ys;
	orbitSim(x, y, v, att, xs, ys);
	//cout << "\n" << xs << "\n\n" << ys << "\n";

	ofstream Xlist("Xlist.txt");
	Xlist << xs;
	Xlist.close();
	ofstream Ylist("Ylist.txt");
	Ylist << ys;
	Ylist.close();

	clock_t duration = clock() - before;
	cout << "\nDuration: " << (float)duration * 1000 / CLOCKS_PER_SEC << " ms";
	return 0;
}

