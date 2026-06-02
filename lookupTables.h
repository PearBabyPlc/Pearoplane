const float g = 9.80665;
const float rEarth = 6356766;
const float Runiv = 8314.46;
const float molAir = 28.9644;
const float RsAir = Runiv / molAir;

namespace getISA {
	const int Href[9] = {
		0,
		11000,
		20000,
		32000,
		47000,
		51000,
		71000,
		84852,
		90000
	};
	// const int Hmax = 105000;
	const float Tref[9] = {
		288.15,
		216.65,
		216.65,
		228.65,
		270.65,
		270.65,
		214.65,
		186.95,
		186.95
	};
	const double Pref[9] = {
		101325,
		22632.6352091,
		5475.15769257,
		868.088257003,
		110.919037481,
		66.9471267903,
		3.95709671594,
		0.373462257454,
		0.145778536657
	};
	const float Lref[9] = {
		-0.0065,
		0,
		0.001,
		0.0028,
		0,
		-0.0028,
		-0.002,
		0,
		0.004
	};
	struct ISA {
		float alt = 0; //default values, update with getISA(ISA)
		float H = 0;
		float T = 288.15;
		double P = 101325;
		double rho = 1.225;
	};

	void getPTrho(ISA &cond) {
		cond.H = (rEarth * cond.alt) / (rEarth + cond.alt);
	
		if (cond.H < Href[1]) {
			cond.T = Tref[0] + (Lref[0] * cond.H);
			double Pexp = (g * molAir) / (Runiv * Lref[0]);
			double Pfrac = Tref[0] / cond.T;
			cond.P = Pref[0] * pow(Pfrac, Pexp);
		}
		if (cond.H >= Href[1] && cond.H < Href[2]) {
			cond.T = Tref[1];
			double Pexp = (-g * molAir * (cond.H - Href[1])) / (Runiv * Tref[1]);
			cond.P = Pref[1] * exp(Pexp);
		}
		if (cond.H >= Href[2] && cond.H < Href[3]) {
			cond.T = Tref[2] + (Lref[2] * (cond.H - Href[2]));
			double Pexp = (g * molAir) / (Runiv * Lref[2]);
			double Pfrac = Tref[2] / cond.T;
			cond.P = Pref[2] * pow(Pfrac, Pexp);
		}
		if (cond.H >= Href[3] && cond.H < Href[4]) {
			cond.T = Tref[3] + (Lref[3] * (cond.H - Href[3]));
			double Pexp = (g * molAir) / (Runiv * Lref[3]);
			double Pfrac = Tref[3] / cond.T;
			cond.P = Pref[3] * pow(Pfrac, Pexp);
		}
		if (cond.H >= Href[4] && cond.H < Href[5]) {
			cond.T = Tref[4];
			double Pexp = (-g * molAir * (cond.H - Href[4])) / (Runiv * Tref[4]);
			cond.P = Pref[4] * exp(Pexp);
		}
		if (cond.H >= Href[5] && cond.H < Href[6]) {
			cond.T = Tref[5] + (Lref[5] * (cond.H - Href[5]));
			double Pexp = (g * molAir) / (Runiv * Lref[5]);
			double Pfrac = Tref[5] / cond.T;
			cond.P = Pref[5] * pow(Pfrac, Pexp);
		}
		if (cond.H >= Href[6] && cond.H < Href[7]) {
			cond.T = Tref[6] + (Lref[6] * (cond.H - Href[6]));
			double Pexp = (g * molAir) / (Runiv * Lref[6]);
			double Pfrac = Tref[6] / cond.T;
			cond.P = Pref[6] * pow(Pfrac, Pexp);
		}
		if (cond.H >= Href[7] && cond.H < Href[8]) {
			cond.T = Tref[7];
			double Pexp = (-g * molAir * (cond.H - Href[7])) / (Runiv * Tref[7]);
			cond.P = Pref[7] * exp(Pexp);

		}
		if (cond.H >= Href[8]) {
			cond.T = Tref[8] + (Lref[8] * (cond.H - Href[8]));
			double Pexp = (g * molAir) / (Runiv * Lref[8]);
			double Pfrac = Tref[8] / cond.T;
			cond.P = Pref[8] * pow(Pfrac, Pexp);
		}

		cond.rho = cond.P / (cond.T * RsAir);
	}

	void readISA(ISA &cond) {
		std::cout << "Geometric altitude: " << cond.alt << " metres\n";
		std::cout << "Geopotential altitude: " << cond.H << " metres\n";
		std::cout << "Temperature: " << cond.T << " Kelvin\n";
		std::cout << "Pressure: " << cond.P << " Pascal\n";
		std::cout << "Density: " << cond.rho << " kg/m3\n \n";
	}
}
