import math
import formulaegg as egg

workDone = 0.95
isentropicStageEff = 0.9

def guideVanes(stationIn, swirlOut):
    swirlRad = math.radians(swirlOut)
    Vin = stationIn[0] * egg.SoS(stationIn[2])
    Vout = Vin / math.cos(swirlRad)
    Mout = Vout / egg.SoS(stationIn[2])
    Ptout = stationIn[5]
    Ttout = stationIn[6]
    Pout = egg.P_Pt(stationIn[4], Mout) * Ptout
    Tout = egg.T_Tt(stationIn[4], Mout) * Ttout
    rhoOut = egg.getRho(Pout, Tout)
    gamOut = egg.gam(Tout)
    stationOut = (Mout, Pout, Tout, rhoOut, gamOut, Ptout, Ttout)
    print("guide vane angle out =", swirlOut)
    print("abs Vin out", Vin, Vout)
    print("Min out", stationIn[0], Mout)
    print("Pin out", stationIn[1], Pout)
    print("Tin out", stationIn[2], Tout)
    print("rhoIn out", stationIn[3], rhoOut)
    print("gamIn out", stationIn[4], gamOut)
    print()
    return stationOut

def compressorStage_design(stageLoading, flowCoeff, degReaction, swirlIn, Rt1, Rh1, stationIn, con_exp):
    Min = stationIn[0]
    Pin = stationIn[1]
    Tin = stationIn[2]
    rhoIn = stationIn[3]
    gamIn = stationIn[4]
    Ptin = stationIn[5]
    Ttin = stationIn[6]
    Vin = Min * egg.SoS(Tin)
    At1 = math.pi * Rt1**2
    Ah1 = math.pi * Rh1**2
    Ain = At1 - Ah1
    R1 = math.sqrt(Ain / math.pi)
    a1 = math.atan(((2 * (1 - degReaction)) - stageLoading) / (2 * flowCoeff))
    Ca = Vin * math.cos(a1)
    mdot = Ain * Ca * rhoIn
    print("mdot", mdot)

    #velocity triangles
    U = Ca / flowCoeff
    a1deg = math.degrees(a1)
    print("swirl in must equal a1. swirlIn =", swirlIn, "| a1 =", a1deg)
    a2 = math.atan((stageLoading / flowCoeff) * math.tan(a1))
    a3 = a1
    b1 = math.atan((1 / flowCoeff) - math.tan(a1))
    b2 = math.atan((1 / flowCoeff) - math.tan(a2))
    Cw1 = math.tan(a1) * Ca
    Cw2 = math.tan(a2) * Ca
    Wc = U * (Cw2 - Cw1)
    rpm = (30 * U) / (math.pi * R1)
    print("rpm", rpm)
    C1 = Ca / math.cos(a1)
    C3 = Ca / math.cos(a3)
    print("C1 should equal C3. C1 =", C1, "| C3 =", C3)

    #stationOut
    Cp = egg.Cp(Tin)
    deltaT0s = ((workDone * U * Ca) / Cp) * (math.tan(b1) - math.tan(b2))
    stagePR = (1 + ((isentropicStageEff * deltaT0s) / Ttin))**(gamIn / (gamIn - 1))
    print("stage pressure ratio =", stagePR)
    Tout = Tin + deltaT0s
    Ttout = Ttin + deltaT0s
    Pout = Pin * stagePR
    Vout = C3
    Mout = Vout / egg.SoS(Tout)
    gamOut = egg.gam(Tout)
    Ptout = egg.Pt_P(gamOut, Mout) * Pout
    rhoOut = egg.getRho(Pout, Tout)
    stationOut = (Mout, Pout, Tout, rhoOut, gamOut, Ptout, Ttout)
    A2 = mdot / (Ca * rhoOut)

    #angles
    swirlOut = math.degrees(a3)
    a2deg = math.degrees(a2)
    a3deg = swirlOut
    b1deg = math.degrees(b1)
    b2deg = math.degrees(b2)
    anglesDeg = (a1deg, a2deg, a3deg, b1deg, b2deg)

    #measurements
    Aout = mdot / (Ca * rhoOut)
    if con_exp == "con":
        Rh2 = Rh1
        At2 = A2 + Ah1
        Rt2 = math.sqrt(At2 / math.pi)
    elif con_exp == "exp":
        Rt2 = Rt1
        Ah2 = At1 - A2
        Rh2 = math.sqrt(Ah2 / math.pi)
    else:
        R2 = math.sqrt(A2 / math.pi)
        Rt2 = (R2 * Rt1) / R1
        Rh2 = (R2 * Rh1) / R1
    print("swirl out =", swirlOut)
    print("Ain out", Ain, Aout)
    print("Rtip hub | in", Rt1, Rh1, "| out", Rt2, Rh2)
    print()
    return Rt2, Rh2, stationOut, anglesDeg, rpm

