enum Axis {
	x-positive, x-negative,
	y-positive, y-negative,
	z-positive, z-negative
};

struct SearsHaackBody {
	float maxRadius;
	float length;
	enum Axis radiusAxis;
	enum Axis lengthAxis;
};

const string SHstart = "startSearsHaack!";
const string SHmaxRad = "SH-maxRadius: ";
const string SHlength = "SH-length: ";
const string SHradAxis = "SH-radiusAxis: ";
const string SHlenAxis = "SH-lengthAxis: ";

// this does not work yet
void parseVehicleFile() {
	ifstream VehicleFile("vehicle.pearo");
	vector<string> vehicleFileLines;
	string line;
	while (getline(VehicleFile, line)) {
		vehicleFileLines.push_back(line);
	}
	VehicleFile.close();
	
	ofstream CopyFile("vehicleCopy.txt");
	ofstream ObjFile("vehicle.obj");
	for (int i = 0; i < vehicleFileLines.size(); i++) {
		string objLine = vehicleFileLines[i];
		CopyFile << objLine << "\n";
		if (objLine.contains(SHstart)) {
			struct SearsHaackBody objSH;
		}
		
	}
	CopyFile.close();
	ObjFile.close();
	cout << "\nSuccessfuly parsed vehicle.pearo\n";
}
