void printStation(Station &sta, string &print) {
	print.append("\nStation pos=");
	print.append(to_string(sta.pos));
	print.append(" M=");
	print.append(to_string(sta.M));
	print.append(" P=");
	print.append(to_string(sta.P));
	print.append(" T=");
	print.append(to_string(sta.T));
	print.append(" rho=");
	print.append(to_string(sta.rho));
	print.append(" gam=");
	print.append(to_string(sta.gam));
	print.append(" Cp=");
	print.append(to_string(sta.Cp));
	print.append(" Pt=");
	print.append(to_string(sta.Pt));
	print.append(" Tt=");
	print.append(to_string(sta.Tt));
	print.append(" V=");
	print.append(to_string(sta.V));
	print.append("\n");
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

void getAmbientStation(Station &sta, float altitude, float velocity) {
	getISA::ISA isaStruct;
	isaStruct.alt = altitude;
	getISA::getPTrho(isaStruct);
	getISA::addISAtoStation(sta, isaStruct);
	calImp::getGam(sta);
	calImp::getCp(sta);
	sta.V = velocity;
	sta.M = sta.V / sqrt(RsAir * sta.gam * sta.T);
	isen::getTt_fromT(sta);
	isen::getPt_fromP(sta);
	sta.pos = 0;
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
}

void solveFan(Station &sta, ExpansionFan &fan) {
	float Arad = fan.defDeg * radInDeg;
	fan.degMu1 = oblique::radMachAngle(sta) * degInRad;
	prandtlMeyer::updateStationPM(sta, Arad);
	fan.degMu2 = oblique::radMachAngle(sta) * degInRad;
	fan.pos = sta.pos + 1;
	sta.pos = fan.pos + 1;
}
