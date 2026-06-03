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

def PeregrinePropulsion(geomIn, M0, Q, jetParams, readouts):
    H = geomIn[1]
    outboard = geomIn[0]
    aDeg = geomIn[2]
    dDeg = geomIn[3]
    
    inletDict = models.supersonicInlet(M0, Q, H, aDeg, outboard, 4)
    aero = inletDict.get("aerodynamics")
    perf = inletDict.get("performance")
    geom = inletDict.get("geometry")
    stations = inletDict.get("stations")
    liftTons = perf.get("liftTotal") / 9806.65

    bypRfull = jetParams[2]
    bypRfan = jetParams[3]
    Ajet = math.pi * jetParams[0]**2
    Ajets = Ajet * jetParams[1]
    AbypFull = Ajets * bypRfull
    Adiffuser = Ajets + AbypFull
    stationNormalShock = stations[-1]

    diffXXYYs, diffuserThrust, stationOut, diffDots, mdotDiffOut, AdiffOut = models.subsonicDiffuser(stations[-1], Adiffuser, H, dDeg, geom)
    stations.append(stationOut)
    mdotJet = mdotDiffOut * (Ajets / AdiffOut)
    mdotBypFull = mdotDiffOut - mdotJet
    stationBypFull = stations[-1]

    LP_PR = jetParams[4]
    HP_PR = jetParams[5]
    TlimMCC = jetParams[6]
    TlimAB = jetParams[7]
    compEff = jetParams[8]
    turbEff = jetParams[9]
    MlimitAllComb = 0.8
    
    stationLPout, LPCpower, LPCAout = solve.Compressor(stations[-1], mdotJet, LP_PR, compEff)
    mdotCoreIn = mdotJet / (bypRfan + 1)
    mdotBypFan = mdotJet - mdotCoreIn
    AbypFan = LPCAout * (bypRfan / (bypRfan + 1))
    #AbypFan = LPCAout * (bypRfan + 1)
    AcompIn = LPCAout - AbypFan

    stationHPout, HPCpower, HPCAout = solve.Compressor(stationLPout, mdotCoreIn, HP_PR, compEff)

    stationMCCout, mdotMCCout, mdotLH2, MCCAout = solve.Combustor(stationHPout, mdotCoreIn, MlimitAllComb, TlimMCC, LHV, Ajets)

    stationHPTout, _, HPTAout = solve.Turbine(stationMCCout, mdotMCCout, HPCpower, turbEff)

    stationLPTout, _, LPTAout = solve.Turbine(stationHPTout, mdotMCCout, LPCpower, turbEff)

    if MCCAout >= LPTAout:
        print("WARNING MCC TOO CHUNGUS")
    
    stationJetMixed, mdotJetMixed = solve.mixFlows(stationLPout, stationLPTout, mdotBypFan, mdotMCCout)
    AjetMixed = LPTAout + AbypFan

    stationPreAB, mdotPreAB = solve.mixFlows(stationJetMixed, stationBypFull, mdotJetMixed, mdotBypFull)
    ApreAB = AjetMixed + AbypFull

    maxA = geomIn[0] * geomIn[1]
    if LPTAout >= (maxA / 2):
        print("WARNING TURBINE TOO CHUNGUS")
    stationABout, mdotABout, mdotABH2, ABAout = solve.Combustor(stationPreAB, mdotPreAB, MlimitAllComb, TlimAB, LHV, maxA)

    backPressureMargin = jetParams[10]
    stationThroat, stationOut, Athroat, Vthroat, Aout, Vout = solve.simpleEjector(stationABout, stations[0], mdotABout, maxA, backPressureMargin)
    Vin = egg.SoS(stations[0][2]) * stations[0][0]
    mdotIn = perf.get("mdotIn")
    mdotOut = mdotABout
    Fthrust = solve.generalThrust(mdotIn, mdotOut, Vin, Vout, stations[0][1], stationOut[1], Aout)
    FkN = Fthrust / 1000
    mdotFuel = mdotLH2 + mdotABH2
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
        print("mdot into LP fan", mdotJet)
        print("mdot full bypass", mdotBypFull)
        print("mdot into core", mdotCoreIn)
        print("mdot fan bypass", mdotBypFan)
        print("mdot MCC", mdotMCCout)
        print("mdot jet mixed", mdotJetMixed)
        print("mdot pre AB", mdotPreAB)
        print("mdot out", mdotABout)
        print()
        print("==================================================")
        print("AREAS")
        print()
        print("A in (m2):", Ain)
        print("Adiffuser", AdiffOut)
        print("A full bypass", AbypFull)
        print("A fan in", Ajets)
        print("A fan out", LPCAout)
        print("A fan bypass", AbypFan)
        print("A comp in", AcompIn)
        print("A comp out", HPCAout)
        print("A MCC out", MCCAout)
        print("A HPT out", HPTAout)
        print("A LPT out", LPTAout)
        print("A jet mixed", AjetMixed)
        print("A pre AB", ApreAB)
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
        readStation(stationLPout, "LP fan out")
        print()
        readStation(stationHPout, "HP fan out")
        print()
        readStation(stationMCCout, "MCC out")
        print()
        readStation(stationHPTout, "HP turbine out")
        print()
        readStation(stationLPTout, "LP turbine out")
        print()
        readStation(stationJetMixed, "Jet mixed")
        print()
        readStation(stationPreAB, "Pre afterburner")
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
