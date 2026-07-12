import math
import numpy as np
from scipy import integrate
from scipy.interpolate import interp1d
from scipy.optimize import fsolve
from ceaparse import Position
import NISTlookup as NIST
import matplotlib.pyplot as plt

#Position(position, P, T, Cp, gam, SoS, M, mu, Pr, AR, Cstar, Isp)
def idealST(T, gam, M):
    return T * (1 + ((gam - 1) / 2)*M**2)

def idealSP(P, gam, M):
    return P * (1 + ((gam - 1) / 2)*M**2)**(gam / (gam - 1))

def findTaw(T, Pr, gam, M):
    Tt = idealST(T, gam, M)
    TawA = 1 + Pr**0.33 * ((gam - 1) / 2) * M**2
    TawB = 1 + ((gam - 1) / 2) * M**2
    return Tt * (TawA / TawB)

omega = 0.6
Rcurv = 0.494
def hgBartz(Rt, muIn, Cpin, Pr, Pcin, Cstar, AR, Tw, Tc, gam, M):
    mu = muIn / 10000 #millipoise to Pa s
    Cp = Cpin * 1000 #kJ/kg-K to J/kg-K
    Pc = Pcin * 100000 #bar to Pa
    Tt = idealST(Tc, gam, M)
    sigmaA = 1 + ((gam - 1) / 2)*M**2
    sigmaB = 0.5*(Tw / Tt)*sigmaA + 0.5
    omegaA = omega / 5
    omegaB = 0.8 - omegaA
    sigma = 1 / (sigmaB**omegaB * sigmaA**omegaA)
    hgA = 0.026 / (2 * Rt)**0.2
    hgB = (mu**0.2 * Cp) / Pr**0.6
    Pt = idealSP(Pc, gam, M)
    hgC = Pt / Cstar
    hgD = (2 * Rt) / Rcurv
    hgE = 1 / AR
    return hgA * hgB * hgC**0.8 * hgD**0.1 * hgE**0.9 * sigma

#colemanspace = np.round(np.linspace(0.00001, 0.1, num=10000), 8)

