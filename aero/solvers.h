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

void printStrVec(vector<string> &StrVec, string &print) {
	vector<string>::iterator i;
	for (i = StrVec.begin(); i != StrVec.end(); ++i) {
		print.append(*i);
		print.append("\n");
	}
}

void printIntVec(vector<int> &IntVec, string &print) {
	vector<int>::iterator i;
	for (i = IntVec.begin(); i != IntVec.end(); ++i) {
		print.append(to_string(*i));
		print.append("\n");
	}
}

void printFloatVec(vector<float> &FloatVec, string &print) {
	vector<float>::iterator i;
	for (i = FloatVec.begin(); i != FloatVec.end(); ++i) {
		print.append(to_string(*i));
		print.append("\n");
	}
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

void splitStationFlows(Station &sta, Station &byp, float &bypR) {
	byp.M = sta.M;
	byp.P = sta.P;
	byp.T = sta.T;
	byp.rho = sta.rho;
	byp.gam = sta.gam;
	byp.Cp = sta.Cp;
	byp.Pt = sta.Pt;
	byp.Tt = sta.Tt;
	byp.V = sta.V;
	float mdotIn = sta.mdot;
	sta.mdot = mdotIn / (bypR + 1.0);
	byp.mdot = (bypR * mdotIn) / (bypR + 1.0);
	getA_fromMdot(sta);
	getA_fromMdot(byp);
}

// TODO probably create structs for intake, diffuser, combustor, convergingThroat, ejector, compressor, turbine

struct Combustor {
	float fuelMdot = 0.0;
	float maxTemp = 1800.0; // highest reliable jet turbine inlet temp
	float TtMax;
	float maxMach = 0.9; // max mach for stable combustion, complete guesstimate, just here to prevent endless loop
	float fuelLHV = 119600000.0;
	int pos = 0;
};

// float &fuel float &maxTemp
void subsonicCombustion(Station &sta, Combustor &comb) {
	float Ttprev;
	comb.TtMax = sta.Tt / rayleigh::Tt_Ttstar(sta, sta.M);
	float &wipTt = Ttprev;
	while (wipTt <= comb.TtMax) {
		wipTt = sta.Tt;
		rayleigh::updateStation_plusMstep(sta, 0.0005);
		getA_fromMdot(sta);
		comb.fuelMdot += ((sta.Tt - wipTt) * sta.Cp * sta.mdot) / comb.fuelLHV;
		if (sta.T >= comb.maxTemp) {
			break;
		}
		if (sta.M > comb.maxMach) {
			break;
		}
	}
	comb.pos = sta.pos + 1;
	sta.pos = comb.pos + 1;
}

struct Diffuser {
	int pos = 0;
	float Aout = 0.0;
	float Mout = 0.01;
	float loss = 0.99995;
};
// diffuser loss will one day be replaced with a more accurate Reynolds number based thingy
void subsonicDiffuser(Station &sta, Diffuser &dif) {
	while (sta.A <= dif.Aout) {
		sta.M -= 0.001;
		sta.Pt *= dif.loss;
		sta.Tt *= dif.loss;
		isen::getP_fromPt(sta);
		isen::getT_fromTt(sta);
		isen::getRho_fromPT(sta);
		calImp::getGam(sta);
		calImp::getCp(sta);
		isen::getPt_fromP(sta);
		isen::getTt_fromT(sta);
		isen::getVelocity(sta);
		getA_fromMdot(sta);
		if (sta.M <= dif.Mout) {
			dif.Aout = sta.A;
			break;
		}
	}	
	dif.pos = sta.pos + 1;
	sta.pos = dif.pos + 1;
}

const float throatLoss = 0.98;
struct Throat {
	int pos = 0;
	float Mout = 1.0;
	float loss = 0.98;
};
// same boundary layer consideration will be added here later. for ease of calculation I'm just going with a single step
void convergingThroat(Station &sta, Throat &thr) {
	sta.M = thr.Mout;
	sta.Pt *= thr.loss;
	sta.Tt *= thr.loss;
	isen::getP_fromPt(sta);
	isen::getT_fromTt(sta);
	isen::getRho_fromPT(sta);
	calImp::getGam(sta);
	calImp::getCp(sta);
	isen::getPt_fromP(sta);
	isen::getTt_fromT(sta);
	isen::getVelocity(sta);
	getA_fromMdot(sta);
	thr.pos = sta.pos + 1;
	sta.pos = thr.pos + 1;
}

struct Ejector {
	int pos = 0;
	float Aout = 0.0;
	float Pamb = 101325;
	float loss = 0.99995;
};
// same here, also one day vector stuff to create a pressure distribution thingy
void supersonicEjector(Station &sta, Ejector &eje) {
	while (sta.A <= eje.Aout) {
		sta.M += 0.001;
		sta.Pt *= eje.loss;
		sta.Tt *= eje.loss;
		isen::getP_fromPt(sta);
		isen::getT_fromTt(sta);
		isen::getRho_fromPT(sta);
		calImp::getGam(sta);
		calImp::getCp(sta);
		isen::getPt_fromP(sta);
		isen::getTt_fromT(sta);
		isen::getVelocity(sta);
		getA_fromMdot(sta);
		if (sta.P <= eje.Pamb) {
			break;
		}
	}
	eje.pos = sta.pos + 1;
	sta.pos = eje.pos + 1;
}

struct IntakeStation {
	float P;
	float Q;
	float A;
};
struct IntakeShock {
	float shockDeg;
	bool axial;
};
struct Intake {
	map<int, IntakeStation> InStats;
	map<int, IntakeShock> InShocks;
	map<int, string> ItTypes;
	float defAngle;
};
void supersonicIntake(Station &sta, Intake &intake) {
	Shock sho;
	sho.defDeg = intake.defAngle;
	IntakeStation InAmb;
	InAmb.P = sta.P;
	InAmb.Q = isen::getDynamicP(sta);
	InAmb.A = sta.A;
	intake.InStats[sta.pos] = InAmb;
	intake.ItTypes[sta.pos] = "STAT";
	float PrevFlowDeg = 0.0;
	while (sho.type != NORMAL) {
		solveShock(sta, sho);
		IntakeStation InStat;
		IntakeShock InShock;
		InStat.P = sta.P;
		InStat.Q = isen::getDynamicP(sta);
		InStat.A = sta.A;
		InShock.shockDeg = sho.shockDeg;
		if (PrevFlowDeg == 0.0) {
			InShock.axial = true;
			PrevFlowDeg = intake.defAngle;
		} else {
			InShock.axial = false;
			PrevFlowDeg = 0.0;
		}
		intake.InStats[sta.pos] = InStat;
		intake.InShocks[sho.pos] = InShock;
		intake.ItTypes[sta.pos] = "STAT";
		intake.ItTypes[sho.pos] = "SHOCK";
		if (sta.M <= 1.0) {
			break;
		}
	}
}

struct SimpleComp {
	int pos = 0;
	float Pratio = 2.0;
	float Power = 0.0;
	float eff = 0.8;
};
void simpleCompressor(Station &sta, SimpleComp &comp) {
	double aTt2a = (sta.gam - 1) / sta.gam;
	float Tt2a = sta.Tt * pow(comp.Pratio, aTt2a);
	sta.T = Tt2a / isen::getTt_T(sta.gam, sta.M);
	calImp::getGam(sta);
	calImp::getCp(sta);
	isen::getMach(sta);
	aTt2a = (sta.gam - 1) / sta.gam;
	comp.Power = (sta.mdot * sta.Cp * sta.Tt * (pow(comp.Pratio, aTt2a) - 1)) / comp.eff;
	sta.Pt *= comp.Pratio;
	sta.Tt *= pow(comp.Pratio, aTt2a);
	isen::getP_fromPt(sta);
	isen::getT_fromTt(sta);
	isen::getRho_fromPT(sta);
	calImp::getGam(sta);
	calImp::getCp(sta);
	isen::getMach(sta);
	getA_fromMdot(sta);
	comp.pos = sta.pos + 1;
	sta.pos = comp.pos + 1;
}

struct SimpleTurb {
	int pos = 0;
	float Pratio = 0.5;
	float Power = 0.0;
	float TW = 0.0;
	float eff = 0.8;
};
void simpleTurbine(Station &sta, SimpleTurb &turb) {
	turb.TW = turb.Power / sta.mdot;
	double TPRaa = (turb.TW / (turb.eff * sta.Cp * sta.Tt)) + 1.0;
	double TPRab = sta.gam / (sta.gam - 1.0); //
	double TPRa = -pow(TPRaa, TPRab);
	float prevTt = sta.Tt;
	double TPRb = (sta.gam - 1) / sta.gam;
	float Tt4a = sta.Tt * pow(-TPRa, TPRb);
	sta.T = Tt4a / isen::getTt_T(sta.gam, sta.M);
	float prevGam = sta.gam;
	float prevCp = sta.Cp;
	calImp::getGam(sta);
	calImp::getCp(sta);
	sta.gam += prevGam;
	sta.Cp += prevCp;
	sta.gam *= 0.5;
	sta.Cp *= 0.5;
	isen::getMach(sta);
	TPRaa = (-turb.TW / (turb.eff * sta.Cp * prevTt)) + 1;
	TPRab = sta.gam / (sta.gam - 1);
	turb.Pratio = pow(TPRaa, TPRab);
	sta.Pt *= turb.Pratio;
	TPRb = (sta.gam - 1) / sta.gam;
	sta.Tt *= pow(turb.Pratio, TPRb);
	isen::getT_fromTt(sta);
	isen::getP_fromPt(sta);
	isen::getRho_fromPT(sta);
	calImp::getGam(sta);
	calImp::getCp(sta);
	isen::getMach(sta);
	getA_fromMdot(sta);
	turb.pos = sta.pos + 1;
	sta.pos = turb.pos + 1;
}
