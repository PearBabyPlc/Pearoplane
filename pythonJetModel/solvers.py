import math
import numpy as np
import formulaegg as egg
import buzzerrookie_isa as isa

smb = 6356750
def getISA_alt(alt):
    altitude = (smb * alt) / (alt + smb)
    return isa.isa(altitude)

def getISA_MQ(M, Q):
    altRange = range(0, 47001)
    alts = []
    Ps = []
    for x in altRange:
        alts.append(x)
        ISA = getISA_alt(x)
        Ps.append(ISA[0])
    altPs = dict(zip(alts, Ps))
    P = (Q / M**2) / 0.7
    alt, _ = min(altPs.items(), key=lambda x: abs(P - x[1]))
    #print("Mach number =", M)
    #print("Dynamic pressure (Pa) =", Q)
    #print("Altitude (m ASL) =", alt)
    #print()
    return isa.isa(alt), alt

#tuple order: M, static P T rho, stagnation, angles

def oblique_s(s, gam, M):
    a = egg.aOblique(s, gam, M)
    Mout = egg.MoutOblique(s, a, gam, M)
    Pr = egg.PrOblique(s, gam, M)
    Tr = egg.TrOblique(s, gam, M)
    rhor = egg.rhorOblique(s, gam, M)
    Ptr = egg.PtrOblique(s, gam, M)
    return (Mout, Pr, Tr, rhor, Ptr, a)

def getTheta(mach1, gamma, delta):
    A = mach1**2 - 1
    B = 0.5*(gamma + 1) * mach1**4 * np.tan(delta)
    C = (1 + 0.5*(gamma + 1)*mach1**2)*np.tan(delta)
    coeffs = [1, C, -A, (B - A*C)]
    roots = np.array([r for r in np.roots(coeffs) if r > 0])
    thetas = np.arctan(1 / roots)
    theta_weak = np.min(thetas)
    theta_strong = np.max(thetas)
    return theta_weak, theta_strong

def oblique_a(a, gam, M):
    s, _ = getTheta(M, gam, a)
    #aCheck = egg.aOblique(s, gam, M)
    #print("a aCheck", a, aCheck)
    Mout = egg.MoutOblique(s, a, gam, M)
    Pr = egg.PrOblique(s, gam, M)
    Tr = egg.TrOblique(s, gam, M)
    rhor = egg.rhorOblique(s, gam, M)
    Ptr = egg.PtrOblique(s, gam, M)
    aDeg = math.degrees(a)
    sDeg = math.degrees(s)
    #print("aDeg =", aDeg)
    #print("sDeg =", sDeg)
    #RATIOS
    return (Mout, Pr, Tr, rhor, Ptr, a, s)

def normal(gam, M):
    Mout = egg.MoutNormal(gam, M)
    Pr = egg.PrNormal(gam, M)
    Tr = egg.TrNormal(gam, M)
    rhor = egg.rhorNormal(gam, M)
    Ptr = egg.PtrNormal(gam, M)
    return (Mout, Pr, Tr, rhor, Ptr)

def shock(s, gam, M):
    a = egg.aOblique(s, gam, M)
    aLim = egg.aLimit(gam, M)
    if a < aLim:
        return oblique_s(s, gam, M)
    else:
        return normal(gam, M)

def obliqueShock(stationIn, aDeg):
    a = math.radians(aDeg)
    aLim = egg.aLimit(stationIn[4], stationIn[0])
    if a < aLim:
        ratios = oblique_a(a, stationIn[4], stationIn[0])
        s = math.degrees(ratios[-1])
    else:
        ratios = normal(stationIn[4], stationIn[0])
        s = 90
    Mout = ratios[0]
    Pout = ratios[1] * stationIn[1]
    Tout = ratios[2] * stationIn[2]
    rhoOut = ratios[3] * stationIn[3]
    gamOut = egg.gam(Tout)
    Ptout = ratios[4] * stationIn[5]
    Ttout = egg.Tt_T(gamOut, Mout) * Tout
    stationOut = (Mout, Pout, Tout, rhoOut, gamOut, Ptout, Ttout)
    return stationOut, s
        
### JET ENGINE COMPONENTS ###