def regenTestH2(positions, plotGeom, heatGeom, lookupH2, Rt, Tw, Pcinn, Tcin, mdot):
    Pcin = Pcinn / 100000
    Pcin = round(Pcin)
    XRs = dict(zip(plotGeom[0], plotGeom[1]))
    posXlist = []
    posQlist = []
    positions.sort(key=lambda x: x.P, reverse=True)
    offsetA = list(XRs.items())
    offset = -offsetA[0][0]
    for p in positions:
        R = Rt * math.sqrt(p.AR)
        hg = hgBartz(Rt, p.mu, p.Cp, p.Pr, p.P, p.Cstar, p.AR, Tw, p.T, p.gamma, p.M)
        Taw = findTaw(p.T, p.Pr, p.gamma, p.M)
        Q = hg * (Taw - Tw)
        Xminus, _ = min(XRs.items(), key=lambda x: abs(R - x[1]))
        X = Xminus + offset
        posXlist.append(X)
        Q = 2 * math.pi * R * Q
        posQlist.append(Q)
    interpXQ = interp1d(posXlist, posQlist)
    lenXlist = []
    lenLlist = []
    for x in heatGeom:
        lenXlist.append(x[0])
        lenLlist.append(x[2])
    interpLX = interp1d(lenLlist, lenXlist)
    neLen = lenLlist[-1]
    neStep = neLen / 200
    neSum = -neStep
    neRange = range(0, 201)
    revLlist = []
    for x in neRange:
        neSum += neStep
        revLlist.append(neSum)
    revXlist = []
    for x in revLlist:
        try:
            revX = interpLX(x)
            revXlist.append(revX)
        except:
            try:
                xa = x + 0.0001
                revX = interpLX(xa)
                revXlist.append(revX)
            except:
                xb = x - 0.0001
                revX = interpLX(xb)
                revXlist.append(revX)
    revQlist = []
    for x in revXlist:
        try:
            revQ = interpXQ(x)
            revQlist.append(revQ)
        except:
            try:
                xa = x + 0.0001
                revQ = interpXQ(xa)
                revQlist.append(revQ)
            except:
                xb = x - 0.0001
                revQ = interpXQ(xb)
                revQlist.append(revQ)

    interpXR = interp1d(plotGeom[0], plotGeom[1])
    revRlist = []
    for x in revXlist:
        Xminus = x - offset
        _, R = min(XRs.items(), key=lambda x: abs(Xminus - x[0]))
        revRlist.append(R)

    it = -1
    lenRev = len(revXlist)
    lenRevRange = range(0, lenRev)
    XRLQs = []
    for x in lenRevRange:
        it += 1
        lenRevTup = (revXlist[it], revRlist[it], revLlist[it], revQlist[it])
        XRLQs.append(lenRevTup)

    Pcini = Pcin + 1
    coolantInTemps = NIST.initLookupNIST("H2", range(Pcin, Pcini))
    coolantIn = min(coolantInTemps, key=lambda x: abs(Tcin - x.T))
    it = -1
    lenXRLQs = len(XRLQs)
    lenXRLQsRange = range(0, lenXRLQs)
    segQgas = []
    segQcool = []
    coolantOut = coolantIn
    Lsum = 0
    Qsum = 0

    def B(epsilon):
        if epsilon >= 7:
            B = 4.7 * (epsilon)**0.2
        elif epsilon < 7:
            B = 4.5 + 0.57*(epsilon)**0.75
        return B

    #leave these
    numChannels = 600
    velChannels = 15
    e = 2.5 * 10**-7
    #roughSmoo = 0.001
    #channelHeight = Rt * 0.04

    pltXs = []
    pltRs = []
    pltQhgs = []
    pltQcgs = []
    pltAs = []
    pltWs = []
    pltHs = []
    

    maxIt = len(XRLQs) -2
    Pout = Pcin
    Tout = Tcin
    for entry in XRLQs:
        it += 1
        if it <= maxIt:
            entryA = XRLQs[it]
            ot = it + 1
            if type(XRLQs[ot]) != tuple:
                break
            else:
                pass
            entryB = XRLQs[ot]
            Qavg = (entryA[3] + entryB[3]) / 2
            Qgas = Qavg * neStep
            Lsum += neStep
            segQgas.append(Qgas)
            Qsum += Qgas
            Cavg = ((entryA[1] + entryB[1]) / 2) * 2 * math.pi
            ravage =  (entryA[1] + entryB[1]) / 2
            channelWidth = Cavg / (numChannels * 1.25)
            rhoIn = coolantIn.rho
            mdotChannel = mdot / numChannels
            volChannel = mdotChannel / rhoIn
            channelHeight = volChannel / (channelWidth * velChannels)
            Dh = (4 * channelWidth * channelHeight) / (2 * (channelWidth + channelHeight))
            Pr = (coolantIn.Cp * 1000 * coolantIn.mu) / coolantIn.thermalCond
            Re = (coolantIn.rho * velChannels * Dh) / coolantIn.mu

            def fF(f):
                LHS = 1 / math.sqrt(f)
                RHS = -2 * math.log10(((e / Dh) / 3.7) + (2.51 / (Re * math.sqrt(f))))
                return abs(LHS - RHS)

            def fFsmooth(f):
                LHS = 1 / math.sqrt(f)
                RHS = -2 * math.log10(2.51 / (Re * math.sqrt(f)))
                return abs(LHS - RHS)

            f = fsolve(fF, 0.01)[0] #this is really fucking shit up
            fSmooth = fsolve(fFsmooth, 0.01)[0]
            roughSmoo = f / fSmooth
            #print("f =", f)
            #f = 0.013
            s = neStep
            C1A = 1 + 1.5*Pr**(-1/6) * Re**-0.125 * (Pr - 1) * roughSmoo
            C1B = 1 + 1.5*Pr**(-1/6) * Re**-0.125 * (Pr*roughSmoo - 1)
            C1 = C1A / C1B
            C2 = 1 + (s/Dh)**-0.7 * (Tw / coolantIn.T)**0.1
            R = Rt * math.sqrt(1.5)
            C3 = Re * (0.25*Dh / R)**2
            epsilon = Re * (e / Dh) * math.sqrt(f / 8)
            NuA = (f/8) * Re * Pr * (coolantIn.T / Tw)**0.55
            NuB = 1 + (f/8)**0.55 * (B(epsilon) - 8.48)
            Nu = (NuA / NuB) * C1 * C2 * C3
            hh2 = (Nu * Dh) / coolantIn.thermalCond
            Qcool = hh2 * numChannels * 2 * s * (channelWidth + channelHeight) * 0.5
            segQcool.append(Qcool)
            deltaT = (Qcool / coolantIn.Cp) / mdot
            deltaP1 = ((f * coolantIn.rho * velChannels**2 * s) / (2 * Dh)) / 100000
            Pout = Pcin - deltaP1
            Tout = Tcin + deltaT
            Pcmid = round(float(Pout))
            Pcmidi = Pcmid + 1
            coolantMidTemps = NIST.initLookupNIST("H2", range(Pcmid, Pcmidi))
            coolantOut = min(coolantMidTemps, key=lambda x: abs(Tout - x.T))
            print("X P T Cp Re Nu", entryA[0], Pout, Tout, coolantIn.Cp, Re, Nu)
            coolantIn = coolantOut
            Pcin = Pout
            Tcin = Tout
            pltxavg = (entryA[0] + entryB[0]) / 2
            pltXs.append(pltxavg)
            pltravg = (entryA[1] + entryB[1]) / 2
            pltRs.append(ravage)
            pltQhgs.append(Qavg)
            qcgsavg = Qcool / s
            pltQcgs.append(qcgsavg)
            achan = channelWidth * channelHeight
            pltAs.append(achan)
            pltWs.append(channelWidth)
            pltHs.append(channelHeight)
        else:
            break
    print()
    Qgsum = 0
    for x in segQgas:
        Qgsum += x
    print("Qsum hot gas =", Qgsum)
    Qcsum = 0
    for x in segQcool:
        Qcsum += x
    print("Qsum coolant =", Qcsum)
    fig, (ax1, ax2, ax3) = plt.subplots(3, 1, sharex='col')
    ax1.plot(pltXs, pltRs, color='white')
    ax1.set_aspect('equal')
    ax2.plot(pltXs, pltQhgs, color='red')
    ax2.plot(pltXs, pltQcgs, color='aqua')
    ax3.plot(pltXs, pltAs, color='white')
    ax3.plot(pltXs, pltWs, color='red')
    ax3.plot(pltXs, pltHs, color='aqua')
    ax1.grid()
    ax2.grid()
    ax3.grid()
    plt.savefig("HMX3KregenH2test.pdf")
    plt.show()

