#include "math.h"
#include "helper.h"
#include "shomate.h"

void getShomate(CombProduct *ptr) {
}

// shomate equation in C
// t = sta->T / 1000;
// Cex = pow(t, 2);
// Dex = pow(t, 3);
// Cp = A + pow(B, t) + pow(C, Cex) + pow(D, Dex) + (E / Cex);
// Rs = Runiv / mol;
// Cv = Cp - Rs;
// gam = Cp / Cv;
//
// https://webbook.nist.gov/cgi/cbook.cgi?ID=C7732185&Mask=7
// https://webbook.nist.gov/cgi/cbook.cgi?ID=C124389&Mask=1
//
// H2O 500-1700
// A 30.092
// B 6.832514
// C 6.793435
// D -2.3448
// E 0.082139
//
// H2O 1700-6000
// A 41.96426
// B 8.622053
// C -1.49978
// D 0.098119
// E -11.15764
//
// CO2 298-1200
// A 24.99735
// B 55.18696
// C -33.69137
// D 7.948387
// E -0.136638
//
// CO2 1200-6000
// A 58.16639
// B 2.720074
// C -0.492289
// D 0.038844
// E -6.447293