def Turbine(stationIn, mdot, powerReq, eff):
    TW = powerReq / mdot
    #print("COMPRESSOR | TW =", TW, "| eff =", eff)
    V4 = egg.SoS(stationIn[2]) * stationIn[0]
    A4 = mdot / (V4 * stationIn[3])
    Cp4 = egg.Cp(stationIn[2])
    gam4 = stationIn[4]

    TPRaa = -TW / (eff * Cp4 * stationIn[6])
    TPRa = (TPRaa + 1)**(gam4 / (gam4 - 1))
    Pt4a = TPRa * stationIn[5]
    Tt4a = stationIn[6] * TPRa**((gam4 - 1) / gam4)
    T4a = egg.T_Tt(gam4, stationIn[0]) * Tt4a
    gam4a = egg.gam(T4a)
    Cp4a = egg.Cp(T4a)
    gam4b = (gam4 + gam4a) / 2
    Cp4b = (Cp4 + Cp4a) / 2
    M4a = V4 / egg.SoS(T4a)

    TPRx = -TW / (eff * Cp4b * stationIn[6])
    TPR = (TPRx + 1)**(gam4b / (gam4b - 1))
    #print("TPR =", TPR)
    #invTPR = 1 / TPR
    #print("Ptin / Ptout =", invTPR)

    Pt5 = TPR * stationIn[5]
    Tt5 = stationIn[6] * TPR**((gam4b - 1) / gam4b)
    T5 = egg.T_Tt(gam4b, M4a) * Tt5
    P5 = egg.P_Pt(gam4b, M4a) * Pt5
    rho5 = egg.getRho(P5, T5)
    gam5 = egg.gam(T5)
    M5 = V4 / egg.SoS(T5)
    A5 = mdot / (V4 * rho5)
    #print("Area out (m2) =", A5)

    stationOut = (M5, P5, T5, rho5, gam5, Pt5, Tt5)
    return stationOut, TPR, A5

def Compressor(stationIn, mdot, CPR, eff):
    #print("COMPRESSOR | CPR =", CPR, "| eff =", eff)
    #print()
    V2 = egg.SoS(stationIn[2]) * stationIn[0]
    #print("V2 = V3 =", V2)
    A2 = mdot / (V2 * stationIn[3])
    Cp2 = egg.Cp(stationIn[2])
    gam2 = stationIn[4]
    #print()

    Tt2a = stationIn[6] * CPR**((gam2 - 1) / gam2)
    T2a = egg.T_Tt(gam2, stationIn[0]) * Tt2a
    gam2a = egg.gam(T2a)
    Cp2a = egg.Cp(T2a)
    M2a = V2 / egg.SoS(T2a)
    #print("Tt2a", Tt2a)
    #print("T2a", T2a)
    #print("gam2a", gam2a)
    #print("Cp2a", Cp2a)
    #print("M2a", M2a)
    #print()

    CWa = (Cp2a * stationIn[6]) / eff
    CWb = CPR**((gam2a - 1) / gam2a)
    CW = CWa * (CWb - 1)
    Cpower = CW * mdot
    #print("CW =", CW)
    #print("Compressor power (watts) =", Cpower)
    #print()

    Pt3 = CPR * stationIn[5]
    Tt3 = stationIn[6] * CPR**((gam2a - 1) / gam2a)
    T3 = egg.T_Tt(gam2a, M2a) * Tt3
    P3 = egg.P_Pt(gam2a, M2a) * Pt3
    rho3 = egg.getRho(P3, T3)
    gam3 = egg.gam(T3)
    M3 = V2 / egg.SoS(T3)
    A3 = mdot / (V2 * rho3)
    #print("Area out (m2) =", A3)
    #print()

    stationOut = (M3, P3, T3, rho3, gam3, Pt3, Tt3)
    return stationOut, Cpower, A3

