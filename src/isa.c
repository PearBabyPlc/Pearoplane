#include "math.h"
#include "isa.h"

const float rEarth = 6356766;
const float g = 9.80665;
const float molISA = 28.9644; //      these fuckin constants
const float RISA = 8314.46; //        kept conflicting with
const float RsISA = RISA / molISA; // formulaegg.c, so renamed

void getISA(ISA *ptr) {
	ptr->H = (rEarth * ptr->alt) / (rEarth + ptr->alt);

	if (ptr->H < 11000) {
		// troposphere, positive lapse rate (-temp with altitude)
		ptr->T = 288.15 + (-0.0065 * ptr->H);
		double Pexp = (g * molISA) / (RISA * -0.0065);
		double Pfrac = 288.15 / ptr->T;
		ptr->P = 101325.0 * pow(Pfrac, Pexp);
		// refT = 288.15
		// refP = 101325
		// refH = 0
		// refL = -0.0065	
	} else if (ptr->H >= 11000 && ptr->H < 20000) {
		// tropopause, constant temperature (zero lapse rate)
		ptr->T = 216.65;
		double Pexp = (-g * molISA * (ptr->H - 11000)) / (RISA * 216.65);
		ptr->P = 22632.6352091 * exp(Pexp);
		// refT = 216.65;
		// refP = 22632.6
		// refH = 11000
		// refL = ±0
	} else if (ptr->H >= 20000 && ptr->H < 32000) {
		// lower stratosphere, negative lapse rate (+temp with altitude)
		ptr->T = 216.65 + (0.001 * (ptr->H - 20000));
		double Pexp = (g * molISA) / (RISA * 0.001);
		double Pfrac = 216.65 / ptr->T;
		ptr->P = 5475.15769257 * pow(Pfrac, Pexp);
		// refT = 216.65
		// refP = 5475.16
		// refH = 20000
		// refL = +0.001
	} else if (ptr->H >= 32000 && ptr->H < 47000) {
		// upper stratosphere, negative lapse rate
		ptr->T = 228.65 + (0.0028 * (ptr->H - 32000));
		double Pexp = (g * molISA) / (RISA * 0.0028);
		double Pfrac = 228.65 / ptr->T;
		ptr->P = 868.088257003 * pow(Pfrac, Pexp);
		// refT = 228.65
		// refP = 868.088
		// refH = 32000
		// refL = +0.0028
	} else if (ptr->H >= 47000 && ptr->H < 51000) {
		// stratopause, constant temperature (zero lapse rate)
		ptr->T = 270.65;
		double Pexp = (-g * molISA * (ptr->H - 47000)) / (RISA * 270.65);
		ptr->P = 110.919037481 * exp(Pexp);
		// refT = 270.65
		// refP = 110.919
		// refH = 47000
		// refL = ±0
	} else if (ptr->H >= 51000 && ptr->H < 71000) {
		// lower mesosphere, positive lapse rate
		ptr->T = 270.65 + (-0.0028 * (ptr->H - 51000));
		double Pexp = (g * molISA) / (RISA * -0.0028);
		double Pfrac = 270.65 / ptr->T;
		ptr->P = 66.9471267903 * pow(Pfrac, Pexp);
		// refT = 270.65
		// refP = 66.9471
		// refH = 51000
		// refL = -0.0028
	} else if (ptr->H >= 71000 && ptr->H < 84852) {
		// upper mesosphere, positive lapse rate
		ptr->T = 214.65 + (-0.002 * (ptr->H - 71000));
		double Pexp = (g * molISA) / (RISA * -0.002);
		double Pfrac = 214.65 / ptr->T;
		ptr->P = 3.95709671594 * pow(Pfrac, Pexp);
		// refT = 214.65
		// refP = 3.9570967
		// refH = 71000
		// refL = -0.002
	} else if (ptr->H >= 84852 && ptr->H < 90000) {
		// mesopause, constant temperature
		ptr->T = 186.95;
		double Pexp = (-g * molISA * (ptr->H - 84852)) / (RISA * 186.95);
		ptr->P = 0.373462257454 * exp(Pexp);
		// refT = 186.95
		// refP = 0.373462257
		// refH = 84852
		// refL = ±0
	} else if (ptr->H >= 90000) {
		// thermosphere, negative lapse rate
		ptr->T = 186.95 + (0.004 * (ptr->H - 90000));
		double Pexp = (g * molISA) / (RISA * 0.004);
		double Pfrac = 186.95 / ptr->T;
		ptr->P = 0.145778536657 * pow(Pfrac, Pexp);
		// refT = 186.95
		// refP = 0.145778536657
		// refH = 90000
		// refL = +0.004
	}
	
	if (ptr->alt > 307446) {
		// set temp to constant 1000K, and P/rho to zero
		ptr->T = 1000.0; // later idk maybe implement solar radiation
		ptr->P = 0.0;
		ptr->rho = 0.0;
	} else {
		// otherwise calculate rho normally
		ptr->rho = ptr->P / (ptr->T * RsISA);
	}
}
