import math
import numpy as np

def SearsHaackRadius(x, Rmax, L): 
    return 2 * math.sqrt(2) * Rmax * ((x / L) * (1 - (x / L)))**0.75

def generateFuselage(sampleRate, length, topRmax, sideRmax, xDistortion, yDistortion):
    output = []
    sideish = math.sqrt((xDistortion * sideRmax)**2 + (yDistortion * topRmax)**2)
    sideM = (yDistortion * topRmax) / (xDistortion * sideRmax)
    alpha = math.atan(sideM)
    
    output.append("v 0.0 0.0 0.0") #1
    SR1 = SearsHaackRadius(sampleRate, sideRmax, length)
    TR1 = SearsHaackRadius(sampleRate, topRmax, length)
    BR1 = math.sqrt((TR1 * yDistortion)**2 + (SR1 * xDistortion)**2)
    aSR1 = -SR1
    abX = math.cos(alpha) * BR1
    abY = math.sin(alpha) * BR1
    bbX = -abX
    bbY = abY
    output.append(f"v {SR1} 0.0 {sampleRate}") #2
    output.append(f"v {abX} {abY} {sampleRate}") #3
    output.append(f"v 0.0 {TR1} {sampleRate}") #4
    output.append(f"v {bbX} {bbY} {sampleRate}") #5
    output.append(f"v {aSR1} 0.0 {sampleRate}") #6
    output.append("f 1 2 3")
    output.append("f 1 3 4")
    output.append("f 1 4 5")
    output.append("f 1 5 6")
    output.append("f 1 2 6")

    samples = int(length / sampleRate)
    linspaceMax = length - sampleRate
    linspaceNum = samples - 1
    zCoords = np.linspace(sampleRate, linspaceMax, num=linspaceNum)

    it = 7
    for z in zCoords:
        SRn = SearsHaackRadius(z, sideRmax, length)
        TRn = SearsHaackRadius(z, topRmax, length)
        BRn = math.sqrt((TRn * yDistortion)**2 + (SRn * xDistortion)**2)
        aSRn = -SRn
        abX = math.cos(alpha) * BRn
        abY = math.sin(alpha) * BRn
        bbX = -abX
        bbY = abY
        output.append(f"v {SRn} 0.0 {z}") #it + 0
        output.append(f"v {abX} {abY} {z}") #it + 1
        output.append(f"v 0.0 {TRn} {z}") #it + 2
        output.append(f"v {bbX} {bbY} {z}") #it + 3
        output.append(f"v {aSRn} 0.0 {z}") #it + 4
        #print(f"abxyz {abX} {abY} {z}")
        #Left f 2 7 8 3
        #lish f 3 8 9 4
        #rish f 4 9 10 5
        #righ f 5 10 11 6
        leA = it - 5
        leB = it #+ 0
        leC = it + 1
        leD = it - 4
        liA = it - 4
        liB = it + 1
        liC = it + 2
        liD = it - 3
        riA = it - 3
        riB = it + 2
        riC = it + 3
        riD = it - 2
        reA = it - 2
        reB = it + 3
        reC = it + 4
        reD = it - 1
        unA = it - 5
        unB = it #+0
        unC = it + 4
        unD = it - 1
        output.append(f"f {leA} {leB} {leC} {leD}")
        output.append(f"f {liA} {liB} {liC} {liD}")
        output.append(f"f {riA} {riB} {riC} {riD}")
        output.append(f"f {reA} {reB} {reC} {reD}")
        output.append(f"f {unA} {unB} {unC} {unD}")

        it += 5

    output.append(f"v 0.0 0.0 {length}") #last
    SR1 = SearsHaackRadius(sampleRate, sideRmax, length)
    TR1 = SearsHaackRadius(sampleRate, topRmax, length)
    BR1 = math.sqrt((TR1 * yDistortion)**2 + (SR1 * xDistortion)**2)
    aSR1 = -SR1
    abX = math.cos(alpha) * BR1
    abY = math.sin(alpha) * BR1
    bbX = -abX
    bbY = abY
    output.append(f"v {SR1} 0.0 {linspaceMax}") #it + 1
    output.append(f"v {abX} {abY} {linspaceMax}") #it + 2
    output.append(f"v 0.0 {TR1} {linspaceMax}") #it + 3
    output.append(f"v {bbX} {bbY} {linspaceMax}") #it + 4
    output.append(f"v {aSR1} 0.0 {linspaceMax}") #it + 5

    it1 = it + 1
    it2 = it + 2
    it3 = it + 3
    it4 = it + 4
    it5 = it + 5
    output.append(f"f {it} {it1} {it2}")
    output.append(f"f {it} {it2} {it3}")
    output.append(f"f {it} {it3} {it4}")
    output.append(f"f {it} {it4} {it5}")
    output.append(f"f {it} {it1} {it5}")

    return output

def generateHexAirfoil(rootChord, tipChord, span, thickness, c1, c2, tipSweep, hedral, trim, ox, oy, oz):
    rootT = rootChord * thickness
    tipT = tipChord * thickness
    #trim unused for now
    
    xRoot = ox
    xTip = ox + span
    halfRT = 0.5 * thickness * rootChord
    halfTT = 0.5 * thickness * tipChord
    yUR = oy + halfRT
    yLR = oy - halfRT
    yUT = (oy + halfTT) + hedral
    yLT = (oy - halfTT) + hedral
    yT = oy + hedral
    z1 = oz
    z2 = z1 + (c1 * rootChord)
    z3 = z1 + (c2 * rootChord)
    z4 = z1 + rootChord
    z8 = z4 + tipSweep
    z5 = z8 - tipChord
    z6 = z5 + (c1 * tipChord)
    z7 = z5 + (c2 * tipChord)

    output = []
    output.append(f"v {xRoot} {oy} {z1}") #1

    output.append(f"v {xRoot} {yUR} {z2}") #2
    output.append(f"v {xRoot} {yLR} {z2}") #3

    output.append(f"v {xRoot} {yUR} {z3}") #4
    output.append(f"v {xRoot} {yLR} {z3}") #5

    output.append(f"v {xRoot} {oy} {z4}") #6

    output.append(f"v {xTip} {yT} {z5}") #7

    output.append(f"v {xTip} {yUT} {z6}") #8
    output.append(f"v {xTip} {yLT} {z6}") #9

    output.append(f"v {xTip} {yUT} {z7}") #10
    output.append(f"v {xTip} {yLT} {z7}") #11

    output.append(f"v {xTip} {yT} {z8}") #12

    output.append("f 7 8 10 12 11 9")
    output.append("f 1 2 8 7")
    output.append("f 2 8 10 4")
    output.append("f 4 10 12 6")
    output.append("f 1 3 9 7")
    output.append("f 3 9 11 5")
    output.append("f 5 11 12 6")

    return output

daFuselage = generateFuselage(5, 140, 7.5, 12.5, 0.6, 0.7)
wingLeft = generateHexAirfoil(74, 6, 22.5, 0.02, 0.25, 0.75, -2, -1, "trim", 12.5, 0, 40)
wingRight = generateHexAirfoil(74, 6, -22.5, 0.02, 0.25, 0.75, -2, -1, "trim", -12.5, 0, 40)

with open("aircraft.obj", "w") as f:
    for x in daFuselage:
        print(x)
        f.write(x)
        f.write("\n")

with open("wingL.obj", "w") as wf:
    for x in wingLeft:
        wf.write(x)
        wf.write("\n")

with open("wingR.obj", "w") as wfa:
    for x in wingRight:
        wfa.write(x)
        wfa.write("\n")
