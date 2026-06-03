import math
import numpy as np
import formulaegg as egg
import solvers as solve
import models

width = 10
height = 2
aDeg = 10
dDeg = 30
geomIn = (width, height, aDeg, dDeg)

M0 = 2
Q = 30000
LHV = 119600000


R = 1
n = 4
bypRfull = 0.1
bypRfan = 0.8
LP_PR = 3
HP_PR = 8
TlimMCC = 1600
TlimAB = 2400
compEff = 0.9
turbEff = 0.9
backPressureMargin = 0.9
jetParams = (R, n, bypRfull, bypRfan, LP_PR, HP_PR, TlimMCC, TlimAB, compEff, turbEff, backPressureMargin)

def readStation(x, s):
    print(s)
    print("Mach:", x[0])
    V = x[0] * egg.SoS(x[2])
    print("V (m/s):", V)
    print("P (Pa):", x[1])
    print("T (K):", x[2])
    print("rho (kg/m3):", x[3])
    print("gamma:", x[4])
    pass

def PeregrinePropulsion(geomIn, M0, Q, readouts):
    H = geomIn[1]
    outboard = geomIn[0]
    aDeg = geomIn[2]
    dDeg = geomIn[3]
    MlimitAllComb = 0.8
    
    inletDict = models.supersonicInlet(M0, Q, H, aDeg, outboard, 4)
    aero = inletDict.get("aerodynamics")
    perf = inletDict.get("performance")
    geom = inletDict.get("geometry")
    stations = inletDict.get("stations")
    liftTons = perf.get("liftTotal") / 9806.65

    Adiffuser = 4
    stationNormalShock = stations[-1]

    diffXXYYs, diffuserThrust, stationOut, diffDots, mdotDiffOut, AdiffOut = models.subsonicDiffuser(stations[-1], Adiffuser, H, dDeg, geom)
    stations.append(stationOut)

    maxA = 10
    stationABout, mdotABout, mdotABH2, ABAout = solve.Combustor(stations[-1], mdotDiffOut, MlimitAllComb, 4000, LHV, maxA)

    backPressureMargin = jetParams[10]
    stationThroat, stationOut, Athroat, Vthroat, Aout, Vout = solve.simpleEjector(stationABout, stations[0], mdotABout, maxA, backPressureMargin)
    Vin = egg.SoS(stations[0][2]) * stations[0][0]
    mdotIn = perf.get("mdotIn")
    mdotOut = mdotABout
    Fthrust = solve.generalThrust(mdotIn, mdotOut, Vin, Vout, stations[0][1], stationOut[1], Aout)
    FkN = Fthrust / 1000
    mdotFuel = mdotABH2
    Vex = Fthrust / mdotFuel
    Isp = Vex / 9.80665
    Ain = H * outboard

    ### final prints ###

    print("==================================================")
    print("PERFORMANCE")
    print()
    print("Altitude (m):", aero.get("altitude"))
    print("Mach", stations[0][0])
    print("Thrust (kN):", FkN)
    print("Isp (m/s):", Vex)
    print("Isp (sec):", Isp)
    print()
    print("==================================================")
    
    if readouts == False:
        pass
    else:
        print("MASS FLOWS")
        print("mdot in (kg/s):", mdotIn)
        print("mdot diffuser (kg/s):", mdotDiffOut)
        print("mdot out", mdotABout)
        print()
        print("==================================================")
        print("AREAS")
        print()
        print("A in (m2):", Ain)
        print("Adiffuser", AdiffOut)
        print("A AB out", ABAout)
        print("A throat", Athroat)
        print("A out", Aout)
        print()
        print("==================================================")
        print("STATIONS")
        print()
        readStation(stations[0], "Ambient")
        print()
        readStation(stationNormalShock, "Normal shock")
        print()
        readStation(stations[-1], "Post diffuser")
        print()
        readStation(stationABout, "Post afterburner")
        print()
        readStation(stationThroat, "Ejector throat")
        print()
        readStation(stationOut, "Ejector out")
        print()
        print("==================================================")
    output = (stations[0][0], aero.get("altitude"), liftTons, Fthrust, Vex)
    return output

def runJetModel():
    PeregrinePropulsion(geomIn, M0, Q, jetParams)
    pass
