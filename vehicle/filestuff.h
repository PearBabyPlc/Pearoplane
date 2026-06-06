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

