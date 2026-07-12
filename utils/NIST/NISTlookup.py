import numpy as np
from os import getcwd
from scipy.interpolate import interp1d

# Methane data for 1 to 600 bar, 90.6941 to 625 Kelvin (no data above)
# Hydrogen data for 1 to 600 bar, 13.957 to 1000 Kelvin (no data above)
# Oxygen data for 1 to 600 bar, 54.361 to 600 Kelvin (limited for detail)

class NISTlookup:
    def __init__(self, molecule, T, P, rho, vol, U, H, S, Cv, Cp, SoS, JouleThomson, mu, thermalCond, phase):
        try:
            self.molecule = str(molecule)
            self.T = float(T)
            self.P = float(P)
            self.rho = float(rho)
            self.vol = float(vol)
            self.U = float(U) #internal energy
            self.H = float(H) #enthalpy
            self.S = float(S) #entropy
            self.Cv = float(Cv)
            self.Cp = float(Cp)
            self.SoS = float(SoS)
            self.JouleThomson = float(JouleThomson)
            self.mu = float(mu)
            self.thermalCond = float(thermalCond)
            self.phase = str(phase)
        except:
            self.molecule = molecule
            self.T = T
            self.P = P
            self.rho = rho
            self.vol = vol
            self.U = U
            self.H = H
            self.S = S
            self.Cv = Cv
            self.Cp = Cp
            self.SoS = SoS
            self.JouleThomson = JouleThomson
            self.mu = mu
            self.thermalCond = thermalCond
            self.phase = "ERROR"

    def __str__(self):
        returnString = str(self.molecule + ", T=" + str(self.T) + ", P=" + str(self.P) + ", Cp=" + str(self.Cp) + ", mu=" + str(self.mu) + ", phase=" + self.phase)
        return returnString

def initLookupNIST(molecule, molRange):
    output = []
    for Pbar in molRange:
        cwd = str(getcwd())
        filename = str(cwd + r"/NISTlookupData/" + str(molecule) + "_" + str(Pbar) + r"b.txt")
        lines = []
        with open(filename) as lookupPbar:
            for line in lookupPbar:
                lines.append(line)
        linesSplit = []
        for line in lines:
            lineSplit = line.split('\t')
            linesSplit.append(lineSplit)
        linesSplit.pop(0)
        for line in linesSplit:
            length = len(line)
            if length != 14:
                pass
                #print("FAIL:", line)
            else:
                phase = line[-1]
                #print("PASS:", line)
                phase.rstrip('\n')
                line.pop(-1)
                line.append(phase)
                lookupEntry = NISTlookup(molecule, line[0], line[1], line[2], line[3], line[4], line[5], line[6], line[7], line[8], line[9], line[10], line[11], line[12], line[13])
                if lookupEntry.phase == "ERROR":
                    pass
                else:
                    output.append(lookupEntry)
    return output

def lookup_PT(lookupTable, Pain, T):
    Pin = Pain / 100000
    P = round(Pin)
    lookupTable.sort(key=lambda x: abs(P - x.P))
    lookupTable.sort(key=lambda x: abs(T - x.T))
    return lookupTable[0]

def initTriprop():
