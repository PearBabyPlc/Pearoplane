void printStation(Station &sta, string &print) {
	print.append("\nStation pos = ");
	print.append(to_string(sta.pos));
	print.append("\nM = ");
	print.append(to_string(sta.M));
	print.append("\nP = ");
	print.append(to_string(sta.P));
	print.append("\nT = ");
	print.append(to_string(sta.T));
	print.append("\nrho = ");
	print.append(to_string(sta.rho));
	print.append("\ngam = ");
	print.append(to_string(sta.gam));
	print.append("\nCp = ");
	print.append(to_string(sta.Cp));
	print.append("\nPt = ");
	print.append(to_string(sta.Pt));
	print.append("\nTt = ");
	print.append(to_string(sta.Tt));
	print.append("\nV = ");
	print.append(to_string(sta.V));
	print.append("\nA = ");
	print.append(to_string(sta.A));
	print.append("\nmdot = ");
	print.append(to_string(sta.mdot));
	print.append("\n");
}

void printBasicStation(Station &sta, string &print) {
	print.append("\nStation pos=");
	print.append(to_string(sta.pos));
	print.append(" Mach=");
	print.append(to_string(sta.M));
	print.append(" P=");
	print.append(to_string(sta.P * 0.00001));
	print.append("bar T=");
	print.append(to_string(sta.T));
	print.append("K rho=");
	print.append(to_string(sta.rho));
	print.append("kg/m3 Pt=");
	print.append(to_string(sta.Pt * 0.00001));
	print.append("bar V=");
	print.append(to_string(sta.V));
	print.append("m/s A=");
	print.append(to_string(sta.A));
	print.append("m2 mdot=");
	print.append(to_string(sta.mdot));
	print.append("kg/s\n");
}

void printShock(Shock &sho, string &print) {
	print.append("\nShock pos=");
	print.append(to_string(sho.pos));
	print.append(" defDeg=");
	print.append(to_string(sho.defDeg));
	print.append(" shockDeg=");
	print.append(to_string(sho.shockDeg));
	print.append(" type=");
	if (sho.type == 0) {
		print.append("WEAK");
	} else if (sho.type == 1) {
		print.append("STRONG");
	} else if (sho.type == 2) {
		print.append("NORMAL");
	} else if (sho.type == 3) {
		print.append("DEBUG");
	} else {
		print.append("ERROR");
	}
	print.append("\n");
}

void printExpansionFan(ExpansionFan &fan, string &print) {
	print.append("\nExpansion fan pos=");
	print.append(to_string(fan.pos));
	print.append(" defDeg=");
	print.append(to_string(fan.defDeg));
	print.append(" degMu1=");
	print.append(to_string(fan.degMu1));
	print.append(" degMu2=");
	print.append(to_string(fan.degMu2));
	print.append("\n");
}

void getAmbientStation(Station &sta, float &altitude) {
	getISA::ISA isaStruct;
	isaStruct.alt = altitude;
	getISA::getPTrho(isaStruct);
	getISA::addISAtoStation(sta, isaStruct);
	calImp::getGam(sta);
	calImp::getCp(sta);
	sta.M = sta.V / sqrt(RsAir * sta.gam * sta.T);
	isen::getTt_fromT(sta);
	isen::getPt_fromP(sta);
	sta.pos = 0;
	getMdot_fromA(sta);
}

void solveShock(Station &sta, Shock &sho) {
	float maxDefDeg = degInRad * oblique::maxDefRad(sta);
	if (sho.defDeg < maxDefDeg) {
		float defRad = radInDeg * sho.defDeg;
		float shockRad = oblique::radShockAngle(sta, defRad);
		sho.shockDeg = degInRad * shockRad;
		oblique::updateStationOblique(sta, shockRad, defRad);
		sho.type = WEAK;
	} else if (sho.defDeg > maxDefDeg) {
		sho.shockDeg = 90.0;
		normal::updateStationNormal(sta);
		sho.type = NORMAL;
	} else {
		sho.shockDeg = 0.0;
		sho.type = DEBUG;
	}
	sho.pos = sta.pos + 1;
	sta.pos = sho.pos + 1;
	getA_fromMdot(sta);
}

void solveExpFan(Station &sta, ExpansionFan &fan) {
	float Arad = fan.defDeg * radInDeg;
	fan.degMu1 = oblique::radMachAngle(sta) * degInRad;
	prandtlMeyer::updateStationPM(sta, Arad);
	fan.degMu2 = oblique::radMachAngle(sta) * degInRad;
	fan.pos = sta.pos + 1;
	sta.pos = fan.pos + 1;
	getA_fromMdot(sta);
}

void mixStationFlows(Station &sta, Station &add) {
	double mdotOut = sta.mdot + add.mdot;
	double Aout = sta.A + add.A;
	//double Qsta = sta.mdot / sta.rho;
	//double Qadd = add.mdot / add.rho;
	double Ksta = 0.5 * sta.mdot * pow(sta.V, 2);
	double Kadd = 0.5 * add.mdot * pow(add.V, 2);
	double Kout = Ksta + Kadd;
	double Vout = sqrt((2 * Kout) / mdotOut);
	double Qout = Aout * Vout;
	double rhoOut = mdotOut / Qout;
	double uRMSsta = sqrt(3 * RsAir * sta.T);
	double uRMSadd = sqrt(3 * RsAir * add.T);
	double nSta = sta.mdot / molAir;
	double nAdd = add.mdot / molAir;
	double nOut = nSta + nAdd;
	double uRMSout = sqrt(((nSta * pow(uRMSsta, 2)) + (nAdd * pow(uRMSadd, 2))) / nOut);
	double Tout = pow(uRMSout, 2) / (3 * RsAir);
	double Pout = RsAir * Tout * rhoOut;
	// all these doubles are performant enough for now, optimise later
	sta.A = Aout;
	sta.V = Vout;
	sta.mdot = mdotOut;
	sta.rho = rhoOut;
	sta.T = Tout;
	sta.P = Pout;
	calImp::getGam(sta);
	calImp::getCp(sta);
	isen::getMach(sta);
	isen::getPt_fromP(sta);
	isen::getTt_fromT(sta);
	sta.pos = add.pos + 1;
}