Mstep = 0.001
def Combustor(stationInn, mdotIn, Mlimit, Tlimit, LHVfuel, maxA):
    M = stationInn[0]
    P = stationInn[1]
    T = stationInn[2]
    rho = stationInn[3]
    gam = stationInn[4]
    Pt = stationInn[5]
    Tt = stationInn[6]
    Cp = egg.Cp(T)
    mdot = mdotIn
    V = egg.SoS(T) * M
    A = mdot / (V * rho)

    stations = [stationInn]
    Cplist = [Cp]
    cumFuel = 0
    Vlist = [V]
    Tlist = [T]
    rhoList = [rho]
    Alist = [A]
    #print("maxA", maxA)
    while (M <= Mlimit) and (T <= Tlimit) and (A <= maxA):
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

        M += Mstep
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

        A = mdot / (V * rho)
        if A >= maxA:
            break
        else:
            heating_kgAir = deltaTt * Cp
            kgFuel = (heating_kgAir * mdot) / LHVfuel
            cumFuel += kgFuel
            mdot += kgFuel
            stations.append(stationOut)
            Vout = egg.SoS(T) * M
            Vlist.append(Vout)
            Tlist.append(T)
            rhoList.append(rho)
            Alist.append(A)

    stationOut = stations[-1]
    cumV = 0
    for x in Vlist:
        cumV += x
    averageV = cumV / len(Vlist)

    #print("COMBUSTOR | Tmax =", Tlimit, "| Mmax =", Mlimit)
    mdotOut = mdot
    #print("mdotIn out", mdotIn, mdotOut)
    #print("mdot fuel (kg/s)", cumFuel)
    #ccheck = mdotOut - (mdotIn + cumFuel)
    #print("check", ccheck)
    #print("Vin avg out", Vlist[0], averageV, Vlist[-1])
    Aout = Alist[-1]
    #print("Ain out", Alist[0], Aout)
    
    return stationOut, mdotOut, cumFuel, Aout

Rs = 287
Runiv = 8.3145
mol = Runiv / Rs

def mixFlows(stationB, stationC, mdotB, mdotC):
    VB = stationB[0] * egg.SoS(stationB[2])
    VC = stationC[0] * egg.SoS(stationC[2])
    AB = mdotB / (VB * stationB[3])
    AC = mdotC / (VC * stationC[3])
    mdot3 = mdotB + mdotC
    A3 = AB + AC
    QB = mdotB / stationB[3]
    QC = mdotC / stationC[3]
    KB = (mdotB * VB**2) / 2
    KC = (mdotC * VC**2) / 2
    K3 = KB + KC
    V3 = math.sqrt((2 * K3) / mdot3)
    Q3 = A3 * V3
    rho3 = mdot3 / Q3
    UrmsB = math.sqrt((3 * Runiv * stationB[2]) / mol)
    UrmsC = math.sqrt((3 * Runiv * stationC[2]) / mol)
    nB = mdotB / mol
    nC = mdotC / mol
    n3 = nB + nC
    Urms3 = math.sqrt(((nB * UrmsB**2) + (nC * UrmsC**2)) / n3)
    T3 = (Urms3**2 * mol) / (3 * Runiv)
    P3 = Rs * T3 * rho3
    M3 = V3 / egg.SoS(T3)
    gam3 = egg.gam(T3)
    Pt3 = egg.Pt_P(gam3, M3) * P3
    Tt3 = egg.Tt_T(gam3, M3) * T3
    stationOut = (M3, P3, T3, rho3, gam3, Pt3, Tt3)
    return stationOut, mdot3
    
def simpleEjector(stationIn, station0, mdot, AoutMax, backPressureMargin):
    station = stationIn
    M = station[0]
    P = station[1]
    P0 = station0[1] * backPressureMargin
    T = station[2]
    rho = station[3]
    gam = station[4]
    Pt = station[5]
    Tt = station[6]
    V = egg.SoS(T) * M
    Ain = mdot / (V * rho)
    A = Ain
    stations = [station]
    Alist = [A]
    Vlist = [V]
    #print("Ain", A)
    #print("Vin", V)
    while M <= 1.0:
        M += Mstep
        #print(M)
        P = egg.P_Pt(gam, M) * Pt
        T = egg.T_Tt(gam, M) * Tt
        rho = egg.getRho(P, T)
        gam = egg.gam(T)
        Pt = egg.Pt_P(gam, M) * P
        Tt = egg.Tt_T(gam, M) * T
        V = egg.SoS(T) * M
        A = mdot / (V * rho)
        stationOut = (M, P, T, rho, gam, Pt, Tt)
        stations.append(stationOut)
        Alist.append(A)
        Vlist.append(V)
    stationThroat = stations[-1]
    Athroat = Alist[-1]
    Vthroat = Vlist[-1]
    #print("Athroat", Athroat)
    #print("Vthroat", Vthroat)
    while A <= AoutMax:
        if P <= P0:
            break
        else:
            M += Mstep
            #print(M)
            P = egg.P_Pt(gam, M) * Pt
            T = egg.T_Tt(gam, M) * Tt
            rho = egg.getRho(P, T)
            gam = egg.gam(T)
            Pt = egg.Pt_P(gam, M) * P
            Tt = egg.Tt_T(gam, M) * T
            V = egg.SoS(T) * M
            A = mdot / (V * rho)
            stationOut = (M, P, T, rho, gam, Pt, Tt)
            stations.append(stationOut)
            Alist.append(A)
            Vlist.append(V)
    stationOut = stations[-1]
    Aout = Alist[-1]
    Vout = Vlist[-1]
    #print("Aout", Aout)
    #print("Vout", Vout)
    #print("P0 out", P0, stationOut[1])
    mdotOut = Aout * Vout * stationOut[3]
    #print("mdotIn out", mdot, mdotOut)
    return stationThroat, stationOut, Athroat, Vthroat, Aout, Vout

