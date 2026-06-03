import math
from random import choice
import formulaegg as egg
import solvers as solve
import numpy as np
from matplotlib.patches import Polygon

Mdiff = 0.001
LHV = 119600000
MlimitAllComb = 0.9
ramResidence = 0.015 #15ms residence time
colors = ["b", "c", "g", "m", "r", "y"]

def readStation(x):
    print("M:", x[0])
    print("P (Pa):", x[1])
    print("T (K):", x[2])
    print("rho (kg/m3):", x[3])
    print("gamma:", x[4])
    Cp = egg.Cp(x[2])
    print("Cp (J/kg-K):", Cp)
    Ptbar = x[5] / 100000
    print("Pt (Bar):", Ptbar)
    print("Tt (K):", x[6])
    pass

def afterburningTurbofan(LPCpr, HPCpr, Ain, bypR, TlimMCC, TlimAB, compEff, turbEff, stationIn, afterburning, maxA):
    Vin = egg.SoS(stationIn[2]) * stationIn[0]
    mdotIn = Vin * Ain * stationIn[3]
    print("AFTERBURNING TURBOFAN")
    print("Vin =", Vin)
    print("mdot into LP fan =", mdotIn)
    print("Ain", Ain)
    print()
    
    stationLPout, LPCpower, LPCAout = solve.Compressor(stationIn, mdotIn, LPCpr, compEff)
    mdotCoreIn = mdotIn / (bypR + 1)
    mdotBypass = mdotIn - mdotCoreIn
    print()
    print("mdot into core (HP comp) =", mdotCoreIn)
    print("mdot bypassed =", mdotBypass)
    #mdotCheck = mdotIn - (mdotCoreIn + mdotBypass)
    #print("check", mdotCheck)
    print()
    stationHPout, HPCpower, HPCAout = solve.Compressor(stationLPout, mdotCoreIn, HPCpr, compEff)
    print()
    stationMCCout, mdotMCCout, mdotLH2, MCCAout = solve.Combustor(stationHPout, mdotCoreIn, MlimitAllComb, TlimMCC, LHV, maxA)
    print()
    stationHPTout, _, HPTAout = solve.Turbine(stationMCCout, mdotMCCout, HPCpower, turbEff)
    print()
    stationLPTout, _, LPTAout = solve.Turbine(stationHPTout, mdotMCCout, LPCpower, turbEff)
    print()
    stationPreAB, mdotPreAB = solve.mixFlows(stationLPout, stationLPTout, mdotBypass, mdotMCCout)
    if afterburning == True:
        stationABout, mdotABout, mdotABH2, ABAout = solve.Combustor(stationPreAB, mdotPreAB, MlimitAllComb, TlimAB, LHV, maxA)
    else:
        pass
    print("==================================================")
    print("afterburning turbofan stations")
    print()
    print("LP fan in")
    print("mdot =", mdotIn)
    print("A =", Ain)
    readStation(stationIn)
    print()
    print("LP fan out")
    print("mdot =", mdotIn)
    print("A =", LPCAout)
    readStation(stationLPout)
    print()
    print("Bypass air")
    print("mdot =", mdotBypass)
    VLPout = stationLPout[0] * egg.SoS(stationLPout[2])
    Abypass = mdotBypass / (VLPout * stationLPout[3])
    print("V =", VLPout)
    print("A =", Abypass)
    print()
    print("HP compressor in")
    print("mdot =", mdotCoreIn)
    AHPin = mdotCoreIn / (VLPout * stationLPout[3])
    print("A =", AHPin)
    print()
    print("HP compressor out")
    print("mdot =", mdotCoreIn)
    print("A =", HPCAout)
    readStation(stationHPout)
    print()
    print("Main combustor out")
    print("mdot total =", mdotMCCout)
    print("mdot LH2 =", mdotLH2)
    print("A =", MCCAout)
    readStation(stationMCCout)
    print()
    print("HP turbine out")
    print("mdot =", mdotMCCout)
    print("A =", HPTAout)
    readStation(stationHPTout)
    print()
    print("LP turbine out")
    print("mdot =", mdotMCCout)
    VLPTout = stationLPTout[0] * egg.SoS(stationLPTout[2])
    print("V =", VLPTout)
    print("A =", LPTAout)
    readStation(stationLPTout)
    print()
    print("Bypass+core mixing, pre afterburner")
    print("mdot =", mdotPreAB)
    VpreAB = stationPreAB[0] * egg.SoS(stationPreAB[2])
    ApreAB = mdotPreAB / (VpreAB * stationPreAB[3])
    print("V =", VpreAB)
    print("A =", ApreAB)
    readStation(stationPreAB)
    print()
    print()
    print("Afterburner out")
    print("mdot total =", mdotABout)
    print("mdot LH2 =", mdotABH2)
    print("A =", ABAout)
    readStation(stationABout)
    print("==================================================")
    stationOut = stationABout
    mdotOut = mdotPreAB
    mdotABH2 = 0
    mdotFuel = mdotLH2 + mdotABH2
    return stationOut, mdotOut, mdotFuel
    
