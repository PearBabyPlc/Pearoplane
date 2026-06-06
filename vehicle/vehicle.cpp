#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>
#include <ctime>
using namespace std;

// vehicle.cpp set up for now to take parametric inputs and return a .obj file
// well it will be at some point

void createVehicleFile() {
	ofstream VehicleFile("vehicle.pearo");
	VehicleFile << "!!PEAROPLANE!!\n" << "!!VEHICLEFILE!!\n";
	VehicleFile.close();
}

void readVehicleFile() {
	ifstream VehicleFile("vehicle.pearo");
	string printLines;
	string line;
	while (getline(VehicleFile, line)) {
		string printLine = line + "\n";
		printLines.append(printLine);
	}
	cout << "\n========== Begin reading vehicle.pearo ==========\n" 
		<< printLines 
		<< "======== Finished reading vehicle.pearo =========\n";
}

void parseVehicleFile() {
	ifstream VehicleFile("vehicle.pearo");
	vector<string> vehicleFileLines;
	string line;
	while (getline(VehicleFile, line)) {
		vehicleFileLines.push_back(line);
	}
	VehicleFile.close();
	
	ofstream ObjFile("vehicle.txt");
	for (int i = 0; i < vehicleFileLines.size(); i++) {
		ObjFile << vehicleFileLines[i] << "\n";
	}
	ObjFile.close();
	cout << "\nSuccessfuly parsed vehicle.pearo\n";
}

int main() {
	clock_t before = clock();
	cout << "Hello. vehicle.cpp is running!\n";

	readVehicleFile();
	parseVehicleFile();

	clock_t duration = clock() - before;
	cout << "\nvehicle.cpp concluded.\nDuration: " << (float)duration * 1000 / CLOCKS_PER_SEC << " ms\n";
	return 0;
}

