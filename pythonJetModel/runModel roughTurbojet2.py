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

H = 3.7
M0 = 7
TlimitTurbojet = 1500
TlimitAfterburner = 2000
TlimitRamjet = 3200
Mcombustion = 0.8
Q = 30000
aDeg = 10
dDeg = 30
outboard = 12.5
A0 = H * outboard
Mdesign = 4
AdiffDesign = 12

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

print("==================================================")

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

liftTons = (perf.get("liftTotal") / (1000 * 9.80665)) * 2
print("lift tons", liftTons)
print("lift tons", liftTons)


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

ax.scatter(xCoL, yCoL)   

ax.plot(shockXs, shockYs, color='white', linestyle='dashed')
ax.grid()
plt.gca().set_aspect('equal')
plt.savefig("runModel.svg")
plt.show()
    
    

    