def ramjet(stationIn, diff4XYs, H, Tlimit, Mlimit, mdotDiff):
    Min = stationIn[0]
    Pin = stationIn[1]
    Tin = stationIn[2]
    rhoIn = stationIn[3]
    SoSin = egg.SoS(Tin)
    ramXY1 = diff4XYs[2]
    ramXY2 = diff4XYs[3]
    ramA = abs(ramXY1[1] - ramXY2[1])
    Vin = Min * SoSin
    #Ain = ramA * H
    mdotIn = mdotDiff
    Ain = mdotDiff / (Vin * rhoIn)
    print("mdotIn ramjet =", mdotIn)
    stations = [stationIn]
    Cpin = egg.Cp(Tin)
    Cplist = [Cpin]
    cumLH2 = 0
    mdot = mdotIn
    M = Min
    T = Tin
    Vlist = [Vin]
    Tlist = [Tin]
    rhoList = [rhoIn]
    while (M <= Mlimit) and (T <= Tlimit):
        stationIn = stations[-1]
        M = stationIn[0]
        P = stationIn[1]
        T = stationIn[2]
        rho = stationIn[3]
        gam = stationIn[4]
        Pt = stationIn[5]
        Tt = stationIn[6]
        Ttstar = egg.Ttstar_Tt(gam, M) * Tt
        Ptstar = egg.Ptstar_Pt(gam, M) * Pt
        Tstar = egg.Tstar_T(gam, M) * T
        Pstar = egg.Pstar_P(gam, M) * P
        M += Mdiff
        P = Pstar / egg.Pstar_P(gam, M)
        T = Tstar / egg.Tstar_T(gam, M)
        rho = egg.getRho(P, T)
        Pt = Ptstar / egg.Ptstar_Pt(gam, M)
        prevTt = Tt
        Tt = Ttstar / egg.Ttstar_Tt(gam, M)
        deltaTt = abs(Tt - prevTt)
        gam = egg.gam(T)
        stationOut = (M, P, T, rho, gam, Pt, Tt)
        Cp = egg.Cp(T)
        Cplist.append(Cp)
        heating_kgAir = deltaTt * Cp
        kgLH2 = (heating_kgAir * mdot) / LHV
        cumLH2 += kgLH2
        mdot += kgLH2
        stations.append(stationOut)
        Vout = egg.SoS(T) * M
        Vlist.append(Vout)
        Tlist.append(T)
        rhoList.append(rho)
    stationOut = stations[-1]
    cumV = 0
    for x in Vlist:
        cumV += x
    averageV = cumV / len(Vlist)
    lenCombustor = averageV * ramResidence

    #cumT = 0
    #for x in Tlist:
    #    cumT += x
    #averageT = cumT / len(Tlist)
    ### TRY TO FIGURE OUT CHARACTERISTIC LENGTH SHIT LATER ###
    #cumRho = 0
    #for x in rhoList:
    #    cumRho += x
    #averageRho = cumRho / len(rhoList)
    
    print("combustor length =", lenCombustor)
    print("combustor mdot =", mdot)
    print("average velocity =", averageV)
    #print("average temperature =", averageT)
    #print("average density =", averageRho)
    print("velocity out =", Vlist[-1])
    #print("temperature out =", stationOut[2])
    #print("density out =", stationOut[3])
    Aout = mdot / (Vlist[-1] * stationOut[3])
    #averageArea = (Ain + Aout) / 2
    #print("area in avg out", Ain, averageArea, Aout)
    print("combustor Aout =", Aout)
    ramX3 = ramXY1[0] + lenCombustor
    ramX4 = ramXY2[0] + lenCombustor
    ramY3 = ramXY2[1] - Aout
    ramY4 = ramXY2[1]
    ramXY3 = (ramX3, ramY3)
    ramXY4 = (ramX4, ramY4)
    ram4XYs = (ramXY1, ramXY2, ramXY4, ramXY3)
    rand = choice(colors)
    polyRam = Polygon(ram4XYs, facecolor=rand)
    return stationOut, cumLH2, mdot, ram4XYs, polyRam

