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
		//double ThetaTexp = exp(ThetaT);
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

namespace oblique {
}

namespace rayleigh {
}
