struct Station {
	float M = 0;
	float P = 101325;
	float T = 288.15;
	float rho = 1.225;
	float gam = 1.4;
	float Cp = 1005;
	float Pt = 101325;
	float Tt = 288.15;
};

namespace calImp {
	void getGam(Station &sta) {
		double ThetaT = Theta / sta.T;	
		double ThetaTexpsub1 = exp(ThetaT) - 1;
		double ThetaTexpsub1sq = pow(ThetaTexpsub1, 2);
		double ThetaTsq = pow(ThetaT, 2);
		sta.gam = 1 + ((gamDiatomic - 1) / (1 + (gamDiatomic - 1) * (ThetaTsq * (exp(ThetaT) / ThetaTexpsub1sq))));
	}
	void getCp(Station &sta) {
		double ThetaT = Theta / sta.T;
		double ThetaTexpsub1 = exp(ThetaT) - 1;
		double ThetaTexpsub1sq = pow(ThetaTexpsub1, 2);
		double ThetaTsq = pow(ThetaT, 2);
		sta.Cp = CpiAir * (1 + ((gamDiatomic - 1) / gamDiatomic) * (ThetaTsq * (exp(ThetaT) / ThetaTexpsub1sq)));
	}
}

namespace isen {
	void getTt_fromT(Station &sta) {
		sta.Tt = sta.T * (1 + ((sta.gam - 1) / 2) * pow(sta.M, 2));
	}
	void getPt_fromP(Station &sta) {
		double Ptow1 = 1 + ((sta.gam - 1) / 2) * pow(sta.M, 2);
		double Ptow2 = sta.gam / (sta.gam - 1);
		sta.Pt = sta.P * pow(Ptow1, Ptow2);
	}
	void getT_fromTt(Station &sta) {
		sta.T = sta.Tt / (1 + ((sta.gam - 1) / 2) * pow(sta.M, 2));
	}
	void getP_fromPt(Station &sta) {
		double Ptow1 = 1 + ((sta.gam - 1) / 2) * pow(sta.M, 2);
		double Ptow2 = sta.gam / (sta.gam - 1);
		sta.P = sta.Pt / pow(Ptow1, Ptow2);
	}
}

namespace normal {
}

const double degInRad = 180 / pi;
const double radInDeg = pi / 180;
void degToRad(float &angle) {
	angle *= radInDeg;
}
void radToDeg(float &angle) {
	angle *= degInRad;
}

namespace oblique {
	float radMachAngle(Station &sta) {
		return asin(1 / sta.M);
	}
	float radDeflectionAngle(Station &sta, float S) {
		double sinS = sin(S);
		double invCota = 1 / (tan(S) * ((((sta.gam + 1) * pow(sta.M, 2) / (2 * (pow(sta.M, 2) * pow(sinS, 2) - 1))) - 1)));
		return atan(invCota);
	}
	float maxDefRad(Station &sta) {
		double onePointFive = pow(sta.M, 2) - 1;
		return (4 / (3 * sqrt(3) * (sta.gam + 1))) * (pow(onePointFive, 1.5) / pow(sta.M, 2));
	}
	float radShockAngle(Station &sta, float A) {
		int minAngleDeg = floor(degInRad * radMachAngle(sta));
		float Adeg = degInRad * A;
		float wipA;
		float &wipAdeg = wipA;
		float iNext;
		float &iTarget = iNext;
		for (int i = minAngleDeg; i < 91; i++) {
			float degi = radInDeg * i;
			float tempi = radDeflectionAngle(sta, degi);
			wipAdeg = degInRad * tempi;
			iTarget = i;
			//std::cout << "\n s=" << i << " a=" << wipAdeg << " targ=" << Adeg << " diff=" << abs(Adeg - wipAdeg); debug
			if (abs(Adeg - wipAdeg) <= 1.0) {
				break;
			}
		}
		float Sdeg;
		float &wipS = Sdeg;
		for (int i = 0; i < 2001; i++) {
			wipS = iNext + (0.0005 * i);
			float degi = radInDeg * wipS;
			float tempi = radDeflectionAngle(sta, degi);
			wipAdeg = degInRad * tempi;
			//std::cout << "\n s=" << wipS << " a=" << wipAdeg << " targ=" << Adeg << " diff=" << abs(Adeg - wipAdeg); debug
			if (abs(Adeg - wipAdeg) <= 0.0005) {
				break;
			}
		}
		degToRad(wipS);
		return wipS;
	}
	void updateStationOblique(Station &sta, float S, float A) {
		double sinS = sin(S);
		double sinSsubA = sin(S - A);
		float pM = sta.M;
		double bM = (sta.gam - 1) * pow(pM, 2) * pow(sinS, 2) + 2;
		double cM = 2 * sta.gam * pow(pM, 2) * pow(sinS, 2) - (sta.gam - 1);
		sta.M = sqrt((bM / cM) / pow(sinSsubA, 2));

		sta.P *= (2 * sta.gam * pow(pM, 2) * pow(sinS, 2) - (sta.gam - 1)) / (sta.gam + 1);
		double gamPlusOne = sta.gam + 1;
		sta.T *= ((2 * sta.gam * pow(pM, 2) * pow(sinS, 2) - (sta.gam - 1)) * ((sta.gam - 1) * pow(pM, 2) * pow(sinS, 2) + 2)) / (pow(gamPlusOne, 2) * pow(pM, 2) * pow(sinS, 2));
		sta.rho *= ((sta.gam + 1) * pow(pM, 2) * pow(sinS, 2)) / ((sta.gam - 1) * pow(pM, 2) * pow(sinS, 2) + 2);
		double aPt = ((sta.gam + 1) * pow(pM, 2) * pow(sinS, 2)) / ((sta.gam - 1) * pow(pM, 2) * pow(sinS, 2) + 2);
		double bPt = sta.gam / (sta.gam - 1);
		double cPt = (sta.gam + 1) / (2 * sta.gam * pow(pM, 2) * pow(sinS, 2) - (sta.gam - 1));
		double dPt = 1 / (sta.gam - 1);
		sta.Pt *= pow(aPt, bPt) * pow(cPt, dPt);
		calImp::getGam(sta);
		isen::getTt_fromT(sta);
		calImp::getCp(sta);
	}
}

namespace rayleigh {
}
