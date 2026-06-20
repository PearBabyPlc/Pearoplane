//typedef struct ISA *ISAptr;
//for some fucking reason I can't get the typedef to work

struct ISA {
	float alt;
	float H;
	float T;
	float P;
	float rho;
	//float RH;
};

void getISA(struct ISA *ptr);

//void updateISA_rhoHumidity(struct ISA *ptr);
//i'll waste my time on this later