def generalThrust(mdotIn, mdotOut, Vin, Vout, Pin, Pout, Aout):
    thrust = mdotOut * Vout
    ramDrag = mdotIn * Vin
    newtonian = thrust - ramDrag
    pressure = (Pout - Pin) * Aout
    return newtonian + pressure

def expansionFan(stationIn, aDeg):
    a = math.radians(aDeg)
    Min = stationIn[0]
    gam = stationIn[4]
    PMin = egg.PMangle(gam, Min)
    PMout = PMin + a
    PMtest = PMin
    Mstep = 0.001
    Mtest = Min
    Mout = 1.0
    while PMtest <= PMout:
        Mtest += Mstep
        PMtest = egg.PMangle(gam, Mtest)
        if PMtest >= PMout:
            #print()
            #print("SUCCESS!")
            #print("PM test out", PMtest, PMout)
            diff = abs(PMtest - PMout)
            #print("difference =", diff)
            Mout = Mtest
            #print("Min out", Min, Mout)
            break
        else:
            pass
            #print("PM test out", PMtest, PMout)
            #print("Min test", Min, Mtest)
    Pout = stationIn[5] * egg.P_Pt(gam, Mout)
    Tout = stationIn[6] * egg.T_Tt(gam, Mout)
    rhoOut = egg.getRho(Pout, Tout)
    gamOut = egg.gam(Tout)
    Ptout = egg.Pt_P(gamOut, Mout) * Pout
    Ttout = egg.Tt_T(gamOut, Mout) * Tout
    stationOut = (Mout, Pout, Tout, rhoOut, gamOut, Ptout, Ttout)
    return stationOut

def AoArequired(M0, Q, Awing, LiftTons):
    ISA, _ = getISA_MQ(M0, Q)
    P0 = ISA[0]
    T0 = ISA[1]
    rho0 = ISA[2]
    gam0 = egg.gam(T0)
    Pt0 = egg.Pt_P(gam0, M0) * P0
    Tt0 = egg.Tt_T(gam0, M0) * T0
    station0 = (M0, P0, T0, rho0, gam0, Pt0, Tt0)
    LiftKg = LiftTons * 1000
    WingLoading = LiftKg / Awing
    print("Wing loading goal =", WingLoading)

    AoArange = np.linspace(0.1, 20.0, num=200)
    a_WL_LDlist = []
    for a in AoArange:
        aRad = math.radians(a)
        stationUpper = expansionFan(station0, a)
        stationLower, _ = obliqueShock(station0, a)
        normalForce = stationLower[1] - stationUpper[1]
        LiftN = math.cos(aRad) * normalForce
        DragN = math.sin(aRad) * normalForce
        LD = LiftN / DragN
        WL = LiftN / 9.80665
        if WL >= WingLoading:
            a_WL_LD = (a, WL, LD)
            #print("Success with AoA =", a, "deg")
            a_WL_LDlist.append(a_WL_LD)
        else:
            pass
            #print("Failure with AoA =", a, "deg")

    a_WL_LDlist.sort(key=lambda x: abs(x[1] - WingLoading))
    AoAout = a_WL_LDlist[0]
    #print()
    #for x in a_WL_LDlist:
    #    print("AoA", x[0], "| WL", x[1], "| LD", x[2])
    return (AoAout[0], AoAout[1], AoAout[2])
        
            
        
            
