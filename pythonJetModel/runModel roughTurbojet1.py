import math
import formulaegg as egg
import solvers as solve
import models
import turbojet as turbo
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon
plt.style.use('dark_background')

#rough turbojet only

H = 3
M0 = 2
TlimitTurbojet = 1500
TlimitAfterburner = 2000
TlimitRamjet = 3200
Mcombustion = 0.8
Q = 30000
aDeg = 10
dDeg = 30
outboard = 12
A0 = H * outboard
Mdesign = 4
AdiffDesign = 9

def readStation(x):
    print("M:", x[0])
    V = x[0] * egg.SoS(x[2])
    print("V (m/s):", V)
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

print("==================================================")
inletDict = models.supersonicInlet(M0, Q, H, aDeg, outboard, Mdesign)
aero = inletDict.get("aerodynamics")
perf = inletDict.get("performance")
geom = inletDict.get("geometry")
stations = inletDict.get("stations")
print("==================================================")
diffXXYYs, diffuserThrust, stationOutDiff, diffDots, mdotDiffActual, Adiffactual = models.subsonicDiffuser(stations[-1], AdiffDesign, H, dDeg, geom)
stations.append(stationOutDiff)
print("==================================================")

ramChannel = 0

def roughTurbojetPoly(dots, Diam, length):
    diffOutBtm = dots[-2]
    diffOutTop = dots[-1]
    #print("diffOutBtm", diffOutBtm)
    #print("diffOutTop", diffOutTop)
    diffW = math.floor((diffOutTop[1] - diffOutBtm[1]) - ramChannel)
    yBtm = round(diffOutBtm[1])
    yTop = math.floor(diffOutTop[1]) - ramChannel
    #print("yTop", yTop)
    numTurbojets = math.floor(diffW / Diam)
    numForNp = (2 * numTurbojets) + 1
    #print("num =", numTurbojets)
    #print("diffW =", diffW)
    turboXin = math.ceil(diffOutBtm[0]) + 1
    #print("Xin", turboXin)
    turboYcenters = np.linspace(yBtm, yTop, num=numForNp)
    turboXYs = []
    allXYs = []
    it = 0
    for Y in turboYcenters:
        it += 1
        if it % 2 == 0:
            XY = (turboXin, Y)
            turboXYs.append(XY)
        else:
            XY = (turboXin, Y)
            allXYs.append(XY)

    Radius = Diam / 2
    turboPolys = []
    for xy in turboXYs:
        yU = xy[1] + Radius
        yB = xy[1] - Radius
        eX = xy[0] + length
        xy1 = (xy[0], yU)
        xy2 = (xy[0], yB)
        xy3 = (eX, yB)
        xy4 = (eX, yU)
        xys = (xy1, xy2, xy3, xy4)
        turboPoly = Polygon(xys, facecolor="red", alpha=0.8)
        turboPolys.append(turboPoly)
    return numTurbojets, turboXYs, allXYs, turboPolys
    
numTurbos, turboXYs, allXYs, turboPolys = roughTurbojetPoly(diffDots, H, 8)

###TURBO JETUS###
swirlIn = 54.46232220802562
PcompressorIn = stations[-1][1]
stationOut_guideVanes = turbo.guideVanes(stations[-1], swirlIn)
stations.append(stationOut_guideVanes)

Rt1 = 1.0
Rh1 = 0.2
anglesList = []
#                                                   loading  flow reaction
_, _, _, anglesLP, rpmLP = turbo.compressorStage_design(0.3, 0.25, 0.5, swirlIn, Rt1, Rh1, stations[-1], "exp")

it = 0
LPstages = 6
HPstages = 12
anglesDeg = anglesLP
anglesList = []
RthList = [(Rt1, Rh1)]
while it < LPstages:
    it += 1
    Rt1, Rh1, stationOut, anglesDeg = turbo.compressorStage_actual(rpmLP, anglesDeg, Rt1, Rh1, stations[-1], "exp")
    stations.append(stationOut)
    anglesList.append(anglesDeg)
    Rth = (Rt1, Rh1)
    RthList.append(Rth)