def linearEjector(stationIn, ram4XYs, H, dDeg, station0, outboard):
    d = math.radians(dDeg)
    ramXYtop = ram4XYs[2]
    ramXYbtm = ram4XYs[3]
    P0 = station0[1]
    V0 = egg.SoS(station0[2]) * station0[0]
    print("P 0 in", P0, stationIn[1])
    print("V0", V0)

    Ain = abs(ramXYtop[1] - ramXYbtm[1]) * H

    station = stationIn
    M = station[0]
    P = station[1]
    T = station[2]
    rho = station[3]
    gam = station[4]
    Pt = station[5]
    Tt = station[6]
    mdot = Ain * egg.SoS(T) * M * rho
    print("mdot =", mdot)
    stations = [station]
    ejectorXs = [ramXYbtm[0]]
    ejectorYs = [ramXYbtm[1]]
    ejectorXXYYs = []
    dragCon = 0
    while M <= 1:
        Pprev = P
        M += Mdiff
        P = egg.P_Pt(gam, M) * Pt
        T = egg.T_Tt(gam, M) * Tt
        rho = egg.getRho(P, T)
        gam = egg.gam(T)
        Pt = egg.Pt_P(gam, M) * P
        Tt = egg.Tt_T(gam, M) * T
        A = mdot / (egg.SoS(T) * M * rho)
        xPrev = ejectorXs[-1]
        yPrev = ejectorYs[-1]
        y = ramXYtop[1] - (A / H)
        x = (abs(ramXYbtm[1] - y) / math.tan(d)) + ramXYbtm[0]
        lenSeg = math.sqrt((abs(x - xPrev))**2 + (abs(y - yPrev))**2)
        print(x, y, lenSeg)
        XX = (xPrev, x)
        YY = (yPrev, y)
        ejectorXs.append(x)
        ejectorYs.append(y)
        XXYY = (XX, YY)
        ejectorXXYYs.append(XXYY)
        dragSeg = lenSeg * H * ((P + Pprev) / 2) * math.sin(d)
        dragCon += dragSeg
        stationOut = (M, P, T, rho, gam, Pt, Tt)
        stations.append(stationOut)
    stationThroat = stations[-1]
    V = egg.SoS(T) * M
    Vlist = []
    Plist = []
    xThroat = ejectorXs[-1]
    yThroat = ejectorYs[-1]
    thrustDiv = 0
    rand = choice(colors)
    print()
    polyCon = Polygon([ramXYbtm, ramXYtop, (xThroat, ramXYtop[1]), (xThroat, yThroat)], facecolor=rand)
    Amax = H * outboard
    while (y >= 0) and (A <= Amax):
        if P <= P0:
            break
        elif y >= 0:
            M += Mdiff
            P = egg.P_Pt(gam, M) * Pt
            T = egg.T_Tt(gam, M) * Tt
            rho = egg.getRho(P, T)
            gam = egg.gam(T)
            Pt = egg.Pt_P(gam, M) * P
            Tt = egg.Tt_T(gam, M) * T
            A = mdot / (egg.SoS(T) * M * rho)
            xPrev = ejectorXs[-1]
            yPrev = ejectorYs[-1]
            y = ramXYtop[1] - (A / H)
            x = (abs(yThroat - y) / math.tan(d)) + xThroat
            lenSeg = math.sqrt((abs(x - xPrev))**2 + (abs(y - yPrev))**2)
            print(x, y, lenSeg)
            XX = (xPrev, x)
            YY = (yPrev, y)
            ejectorXs.append(x)
            ejectorYs.append(y)
            XXYY = (XX, YY)
            ejectorXXYYs.append(XXYY)
            #thrustSeg = lenSeg * H * ((P + Pprev) / 2) * math.sin(d)
            #too optimistic, changed to
            thrustSeg = lenSeg * H * P * math.sin(d)
            thrustDiv += thrustSeg
            stationOut = (M, P, T, rho, gam, Pt, Tt)
            stations.append(stationOut)
            V = egg.SoS(T) * M
        else:
            break
    print("drag converging =", dragCon)
    print("thrust diverging =", thrustDiv)
    print("Vex =", V)
    print("Pex =", P)
    rand = choice(colors)
    polyDiv = Polygon([(xThroat, ramXYtop[1]), (xThroat, yThroat), (ejectorXs[-1], ejectorYs[-1]), (ejectorXs[-1], ramXYtop[1])], facecolor=rand)
    polygs = (polyCon, polyDiv)
    return stationThroat, stations[-1], ejectorXXYYs, polygs, V, dragCon, thrustDiv

