typedef struct {
	float alt;
	float H;
	float T;
	float P;
	float rho;
	//float RH;
} ISA;

void getISA(ISA *ptr);

//void updateISA_rhoHumidity(struct ISA *ptr);
//i'll waste my time on this later
