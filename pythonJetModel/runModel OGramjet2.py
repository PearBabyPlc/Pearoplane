import math
import formulaegg as egg
import solvers as solve
import models
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon
plt.style.use('dark_background')

#original ramjet only

H = 3
M0 = 1.5
TlimitRamjet = 4000
Q = 30000
aDeg = 10
dDeg = 30
outboard = 12
Mdesign = 4
AdiffDesign = 3

print("==================================================")
inletDict = models.supersonicInlet(M0, Q, H, aDeg, outboard, Mdesign)
aero = inletDict.get("aerodynamics")
perf = inletDict.get("performance")
geom = inletDict.get("geometry")
stations = inletDict.get("stations")
print("==================================================")
diffXXYYs, diffuserThrust, stationOut, diffDots, mdotDiff, _Adiff = models.subsonicDiffuser(stations[-1], AdiffDesign, H, dDeg, geom)
stations.append(stationOut)
print("==================================================")
stationOut, mdotLH2, mdotOut, ram4XYs, polyRam = models.ramjet(stations[-1], diffDots, H, TlimitRamjet, 0.8, mdotDiff)
stations.append(stationOut)
print("==================================================")
stationThroat, stationOut, ejectorXXYYs, polyEje, Vex, dragCon, thrustDiv = models.linearEjector(stations[-1], ram4XYs, H, dDeg, stations[0], outboard)
stations.append(stationThroat)
stations.append(stationOut)
print("==================================================")

def readStation(x):
    print("M:", x[0])
    print("P (Pa):", x[1])
    print("T (K):", x[2])
    print("rho (kg/m3):", x[3])
    print("gamma:", x[4])
    Ptbar = x[5] / 100000
    print("Pt (Bar):", Ptbar)
    print("Tt (K):", x[6])
    pass

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
print("Vout =", Vex)
print("Inlet drag =", inletDrag)
print("Diffuser thrust =", diffuserThrust)
print("Converging drag =", dragCon)
print("Diverging thrust =", thrustDiv)
Aingest = outboard * H
mdot = V0 * stations[0][3] * Aingest
newtonianThrust = (mdotOut * Vex) - (mdot * V0)
print("Newtonian thrust =", newtonianThrust)
print("Frontal area (m2) =", Aingest)
print("mdot in =", mdot)
print("mdot out =", mdotOut)
print("mdot LH2 =", mdotLH2)
overallThrust = (diffuserThrust + thrustDiv + newtonianThrust) - (dragCon + inletDrag)
print()
print("Overall thrust =", overallThrust)
alternateThrust = newtonianThrust + (Aingest * (stations[-1][1] - stations[0][1]))
print("alternate (correct) formula =", alternateThrust)
Isp = alternateThrust / mdotLH2
print("Isp (m/s) =", Isp)
Ispg = Isp / 9.80665
print("Isp (s) =", Ispg)
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

ax.add_patch(polyRam)
for x in polyEje:
    ax.add_patch(x)

ax.scatter(xCoL, yCoL)
for x in diffDots:
    ax.scatter(x[0], x[1])

for x in ram4XYs:
    ax.scatter(x[0], x[1])

for x in ejectorXXYYs:
    ax.plot(x[0], x[1])

ax.plot(shockXs, shockYs, color='white', linestyle='dashed')
ax.grid()
plt.gca().set_aspect('equal')
plt.savefig("runModel.svg")
plt.show()
    
    

    