def subsonicDiffuser(stationIn, AoutDesign, H, dDeg, inletGeom):
    d = math.radians(dDeg)
    dotXYs = inletGeom.get("dotXYs")
    dotXYs.sort()
    throatXYs = (dotXYs[-2], dotXYs[-1])
    throatXdiff = abs(throatXYs[0][0] - throatXYs[1][0])
    throatYdiff = abs(throatXYs[0][1] - throatXYs[1][1])
    throatYs = [throatXYs[0][1], throatXYs[1][1]]
    throatXs = [throatXYs[0][0], throatXYs[1][0]]
    throatYs.sort()
    throatXs.sort()
    #print("throatXs:", throatXs)
    #print("throatYs:", throatYs)
    throatA = math.sqrt(throatXdiff**2 + throatYdiff**2)
    SoSin = egg.SoS(stationIn[2])
    Vin = stationIn[0] * SoSin
    Ain = throatA * H
    mdot = Ain * stationIn[3] * Vin
    #print("normal shock area =", Ain)
    #print("SoS =", SoSin)
    #print("velocity =", Vin)
    #print("normal shock mdot =", mdot)
    #print()
    station = stationIn
    stations = [station]
    M = station[0]
    P = station[1]
    T = station[2]
    rho = station[3]
    gam = station[4]
    Pt = station[5]
    Tt = station[6]
    A_Astar = egg.A_Astar(gam, M)
    A = Ain #this was where the low mdot error was coming from
    Astar = A / A_Astar
    diffMax = throatYs[-1]
    diffXs = [throatXs[-1]]
    diffYs = [throatYs[0]]
    diffXXYYs = []
    thrustDiffuser = 0
    y = diffMax - A
    while (A < AoutDesign):
        Pprev = P
        M -= Mdiff
        A = egg.A_Astar(gam, M) * Astar
        P = egg.P_Pt(gam, M) * Pt
        T = egg.T_Tt(gam, M) * Tt
        rho = egg.getRho(P, T)
        gam = egg.gam(T)
        Pt = egg.Pt_P(gam, M) * P
        Tt = egg.Tt_T(gam, M) * T
        xPrev = diffXs[-1]
        yPrev = diffYs[-1]
        y = diffMax - A
        x = (abs(y - throatYs[0]) / math.tan(d)) + throatXs[-1]
        lenDiffSeg = math.sqrt((abs(x - diffXs[-1]))**2 + (abs(y - diffYs[-1]))**2)
        XX = (xPrev, x)
        YY = (yPrev, y)
        diffXs.append(x)
        diffYs.append(y)
        XXYY = (XX, YY)
        #print(XXYY)
        diffXXYYs.append(XXYY)
        thrustDiffSeg = lenDiffSeg * H * ((P + Pprev) / 2) * math.sin(d)
        thrustDiffuser += thrustDiffSeg
        stationOut = (M, P, T, rho, gam, Pt, Tt)
        stations.append(stationOut)
        #print(A, AoutDesign)
    #print("diffuser thrust =", thrustDiffuser)
    #print("fsadfdsfaef")
    #print(diffXXYYs)
    stationOut = stations[-1]
    Aout = A
    SoSout = egg.SoS(stationOut[2])
    Vout = SoSout * stationOut[0]
    mdotOut = Vout * Aout * stationOut[3]
    print("diffuser area =", Aout)
    #print("SoS =", SoSout)
    print("velocity =", Vout)
    print("diffuser mdot =", mdotOut)
    diff4XYs = [(throatXs[-1], throatYs[-1]),
                (throatXs[-1], throatYs[0]),
                (x, y), (x, throatYs[-1])]
    print("fsadfdsfaef")
    print(diffXXYYs)
    return diffXXYYs, thrustDiffuser, stationOut, diff4XYs, mdotOut, Aout