def compressorStage_actual(rpm, anglesIn, Rt1, Rh1, stationIn, con_exp):
    a1 = math.radians(anglesIn[0])
    a2 = math.radians(anglesIn[1])
    a3 = math.radians(anglesIn[2])
    b1 = math.radians(anglesIn[3])
    b2 = math.radians(anglesIn[4])
    
    
    Min = stationIn[0]
    Pin = stationIn[1]
    Tin = stationIn[2]
    rhoIn = stationIn[3]
    gamIn = stationIn[4]
    Ptin = stationIn[5]
    Ttin = stationIn[6]
    Vin = Min * egg.SoS(Tin)
    At1 = math.pi * Rt1**2
    Ah1 = math.pi * Rh1**2
    Ain = At1 - Ah1
    R1 = math.sqrt(Ain / math.pi)
    Ca = Vin * math.cos(a1)
    mdot = Ain * Ca * rhoIn
    print("mdot", mdot)

    #velocity triangles
    U = (rpm * math.pi * R1) / 30
    flowCoeff = Ca / U
    a1deg = math.degrees(a1)
    Cw1 = math.tan(a1) * Ca
    Cw2 = math.tan(a2) * Ca
    stageLoading1 = (workDone * (Cw2 - Cw1)) / U
    stageLoading2 = ((workDone * Ca) / U) * (math.tan(a2) - math.tan(a1))
    stageLoading3 = workDone * flowCoeff * (math.tan(a2) - math.tan(a1))
    print("flow coeff =", flowCoeff)
    print("stage loadings", stageLoading1, stageLoading2, stageLoading3)
    Wc = U * (Cw2 - Cw1)
    C1 = Ca / math.cos(a1)
    C3 = Ca / math.cos(a3)

    #stationOut
    Cp = egg.Cp(Tin)
    deltaT0s = ((workDone * U * Ca) / Cp) * (math.tan(b1) - math.tan(b2))
    stagePR = (1 + ((isentropicStageEff * deltaT0s) / Ttin))**(gamIn / (gamIn - 1))
    print("stage pressure ratio =", stagePR)
    Tout = Tin + deltaT0s
    Ttout = Ttin + deltaT0s
    Pout = Pin * stagePR
    Vout = C3
    Mout = Vout / egg.SoS(Tout)
    gamOut = egg.gam(Tout)
    Ptout = egg.Pt_P(gamOut, Mout) * Pout
    rhoOut = egg.getRho(Pout, Tout)
    stationOut = (Mout, Pout, Tout, rhoOut, gamOut, Ptout, Ttout)
    A2 = mdot / (Ca * rhoOut)

    #angles
    swirlOut = math.degrees(a3)
    a2deg = math.degrees(a2)
    a3deg = swirlOut
    b1deg = math.degrees(b1)
    b2deg = math.degrees(b2)
    anglesDeg = (a1deg, a2deg, a3deg, b1deg, b2deg)

    #measurements
    Aout = mdot / (Ca * rhoOut)
    if con_exp == "con":
        Rh2 = Rh1
        At2 = A2 + Ah1
        Rt2 = math.sqrt(At2 / math.pi)
    elif con_exp == "exp":
        Rt2 = Rt1
        Ah2 = At1 - A2
        Rh2 = math.sqrt(Ah2 / math.pi)
    else:
        R2 = math.sqrt(A2 / math.pi)
        Rt2 = (R2 * Rt1) / R1
        Rh2 = (R2 * Rh1) / R1
    print("Ain out", Ain, Aout)
    print("Rtip hub | in", Rt1, Rh1, "| out", Rt2, Rh2)
    print()
    return Rt2, Rh2, stationOut, anglesDeg