Rt1 = RthList[-1][0]
Rh1 = RthList[-1][1]
PcompressorLP = stations[-1][1]
_, _, _, anglesHP, rpmHP = turbo.compressorStage_design(0.32, 0.2, 0.5, swirlIn, Rt1, Rh1, stations[-1], "con")

it = 0
anglesDeg = anglesHP
while it < HPstages:
    it += 1
    Rt1, Rh1, stationOut, anglesDeg = turbo.compressorStage_actual(rpmHP, anglesDeg, Rt1, Rh1, stations[-1], "exp")
    stations.append(stationOut)
    anglesList.append(anglesDeg)
    Rth = (Rt1, Rh1)
    RthList.append(Rth)

PcompressorHP = stations[-1][1]
LPopr = PcompressorLP / PcompressorIn
HPopr = PcompressorHP / PcompressorLP
compOPR = PcompressorHP / PcompressorIn
fullOPR = PcompressorHP / stations[0][1]
print("LP OPR rpm =", LPopr, rpmLP)
print("HP OPR rpm =", HPopr, rpmHP)
print("Compressor OPR =", compOPR)
print("Overall pressure ratio =", fullOPR)

print("==================================================")
for x in anglesList:
    print(x)
print("==================================================")
it = -1
for x in stations:
    it += 1
    print("station", it)
    readStation(x)
    print()
print("==================================================")
V0 = egg.SoS(stations[0][2]) * stations[0][0]
inletDrag = perf.get("dragTotal")
print("Min =", stations[0][0])
print("Vin =", V0)
print("Ain =", A0)
mdot0 = V0 * A0 * stations[0][3]
print("mdot =", mdot0)
print()
print("Mdiff =", stationOutDiff[0])
V1 = egg.SoS(stations[-1][2]) * stations[-1][0]
print("Vdiff =", V1)
diffCross = abs(diffDots[-1][1] - diffDots[-2][1])
diffA = diffCross * H
print("Adiff =", Adiffactual)
mdot1 = V1 * diffA * stations[-1][3]
print("mdot =", mdot1)
print("mdotDiffActual", mdotDiffActual)
print("==================================================")
sDegList = aero.get("sList (deg)")
aDegList = aero.get("aList (deg)")
asDeg = dict(zip(aDegList, sDegList))
for x in asDeg.items():
    print("deflection angle =", x[0])
    print("shock angle =", x[1])
    print()
print("==================================================")

fig, ax = plt.subplots()

rampXXYYs = geom.get("rampXXYYs")
lipXXYYs = geom.get("lipXXYYs")
dotXYs = geom.get("dotXYs")
triangles = geom.get("triangles")
(xCoL, yCoL) = geom.get("xyCoL")
shockXs = geom.get("shockXs")
shockYs = geom.get("shockYs")

#print("diffuser xys")
#print(diffXXYYs[0][-1], outboard)
#print(diffXXYYs[0][-1], diffXXYYs[1][-1])

for XY in rampXXYYs:
    #print("ramp", XY)
    ax.plot(XY[0], XY[1])

for XY in lipXXYYs:
    #print("lip", XY)
    ax.plot(XY[0], XY[1])

for xy in dotXYs:
    ax.scatter(xy[0], xy[1])
    
for tri in triangles:
    ax.add_patch(tri)

ax.plot((diffXXYYs[0][0], diffXXYYs[-1][0]), (outboard, outboard), color='white')

for diff in diffXXYYs:
    ax.plot(diff[0], diff[1])

ax.scatter(xCoL, yCoL)
for x in diffDots:
    ax.scatter(x[0], x[1])

for x in turboPolys:
    ax.add_patch(x)

for x in allXYs:
    ax.scatter(x[0], x[1], alpha=0.5, s=50)

for x in turboXYs:
    ax.scatter(x[0], x[1], alpha=1.0, s=100)
    

ax.plot(shockXs, shockYs, color='white', linestyle='dashed')
ax.grid()
plt.gca().set_aspect('equal')
plt.savefig("runModel.svg")
plt.show()
    
    

    