def supersonicInlet(M0, Q, H, aDeg, outboard, Mdesign):
    ISA, altitude = solve.getISA_MQ(M0, Q)
    P0 = ISA[0]
    T0 = ISA[1]
    rho0 = ISA[2]
    gam0 = egg.gam(T0)
    Pt0 = egg.Pt_P(gam0, M0) * P0
    Tt0 = egg.Tt_T(gam0, M0) * T0

    SoSin = egg.SoS(T0)
    Vin = M0 * SoSin
    Ain = H * outboard
    mdot = Ain * rho0 * Vin
    #print("ambient inlet area =", Ain)
    #print("SoS =", SoSin)
    #print("velocity =", Vin)
    #print("ambient inlet mdot =", mdot)

    M = M0
    P = P0
    T = T0
    rho = rho0
    gam = gam0
    Pt = Pt0
    Tt = Tt0

    aListRad = []
    sListRad = []
    aList = []
    sList = []
    Mlist = [M]
    Plist = [P]
    Tlist = [T]
    rhoList = [rho]
    gamList = [gam]
    Ptlist = [Pt]
    Ttlist = [Tt]
    station0 = (M0, P0, T0, rho0, gam0, Pt0, Tt0)
    stations = [station0]

    a = math.radians(aDeg)
    it_a_s_listDeg = []
    it_a_s_list = []
    it = -1
    while M > 1:
        aLim = egg.aLimit(gam, M)
        it += 1
        if (a < aLim) and (M > 1):
            ob = solve.oblique_a(a, gam, M)
            M = ob[0]
            P = P * ob[1]
            T = T * ob[2]
            rho = rho * ob[3]
            gam = egg.gam(T)
            Pt = Pt * ob[4]
            Tt = egg.Tt_T(gam, M) * T
            #print("Oblique shock")
            s = ob[-1]
            sDeg = math.degrees(s)
            #print("deflection (deg) =", aDeg)
            #print("shock angle (deg) =", sDeg)
            #print("Mout =", M)
            #print("P =", P)
            #print("T =", T)
            #print("rho =", rho)
            #print("gam =", gam)
            #print("Pt =", Pt)
            #print("Tt =", Tt)
            #print()
            aList.append(aDeg)
            sList.append(sDeg)
            aListRad.append(a)
            sListRad.append(s)
            Mlist.append(M)
            Plist.append(P)
            Tlist.append(T)
            rhoList.append(rho)
            gamList.append(gam)
            Ptlist.append(Pt)
            Ttlist.append(Tt)
            it_a_s = (it, a, s, P)
            it_a_s_deg = (it, aDeg, sDeg)
            it_a_s_list.append(it_a_s)
            it_a_s_listDeg.append(it_a_s_deg)
            station = (M, P, T, rho, gam, Pt, Tt)
            stations.append(station)
        else:
            no = solve.normal(gam, M)
            M = no[0]
            P = P * no[1]
            T = T * no[2]
            rho = rho * no[3]
            gam = egg.gam(T)
            Pt = Pt * no[4]
            Tt = egg.Tt_T(gam, M) * T
            #print("Normal shock")
            aNormal = 0
            #print("deflection (deg) =", aNormal)
            #print("shock angle (deg) = 90")
            #print("Mout =", M)
            #print("P =", P)
            #print("T =", T)
            #print("rho =", rho)
            #print("gam =", gam)
            #print("Pt =", Pt)
            #print("Tt =", Tt)
            #print()
            right = math.radians(90)
            aList.append(aNormal)
            sList.append(90)
            aListRad.append(aNormal)
            sListRad.append(right)
            Mlist.append(M)
            Plist.append(P)
            Tlist.append(T)
            rhoList.append(rho)
            gamList.append(gam)
            Ptlist.append(Pt)
            Ttlist.append(Tt)
            it_a_s = (it, aNormal, right, P)
            it_a_s_deg = (it, aNormal, int(90))
            it_a_s_list.append(it_a_s)
            it_a_s_listDeg.append(it_a_s_deg)
            station = (M, P, T, rho, gam, Pt, Tt)
            stations.append(station)

    #print("Plist", Plist)
    s_ixy_oxy_list = []
    it = -1
    #ixy and oxy here mean in(x,y) and out(x,y), not inboard/outboard
    for it_a_s in it_a_s_list:
        it += 1
        if it == 0:
            ixy = (0, 0)
            oy = outboard
            ox = oy / math.tan(it_a_s[2])
            oxy = (ox, oy)
            s_ixy_oxy = (it_a_s[2], ixy, oxy)
            s_ixy_oxy_list.append(s_ixy_oxy)
        elif it == 1:
            if M0 < Mdesign:
                ix = outboard / math.tan(solve.oblique_a(aListRad[0], gamList[0], Mdesign)[-1])
            else:
                ix = s_ixy_oxy_list[0][2][0]
            iy = s_ixy_oxy_list[0][2][1]
            ixy = (ix, iy)
            #print("ixy", ixy)
            s = a - it_a_s[2]
            ox = (-math.tan(s) * ixy[0] + ixy[1]) / (math.tan(a) - math.tan(s))
            oy = math.tan(a) * ox
            oxy = (ox, oy)
            s_ixy_oxy = (s, ixy, oxy)
            s_ixy_oxy_list.append(s_ixy_oxy)
        elif it % 2 == 0:
            s = it_a_s[2]
            ixy = s_ixy_oxy_list[-1][2]
            oy = outboard
            ox = ((oy - ixy[1]) / math.tan(s)) + ixy[0]
            oxy = (ox, oy)
            s_ixy_oxy = (s, ixy, oxy)
            s_ixy_oxy_list.append(s_ixy_oxy)
        else:
            ixy = s_ixy_oxy_list[-1][2]
            s = a - it_a_s[2]
            ox = (-math.tan(s) * ixy[0] + ixy[1]) / (math.tan(a) - math.tan(s))
            oy = math.tan(a) * ox
            oxy = (ox, oy)
            s_ixy_oxy = (s, ixy, oxy)
            s_ixy_oxy_list.append(s_ixy_oxy)

    shockXs = []
    shockYs = []
    for x in s_ixy_oxy_list:
        ixy = x[1]
        oxy = x[2]
        shockXs.append(ixy[0])
        shockXs.append(oxy[0])
        shockYs.append(ixy[1])
        shockYs.append(oxy[1])

    #print()

    rampXYs1 = []
    rampPs = []
    for x in s_ixy_oxy_list:
        ixy = x[1]
        oxy = x[2]
        rampXYs1.append(ixy)
        rampXYs1.append(oxy)
        if x[0] > 0:
            rampPs.append(x[-1])

    rampXYs2 = list(set(rampXYs1))
    rampXYs2.sort()
    rampXYs3 = []
    lipXs = []
    for x in rampXYs2:
        if x[1] == outboard:
            lipXs.append(x[0])
        else:
            rampXYs3.append(x)

    lipXXs = [lipXs[0]]
    lipXXYYs = []

    for x in lipXs:
        x2 = x
        x1 = lipXXs[-1]
        YY = (outboard, outboard)
        XX = (x1, x2)
        lipXXs.append(x1)
        lipXXs.append(x2)
        XXYY = (XX, YY)
        if x1 == x2:
            pass
        else:
            lipXXYYs.append(XXYY)

    rampXXs = [0]
    rampYYs = [0]
    rampXXYYs = []
    rampXYs3.pop(0)
    for x in rampXYs3:
        x2 = x[0]
        y2 = x[1]
        x1 = rampXXs[-1]
        y1 = rampYYs[-1]
        rampXXs.append(x1)
        rampYYs.append(y1)
        rampXXs.append(x2)
        rampYYs.append(y2)
        XXYY = ((x1, x2), (y1, y2))
        rampXXYYs.append(XXYY)

    PlistForces = Plist
    PlistForces.pop(0)
    PlistForces.pop(-1)

    it = 0
    lipIt = -1
    rampIt = -1
    dragTotal = 0
    for x in PlistForces:
        it += 1
        if it % 2 == 0:
            lipIt += 1
            lipCoords = lipXXYYs[lipIt]
            #print("lipCoords", lipCoords)
            lipSegLen = abs(lipCoords[0][0] - lipCoords[0][1])
            #print("length", lipSegLen)
            lipSegArea = lipSegLen * H
            #print("area", lipSegArea)
            lipSegForce = lipSegArea * x
            #print("force", lipSegForce)
        else:
            rampIt += 1
            rampCoords = rampXXYYs[rampIt]
            #print("rampCoords", rampCoords)
            side1 = abs(rampCoords[0][0] - rampCoords[0][1])
            side2 = abs(rampCoords[1][0] - rampCoords[1][1])
            rampSegLen = math.sqrt(side1**2 + side2**2)
            #print("length", rampSegLen)
            rampSegArea = rampSegLen * H
            #print(area)
            rampSegForce = rampSegArea * x
            rampSegDrag = rampSegForce * math.sin(a)
            dragTotal += rampSegDrag
            #print("drag", rampSegDrag)

    sioListLen = len(s_ixy_oxy_list)
    #print("LEN SIO", sioListLen)
    maxIt = sioListLen - 2
    it = -1
    triangleAreas = []
    dotXYs = []
    triangles = []

    def triangleArea(x1, x2, x3, y1, y2, y3):
        l1 = math.sqrt((x1 - x2)**2 + (y1 - y2)**2)
        l2 = math.sqrt((x2 - x3)**2 + (y2 - y3)**2)
        l3 = math.sqrt((x3 - x1)**2 + (y3 - y1)**2)
        p = (l1 + l2 + l3)/2
        area = math.sqrt(p * (p - l1) * (p - l2) * (p - l3))
        return area

    def triangleCenter(x1, x2, x3, y1, y2, y3):
        x = (x1 + x2 + x3) / 3
        y = (y1 + y2 + y3) / 3
        return (x, y)
        
    liftAreas = []
    liftAmounts = []
    liftTotal = 0
    liftCenters = []
    while it < maxIt:
        it += 1
        tit = it + 1
        x1 = s_ixy_oxy_list[it][1][0]
        x2 = s_ixy_oxy_list[tit][1][0]
        x3 = s_ixy_oxy_list[tit][2][0]
        y1 = s_ixy_oxy_list[it][1][1]
        y2 = s_ixy_oxy_list[tit][1][1]
        y3 = s_ixy_oxy_list[tit][2][1]
        xy1 = (x1, y1)
        xy2 = (x2, y2)
        xy3 = (x3, y3)
        dotXYs.append(xy1)
        dotXYs.append(xy2)
        dotXYs.append(xy3)
        trianglePoints = (xy1, xy2, xy3)
        rand = choice(colors)
        triangle = Polygon(trianglePoints, facecolor=rand)
        triangles.append(triangle)
        area = triangleArea(x1, x2, x3, y1, y2, y3)
        CoL = triangleCenter(x1, x2, x3, y1, y2, y3)
        liftCenters.append(CoL)
        liftAreas.append(area)

    liftPs = dict(zip(liftAreas, PlistForces))
    for x in liftPs.items():
        amount = x[0] * (x[1] - ISA[0])
        liftAmounts.append(amount)
        liftTotal += amount

    #print("inlet drag", dragTotal)
    #print("inlet lift", liftTotal)
    #print()

    overallCoLd = dict(zip(liftAmounts, liftCenters))
    xCoL = 0
    yCoL = 0
    for x in overallCoLd.items():
        xCoL += ((x[0] * x[1][0]) / liftTotal)
        yCoL += ((x[0] * x[1][1]) / liftTotal)

    ### RETURNS ###
    #aerodynamics = (aListRad, sListRad, aList, sList,
                 #Mlist, Plist, Tlist, rhoList,
                 #gamList, Ptlist, Ttlist)
    #print("Plist", Plist)
    #OPR = Plist[-1] / Plist[0]
    Ptloss = 1 - (Ptlist[-1] / Ptlist[0])
    Ttloss = 1 - (Ttlist[-1] / Ttlist[0])
    #performance = (Ptloss, Ttloss, liftTotal, dragTotal)
    xyCoL = (xCoL, yCoL)
    #geometry = (rampXXYYs, lipXXYYs, dotXYs, triangles, (xCoL, yCoL), shockXs, shockYs)

    if M0 < Mdesign:
        xLip = outboard / math.tan(solve.oblique_a(aListRad[0], gamList[0], Mdesign)[-1])
        def fCap(x):
            return math.tan(aListRad[0]) * (x - xLip) + outboard
        def fShock1(x):
            return math.tan(sListRad[0]) * x
        x1 = 0
        y1 = 0
        x2 = 50
        y2 = fShock1(x2)
        x3 = 0
        y3 = fCap(x3)
        x4 = x2
        y4 = fCap(x4)

        def findIntersection(x1,y1,x2,y2,x3,y3,x4,y4):
            px= ( (x1*y2-y1*x2)*(x3-x4)-(x1-x2)*(x3*y4-y3*x4) ) / ( (x1-x2)*(y3-y4)-(y1-y2)*(x3-x4) ) 
            py= ( (x1*y2-y1*x2)*(y3-y4)-(y1-y2)*(x3*y4-y3*x4) ) / ( (x1-x2)*(y3-y4)-(y1-y2)*(x3-x4) )
            return px, py
        intx, inty = findIntersection(x1,y1,x2,y2,x3,y3,x4,y4)
        Ain = H * (inty / outboard) * outboard
        caper = (inty / outboard) * 100
        #print(caper, "% capture")
    else:
        Ain = H * outboard
        #print("100 % capture")
    Vin = egg.SoS(stations[0][2]) * stations[0][0]
    mdotIn = Ain * Vin * stations[0][3]
    #print("mdot capture", mdotIn)

    aeroDict = {
        "aListRad": aListRad,
        "sListRad": sListRad,
        "aList (deg)": aList,
        "sList (deg)": sList,
        "Mlist": Mlist,
        "Plist (Pa)": Plist,
        "Tlist (K)": Tlist,
        "rhoList (kg/m3)": rhoList,
        "gamList": gamList,
        "Ptlist": Ptlist,
        "Ttlist": Ttlist,
        "altitude": altitude
    }
    perfDict = {
        "Ptloss": Ptloss,
        "Ttloss": Ttloss,
        "liftTotal": liftTotal,
        "dragTotal": dragTotal,
        "mdotIn": mdotIn,
    }
    geomDict = {
        "rampXXYYs": rampXXYYs,
        "lipXXYYs": lipXXYYs,
        "dotXYs": dotXYs,
        "triangles": triangles,
        "xyCoL": xyCoL,
        "shockXs": shockXs,
        "shockYs": shockYs
    }

    returnDict = {
        "aerodynamics": aeroDict,
        "performance": perfDict,
        "geometry": geomDict,
        "stations": stations
    }
    return returnDict
        
        

    
