namespace models {
	struct Turbojet {
		float altitude = 10000.0;
		float Ain = 10.0;
		float Vin = 800.0;
		float defAngle = 10.0;
		float inletDiffM = 0.2;
		float inletDiffA = 9.0;
		float compPR = 20.0;
		float compDiffM = 0.2;
		float compDiffA = 2.0;
		float MCCtemp = 1800.0;
		float MCCmach = 0.5;
		float MCCdiffM = 0.3;
		float MCCdiffA = 3.0;
		float turbDiffM = 0.3;
		float turbDiffA = 8.0;
		float ABtemp = 2400.0;
		float ABmach = 0.8;
		float Aout = 12.0;
		float Vout = 0.0;
		float fuelConsumption = 0.0;
		float Fthrust = 0.0;
		float Isp = 0.0;
		float IspSec = 0.0;
	};
	void turbojet(Turbojet &tj, string &print) {
		Station sta;
		sta.A = tj.Ain;
		sta.V = tj.Vin;
		getAmbientStation(sta, tj.altitude);
		float Pamb = sta.P;
		cout << "\nAmbient P=" << sta.P << " T=" << sta.T << " rho=" << sta.rho << " M=" << sta.M << " V=" << sta.V << " A=" << sta.A << "\n";

		Intake inVecs;
		inVecs.defAngle = tj.defAngle;
		supersonicIntake(sta, inVecs);
		cout << "Inlet P=" << sta.P << " T=" << sta.T << " rho=" << sta.rho << " M=" << sta.M << " V=" << sta.V << " A=" << sta.A << "\n";

		Diffuser inDiff;
		inDiff.Aout = tj.inletDiffA;
		inDiff.Mout = tj.inletDiffM;
		subsonicDiffuser(sta, inDiff);
		cout << "InletDiff P=" << sta.P << " T=" << sta.T << " rho=" << sta.rho << " M=" << sta.M << " V=" << sta.V << " A=" << sta.A << "\n";

		SimpleComp compress;
		compress.Pratio = tj.compPR;
		simpleCompressor(sta, compress);
		cout << "Compressor P=" << sta.P << " T=" << sta.T << " rho=" << sta.rho << " M=" << sta.M << " V=" << sta.V << " A=" << sta.A << "\n";

		Diffuser compDiff;
		compDiff.Aout = tj.compDiffA;
		compDiff.Mout = tj.compDiffM;
		subsonicDiffuser(sta, compDiff);
		cout << "CompDiff P=" << sta.P << " T=" << sta.T << " rho=" << sta.rho << " M=" << sta.M << " V=" << sta.V << " A=" << sta.A << "\n";

		Combustor mcc;
		mcc.maxTemp = tj.MCCtemp;
		mcc.maxMach = tj.MCCmach;
		subsonicCombustion(sta, mcc);
		cout << "Combustor P=" << sta.P << " T=" << sta.T << " rho=" << sta.rho << " M=" << sta.M << " V=" << sta.V << " A=" << sta.A << "\n";

		SimpleTurb turb;
		turb.Power = compress.Power;
		simpleTurbine(sta, turb);
		cout << "Turbine P=" << sta.P << " T=" << sta.T << " rho=" << sta.rho << " M=" << sta.M << " V=" << sta.V << " A=" << sta.A << "\n";
		
		Diffuser turbDiff;
		turbDiff.Aout = tj.turbDiffA;
		turbDiff.Mout = tj.turbDiffM;
		subsonicDiffuser(sta, turbDiff);
		cout << "TurbDiff P=" << sta.P << " T=" << sta.T << " rho=" << sta.rho << " M=" << sta.M << " V=" << sta.V << " A=" << sta.A << "\n";

		Combustor aftb;
		aftb.maxTemp = tj.ABtemp;
		aftb.maxMach = tj.ABmach;
		subsonicCombustion(sta, aftb);
		cout << "Afterburner P=" << sta.P << " T=" << sta.T << " rho=" << sta.rho << " M=" << sta.M << " V=" << sta.V << " A=" << sta.A << "\n";

		Throat throa;
		convergingThroat(sta, throa);
		cout << "Throat P=" << sta.P << " T=" << sta.T << " rho=" << sta.rho << " M=" << sta.M << " V=" << sta.V << " A=" << sta.A << "\n";

		Ejector ejec;
		ejec.Aout = tj.Aout;
		ejec.Pamb = Pamb;
		supersonicEjector(sta, ejec);
		cout << "Ejector P=" << sta.P << " T=" << sta.T << " rho=" << sta.rho << " M=" << sta.M << " V=" << sta.V << " A=" << sta.A << "\n";

		tj.fuelConsumption = mcc.fuelMdot + aftb.fuelMdot;
		tj.Vout = sta.V;
		tj.Fthrust = (sta.mdot * sta.V) - (sta.mdot * tj.Vin) + (sta.A * (sta.P - Pamb));
		tj.Isp = tj.Fthrust / tj.fuelConsumption;
		tj.IspSec = tj.Isp / g;
		cout << "\nThrust: " << tj.Fthrust / 1000 << "kN\nFuel consumption: " << tj.fuelConsumption << "kg/s\nEffective Vex: " << tj.Isp << "m/s\nIsp: " << tj.IspSec << " seconds\n";
	}

	void ramjet() {
	}
	void turbofan() {
	}
	void turboramjet() {
	}
}
