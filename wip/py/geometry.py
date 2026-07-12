import math
import numpy as np
from scipy import integrate
import matplotlib.pyplot as plt
plt.style.use('dark_background')

def getGeometry(Rt, conR, expR, angleTC, angleTE, angleNE, Lstar):
    #inputs
    #conR
    #expR
    radTC = math.radians(angleTC)
    radTE = math.radians(angleTE)
    radNE = math.radians(angleNE)
    #Lstar
    
    #constants
    radCon = math.radians(30)
    radExp = math.radians(18)
    multiCon = 1.0
    multiExp = 0.8
    multiTE = 0.382
    multiTC = 1.5
    CCLXX = math.radians(270)
    CCCLX = math.radians(360)
    xylList = []
    #l values are distance from the throat, for now

    #throat expanding te
    Rte = Rt * multiTE
    def f_xte(t):
        return Rte * math.cos(t + CCLXX)
    def f_yte(t):
        return Rte * math.sin(t + CCLXX) + Rte + Rt
    xytes = []
    radTErange = np.linspace(0, radTE, num=16)
    for t in radTErange:
        xte = f_xte(t)
        yte = f_yte(t)
        xyte = (xte, yte)
        xytes.append(xyte)
        lte = 2 * math.pi * Rte * (t * (1 / CCCLX))
        xyl = (xte, yte, lte, "te")
        xylList.append(xyl)
    xte = f_xte(radTE)
    yte = f_yte(radTE)

    #xylList.pop(0)
    xylList.reverse()
    xylThroat = (0.0, Rt, 0.0, "th")
    xylList.append(xylThroat)
    xyltcs = []
    
    #throat converging tc
    Rtc = Rt * multiTC
    def f_xtc(t):
        return Rtc * math.cos(CCLXX - t)
    def f_ytc(t):
        return Rtc * math.sin(CCLXX - t) + Rtc + Rt
    xytcs = []
    radTCrange = np.linspace(0, radTC, num=16)
    for t in radTCrange:
        xtc = f_xtc(t)
        ytc = f_ytc(t)
        xytc = (xtc, ytc)
        xytcs.append(xytc)
        ltc  = 2 * math.pi * Rtc * (t * (1 / CCCLX))
        xyl = (xtc, ytc, ltc, "tc")
        xyltcs.append(xyl)
    xtc = f_xtc(radTC)
    ytc = f_ytc(radTC)
    #xyltcs.pop(0)
    xylList = xylList + xyltcs

    #nozzle bezier n
    Re = Rt * math.sqrt(expR)
    Ln = multiExp * (Re / math.tan(radExp))
    xne = Ln
    yne = Re
    mn = math.tan(radTE)
    me = math.tan(radNE)
    cn = yte - mn*xte
    ce = yne - me*xne
    Qxn = (ce - cn) / (mn - me)
    Qyn = (mn*ce - me*cn) / (mn - me)
    def f_xn(t):
        return ((1 - t)**2 * xte) + (2 * (1 - t) * t * Qxn) + (t**2 * xne)
    def f_yn(t):
        return ((1 - t)**2 * yte) + (2 * (1 - t) * t * Qyn) + (t**2 * yne)
    xyns = []
    tRange = np.linspace(0, 1, num=51)
    for t in tRange:
        xn = f_xn(t)
        yn = f_yn(t)
        xyn = (xn, yn)
        xyns.append(xyn)

    #nozzle length calc
    it = -1
    lnEnd = len(xyns) #- 1
    lnRange = range(1, lnEnd)
    ln0 = xylList[0][2]
    xyln0 = (xyns[0][0], xyns[0][1], ln0, "ne")
    xylns = [xyln0]
    for xaba in lnRange:
        it += 1
        (x1, y1) = xyns[it]
        ot = it + 1
        (x2, y2) = xyns[ot]
        run = abs(x1 - x2)
        rise = abs(y1 - y2)
        hyp = math.sqrt(run**2 + rise**2)
        ln0 += hyp
        xyln = (x2, y2, ln0, "ne")
        xylns.append(xyln)
    xylList.reverse()
    #xylns.pop(0)
    xylList = xylList + xylns
    
    #chamber converging bezier cc
    Rc = Rt * math.sqrt(conR)
    xcc = -multiCon * (Rc / math.tan(radCon))
    ycc = Rc
    mct = math.tan(-radTC)
    ctc = ytc - mct*xtc
    Qxc = (Rc - ctc) / mct
    Qyc = (mct * Rc) / mct
    def f_xc(t):
        return ((1 - t)**2 * xtc) + (2 * (1 - t) * t * Qxc) + (t**2 * xcc)
    def f_yc(t):
        return ((1 - t)**2 * ytc) + (2 * (1 - t) * t * Qyc) + (t**2 * ycc)
    xyccs = []
    tRange = np.linspace(0, 1, num=26)
    for t in tRange:
        xc = f_xc(t)
        yc = f_yc(t)
        xyc = (xc, yc)
        xyccs.append(xyc)

    #chamber converging length calc
    it = -1
    lccEnd = len(xyccs) - 1
    lccRange = range(1, lccEnd)
    lcc0 = xylList[0][2]
    xylcc0 = (xyccs[0][0], xyccs[0][1], lcc0, "cc")
    xylccs = [xylcc0]
    for xbab in lccRange:
        it += 1
        (x1, y1) = xyccs[it]
        ot = it + 1
        (x2, y2) = xyccs[ot]
        run = abs(x1 - x2)
        rise = abs(y1 - y2)
        hyp = math.sqrt(run**2 + rise**2)
        lcc0 += hyp
        xylcc = (x2, y2, lcc0, "cc")
        xylccs.append(xylcc)
    
    xylList.reverse()
    #xylccs.pop(0)
    xylList = xylList + xylccs

    #chamber straight cs
    def f_acs(x):
        herbert = -((x - xtc) / (xtc - xcc))
        return math.pi * (f_yc(x) * herbert)**2
    Vcc, _ = integrate.quad(lambda x: f_acs(x), xcc, xtc)
    def f_atc(x):
        gerbert = -math.sqrt(Rtc**2 - x**2) + Rtc + Rt
        return math.pi * gerbert**2
    Vtc, _ = integrate.quad(lambda x: f_atc(x), xtc, 0)
    Vc = math.pi * Lstar * Rt**2
    Vcs = Vc - (Vcc + Vtc)
    Lcs = Vcs / (math.pi * Rc**2)
    xin = xcc - Lcs
    xycsIn = (xin, Rc)
    xycsOut = (xcc, Rc)
    xycss = [xycsIn, xycsOut]

    #chamber straight lengths
    lcs0 = -xycss[0][0]
    lcs1 = -xycss[1][0]
    xylcss0 = (xin, Rc, lcs0, "in")
    xylcss = []
    xcsslinspace = np.linspace(xcc, xin, num=3)
    xcshmm = xylList[-1][0]
    for xcss in xcsslinspace:
        lcshmm = lcs1 + abs(xcshmm - xcss)
        xylcssAppend = (xcss, Rc, lcshmm, "cs")
        xylcss.append(xylcssAppend)
    xylcss.pop(-1)
    xylcss.append(xylcss0)
    xylList = xylList + xylcss

    #reversing and putting it all together so lengths are from injector
    xylList.reverse()
    xylChamber = []
    xylNozzle = []
    for x in xylList:
        if (x[3] == "in") or (x[3] == "cs") or (x[3] == "cc") or (x[3] == "tc"):
            chamberx = x[0] - xin
            chambery = x[1]
            chamberl = -x[2] + lcs0
            chambera = x[3]
            chamber = (chamberx, chambery, chamberl, chambera)
            xylChamber.append(chamber)
        elif (x[3] == "te") or (x[3] == "ne"):
            nozzlex = x[0] - xin
            nozzley = x[1]
            nozzlel = x[2] + lcs0
            nozzlea = x[3]
            nozzle = (nozzlex, nozzley, nozzlel, nozzlea)
            xylNozzle.append(nozzle)
        elif x[3] == "th":
            xylThroat = [(-xin, x[1], lcs0, x[3])]
    geomXYLdescA = xylChamber + xylThroat + xylNozzle
    geomXYLdesc = []
    offset = geomXYLdescA[1][0] - geomXYLdescA[1][2]
    for x in geomXYLdescA:
        gax = x[0]
        gbx = x[1]
        gcx = x[2] + offset
        gdx = x[3]
        gtup = (gax, gbx, gcx, gdx)
        geomXYLdesc.append(gtup)
    actualFirstTup = (0.0, Rc, 0.0, "in")
    geomXYLdesc.pop(0)
    geomXYLdesc.insert(0, actualFirstTup)

    #putting geometry together for plotting
    xyccs.reverse()
    xytcs.reverse()
    xysSorted = xycss + xyccs + xytcs + xytes + xyns
    xsSorted = []
    ysSorted = []
    for xy in xysSorted:
        x = xy[0]
        y = xy[1]
        xsSorted.append(x)
        ysSorted.append(y)
    geomXYlists = (xsSorted, ysSorted)

    return geomXYlists, geomXYLdesc

def plotGeometry(geometry):
    xList = geometry[0]
    yList = geometry[1]
    mirrorYlist = []
    for y in yList:
        mirrorY = -y
        mirrorYlist.append(mirrorY)
    
    #matplotlib time
    fig, ax = plt.subplots(1, 1, sharex='col')
    ax.plot(xList, yList, color='white')
    ax.plot(xList, mirrorYlist, color='white')
    ax.grid()
    plt.gca().set_aspect('equal')
    plt.savefig("HMX3Kgeom_29dec.pdf")
    plt.show()
    