def regenTestCH4(positions, plotGeom, heatGeom, lookupCH4, Rt, Tw, Pcinn, Tcin, mdot):
    Pcin = Pcinn / 100000
    Pcin = round(Pcin)
    XRs = dict(zip(plotGeom[0], plotGeom[1]))
    posXlist = []
    posQlist = []
    positions.sort(key=lambda x: x.P, reverse=True)
    offsetA = list(XRs.items())
    offset = -offsetA[0][0]
    for p in positions:
        R = Rt * math.sqrt(p.AR)
        hg = hgBartz(Rt, p.mu, p.Cp, p.Pr, p.P, p.Cstar, p.AR, Tw, p.T, p.gamma, p.M)
        Taw = findTaw(p.T, p.Pr, p.gamma, p.M)
        Q = hg * (Taw - Tw)
        Xminus, _ = min(XRs.items(), key=lambda x: abs(R - x[1]))
        X = Xminus + offset
        posXlist.append(X)
        Q = 2 * math.pi * R * Q
        posQlist.append(Q)
    interpXQ = interp1d(posXlist, posQlist)
    lenXlist = []
    lenLlist = []
    for x in heatGeom:
        lenXlist.append(x[0])
        lenLlist.append(x[2])
    interpLX = interp1d(lenLlist, lenXlist)
    neLen = lenLlist[-1]
    neStep = neLen / 200
    neSum = -neStep
    neRange = range(0, 201)
    revLlist = []
    for x in neRange:
        neSum += neStep
        revLlist.append(neSum)
    revXlist = []
    for x in revLlist:
        try:
            revX = interpLX(x)
            revXlist.append(revX)
        except:
            try:
                xa = x + 0.0001
                revX = interpLX(xa)
                revXlist.append(revX)
            except:
                xb = x - 0.0001
                revX = interpLX(xb)
                revXlist.append(revX)
    revQlist = []
    for x in revXlist:
        try:
            revQ = interpXQ(x)
            revQlist.append(revQ)
        except:
            try:
                xa = x + 0.0001
                revQ = interpXQ(xa)
                revQlist.append(revQ)
            except:
                xb = x - 0.0001
                revQ = interpXQ(xb)
                revQlist.append(revQ)

    interpXR = interp1d(plotGeom[0], plotGeom[1])
    revRlist = []
    for x in revXlist:
        Xminus = x - offset
        _, R = min(XRs.items(), key=lambda x: abs(Xminus - x[0]))
        revRlist.append(R)

    it = -1
    lenRev = len(revXlist)
    lenRevRange = range(0, lenRev)
    XRLQs = []
    for x in lenRevRange:
        it += 1
        lenRevTup = (revXlist[it], revRlist[it], revLlist[it], revQlist[it])
        XRLQs.append(lenRevTup)

    Pcini = Pcin + 1
    coolantInTemps = NIST.initLookupNIST("CH4", range(Pcin, Pcini))
    coolantIn = min(coolantInTemps, key=lambda x: abs(Tcin - x.T))
    it = -1
    lenXRLQs = len(XRLQs)
    lenXRLQsRange = range(0, lenXRLQs)
    segQgas = []
    segQcool = []
    coolantOut = coolantIn
    Lsum = 0
    Qsum = 0

    def B(epsilon):
        if epsilon >= 7:
            B = 4.7 * (epsilon)**0.2
        elif epsilon < 7:
            B = 4.5 + 0.57*(epsilon)**0.75
        return B

    #leave these
    numChannels = 180
    velChannels = 32
    e = 2.5 * 10**-7
    roughSmoo = 0.001
    #channelHeight = Rt * 0.04

    maxIt = len(XRLQs) -2
    Pout = Pcin
    Tout = Tcin
    for entry in XRLQs:
        it += 1
        if it <= maxIt:
            entryA = XRLQs[it]
            ot = it + 1
            if type(XRLQs[ot]) != tuple:
                break
            else:
                pass
            entryB = XRLQs[ot]
            Qavg = (entryA[3] + entryB[3]) / 2
            Qgas = Qavg * neStep
            Lsum += neStep
            segQgas.append(Qgas)
            Qsum += Qgas
            Cavg = ((entryA[1] + entryB[1]) / 2) * 2 * math.pi
            channelWidth = Cavg / (numChannels * 1.5)
            rhoIn = coolantIn.rho
            mdotChannel = mdot / numChannels
            volChannel = mdotChannel / rhoIn
            channelHeight = volChannel / (channelWidth * velChannels)
            Dh = (4 * channelWidth * channelHeight) / (2 * (channelWidth + channelHeight))
            Pr = (coolantIn.Cp * 1000 * coolantIn.mu) / coolantIn.thermalCond
            Re = (coolantIn.rho * velChannels * Dh) / coolantIn.mu
            f = 0.02
            s = neStep
            C1A = 1 + 1.5*Pr**(-1/6) * Re**-0.125 * (Pr - 1) * roughSmoo
            C1B = 1 + 1.5*Pr**(-1/6) * Re**-0.125 * (Pr*roughSmoo - 1)
            C1 = C1A / C1B
            C2 = 1 + (s/Dh)**-0.7 * (Tw / coolantIn.T)**0.1
            R = Rt * math.sqrt(1.5)
            C3 = Re * (0.25*Dh / R)**2
            epsilon = Re * (e / Dh) * math.sqrt(f / 8)
            NuA = (f/8) * Re * Pr * (coolantIn.T / Tw)**0.55
            NuB = 1 + (f/8)**0.55 * (B(epsilon) - 8.48)
            Nu = (NuA / NuB) * C1 * C2 * C3
            hch4 = (Nu * Dh) / coolantIn.thermalCond
            Qcool = hch4 * numChannels * 2 * s * (channelWidth + channelHeight) * 0.5
            segQcool.append(Qcool)
            deltaT = (Qcool / coolantIn.Cp) / mdot
            deltaP1 = ((f * coolantIn.rho * velChannels**2 * s) / (2 * Dh)) / 100000
            Pout = Pcin - deltaP1
            Tout = Tcin + deltaT
            Pcmid = round(Pout)
            Pcmidi = Pcmid + 1
            coolantMidTemps = NIST.initLookupNIST("CH4", range(Pcmid, Pcmidi))
            coolantOut = min(coolantMidTemps, key=lambda x: abs(Tout - x.T))
            print("X PTout HW", entryA[0], Pout, Tout, channelHeight, channelWidth)
            coolantIn = coolantOut
            Pcin = Pout
            Tcin = Tout
        else:
            break

def findf():
    hydrogenPrange = NIST.initLookupNIST("H2", range(400, 401))
    h = min(hydrogenPrange, key=lambda x: abs(300 - x.T))
    Dh = (4 * 0.004 * 0.004) / (2 * (0.004 + 0.004))
    Re = (h.rho * 100 * Dh) / h.mu
    print(h)
    print(Dh)
    print(Re)
    e = 2.5 * 10**-7
    DarcyRange = np.linspace(0.0001, 0.2, num=10000)
    Darcies = []
    for f in DarcyRange:
        left = 1 / math.sqrt(f)
        epsilon = Re * (e / Dh) * math.sqrt(f / 8)
        right = -2 * math.log10(((epsilon / Dh) / 3.7) + (2.51 / (Re * math.sqrt(f))))
        diff = abs(left - right)
        tupe = (left, right, diff)
        Darcies.append(tupe)
        Darcies.sort(key=lambda x: x[2])
    for x in Darcies:
        print(x[0], x[1], x[2])
    pass