// TODO probably create structs for intake, diffuser, combustor, convergingThroat, ejector, compressor, turbine

void subsonicCombustion(Station &sta, float &fuel, float &maxTemp) {
	float Ttprev;
	float TtMax = sta.Tt / rayleigh::Tt_Ttstar(sta, sta.M);
	cout << "\nTtMax = " << TtMax << "\n";
	float &wipTt = Ttprev;
	while (wipTt <= TtMax) {
		wipTt = sta.Tt;
		rayleigh::updateStation_plusMstep(sta, 0.0005);
		getA_fromMdot(sta);
		fuel += ((sta.Tt - wipTt) * sta.Cp * sta.mdot) / 119600000;
		if (sta.T >= maxTemp) {
			break;
		}
	}
	sta.pos += 1;
}

const float diffuserLoss = 0.99995;
// diffuser loss will one day be replaced with a more accurate Reynolds number based thingy
void subsonicDiffuser(Station &sta, float &Aout) {
	while (sta.A <= Aout) {
		sta.M -= 0.001;
		sta.Pt *= diffuserLoss;
		sta.Tt *= diffuserLoss;
		isen::getP_fromPt(sta);
		isen::getT_fromTt(sta);
		isen::getRho_fromPT(sta);
		calImp::getGam(sta);
		calImp::getCp(sta);
		isen::getPt_fromP(sta);
		isen::getTt_fromT(sta);
		isen::getVelocity(sta);
		getA_fromMdot(sta);
		//cout << "\ndiffuser debug: M=" << sta.M << " P=" << sta.P << " T=" << sta.T << " rho=" << sta.rho << " gam=" << sta.gam << " Pt=" << sta.Pt << " Tt=" << sta.Tt << " A=" << sta.A << "\n";
	}
	sta.pos += 1;
}

const float throatLoss = 0.98;
// same boundary layer consideration will be added here later. for ease of calculation I'm just going with a single step
void convergingThroat(Station &sta) {
	sta.M = 1.0;
	sta.Pt *= throatLoss;
	sta.Tt *= throatLoss;
	isen::getP_fromPt(sta);
	isen::getT_fromTt(sta);
	isen::getRho_fromPT(sta);
	calImp::getGam(sta);
	calImp::getCp(sta);
	isen::getPt_fromP(sta);
	isen::getTt_fromT(sta);
	isen::getVelocity(sta);
	getA_fromMdot(sta);
	sta.pos += 1;
}

const float ejectorLoss = diffuserLoss;
// same here, also one day vector stuff to create a pressure distribution thingy
void supersonicEjector(Station &sta, float &Aout, float &Pamb) {
	while (sta.A <= Aout) {
		sta.M += 0.001;
		sta.Pt *= ejectorLoss;
		sta.Tt *= ejectorLoss;
		isen::getP_fromPt(sta);
		isen::getT_fromTt(sta);
		isen::getRho_fromPT(sta);
		calImp::getGam(sta);
		calImp::getCp(sta);
		isen::getPt_fromP(sta);
		isen::getTt_fromT(sta);
		isen::getVelocity(sta);
		getA_fromMdot(sta);
		//cout << "\nejector debug: M=" << sta.M << " P=" << sta.P << " T=" << sta.T << " rho=" << sta.rho << " gam=" << sta.gam << " Pt=" << sta.Pt << " Tt=" << sta.Tt << " A=" << sta.A << "\n";

		if (sta.P <= Pamb) {
			break;
		}
	}
	sta.pos += 1;
}

void supersonicIntake(Station &sta, float &defAngle) {
	Shock sho;
	sho.defDeg = defAngle;
	while (sho.type != NORMAL) {
		solveShock(sta, sho);
		if (sta.M <= 1.0) {
			break;
		}
	}
	sta.pos = 1; // for now
}

// this seems cooked probably try to rewrite
void simpleCompressor(Station &sta, float &Pratio, float &PowerReq, float &eff) {
	double a2gam = (sta.gam - 1) / sta.gam;
	float prevPt = sta.Pt;
	float prevTt = sta.Tt;
	sta.Tt = sta.Tt * pow(Pratio, a2gam);
	isen::getT_fromTt(sta);
	calImp::getGam(sta);
	calImp::getCp(sta);
	isen::getMach(sta);
	aCW = (sta.gam - 1) / sta.gam;
	PowerReq = (((sta.Cp * prevTt) / eff) * (pow(Pratio, aCW) - 1)) * sta.mdot;
	sta.Pt = Pratio * prevPt;
	sta.Tt = prevTt * pow(Pratio, aCw);
	isen::getP_fromPt(sta);
	isen::getT_fromTt(sta);
	isen::getRho_fromPT(sta);
	calImp::getGam(sta);
	calImp::getCp(sta);
	isen::getMach(sta);
	getA_fromMdot(sta);
}

void simpleTurbine(Station &sta, float &PowerReq, float &eff) {
}
