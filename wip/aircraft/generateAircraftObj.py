import math
import numpy as np

# TODO: try to consolidate down into a few more iteratable functions
# REQUIREMENTS: python 3.13, numpy, aircraftUserDefined.obj file in same folder

# finds Sears-Haack body radius at a given position
def getRadii(x, Rmax, L):
    return 2 * math.sqrt(2) * Rmax * ((x / L) * (1 - (x / L)))**0.75

# setting everything up
sampleRate = 1
length = 140
samples = int(length / sampleRate)
linspaceMax = length - sampleRate
linspaceNum = samples - 1
topMaxRadius = 7.5
sideMaxRadius = 12.5
dist = (0.6, 0.7) #distortion factors for (x, y) when drawing the additional two Sears-Haack lines
sideish = math.sqrt((dist[0] * sideMaxRadius)**2 + (dist[1] * topMaxRadius)**2)
sideM = (dist[1] * topMaxRadius) / (dist[0] * sideMaxRadius)
alpha = math.atan(sideM)
endString = "v 0.0 0.0 " + str(length)
fStrings = ["v 0.0 0.0 0.0", endString]
numV = 2

# rightmost vertices
zCoords = np.linspace(sampleRate, linspaceMax, num=linspaceNum)
vNs = []
vN = 2
vNsRight = []
for z in zCoords:
    xCoord = getRadii(z, sideMaxRadius, length)
    vN += 1
    numV += 1
    vNs.append(vN)
    vNsRight.append(vN)
    fString = "v " + str(xCoord) + " 0.0 " + str(z)
    fStrings.append(fString)
vNstring = "f 1 "
for n in vNs:
    vNstring += str(n)
    vNstring += " "
vNstring += "2 1"
fStrings.append(vNstring)

# rightish vertices
vN = vNs[-1]
vNs = []
vNsRightish = []
for z in zCoords:
    radius = getRadii(z, sideish, length)
    vN += 1
    numV += 1
    vNs.append(vN)
    vNsRightish.append(vN)
    xCoord = math.cos(alpha) * radius
    yCoord = math.sin(alpha) * radius
    fString = "v " + str(xCoord) + " " + str(yCoord) + " " + str(z)
    fStrings.append(fString)
#we don't need to generate the rightish slice
#vNstring = "f 1 "
#for n in vNs:
#    vNstring += str(n)
#    vNstring += " "
#vNstring += "2 1"
#fStrings.append(vNstring)

# leftmost vertices
vN = vNs[-1]
vNs = []
vNsLeft = []
for z in zCoords:
    xCoord = getRadii(z, sideMaxRadius, length)
    vN += 1
    numV += 1
    vNs.append(vN)
    vNsLeft.append(vN)
    fString = "v -" + str(xCoord) + " 0.0 " + str(z)
    fStrings.append(fString)
#we DO need to generate the bottom left slice
vNstring = "f 1 "
for n in vNs:
    vNstring += str(n)
    vNstring += " "
vNstring += "2 1"
fStrings.append(vNstring)

## leftish vertices
vN = vNs[-1]
vNs = []
vNsLeftish = []
for z in zCoords:
    radius = getRadii(z, sideish, length)
    vN += 1
    numV += 1
    vNs.append(vN)
    vNsLeftish.append(vN)
    xCoord = math.cos(alpha) * radius
    yCoord = math.sin(alpha) * radius
    fString = "v -" + str(xCoord) + " " + str(yCoord) + " " + str(z)
    fStrings.append(fString)
#we don't need to generate the leftish slice
#vNstring = "f 1 "
#for n in vNs:
#    vNstring += str(n)
#    vNstring += " "
#vNstring += "2 1"
#fStrings.append(vNstring)

# top vertices
vN = vNs[-1]
vNs = []
vNsTop = []
for z in zCoords:
    yCoord = getRadii(z, topMaxRadius, length)
    vN += 1
    numV += 1
    vNs.append(vN)
    vNsTop.append(vN)
    fString = "v 0.0 " + str(yCoord) + " " + str(z)
    fStrings.append(fString)
#we don't need to generate the top slice
#vNstring = "f 1 "
#for n in vNs:
#    vNstring += str(n)
#    vNstring += " "
#vNstring += "2 1"
#fStrings.append(vNstring)

# generating faces
#print("length of vNs left right top", len(vNsLeft), len(vNsRight), len(vNsTop))
vNsRange = range(1, samples)
firstLeft = "f 1 " + str(vNsLeft[0]) + " " + str(vNsLeftish[0])
firstLeftish = "f 1 " + str(vNsLeftish[0]) + " " + str(vNsTop[0])
firstRightish= "f 1 " + str(vNsRightish[0]) + " " + str(vNsTop[0])
firstRight = "f 1 " + str(vNsRight[0]) + " " + str(vNsRightish[0])
fStrings.append(firstLeft)
fStrings.append(firstRight)
fStrings.append(firstLeftish)
fStrings.append(firstRightish)
for x in vNsRange:
    i = x - 1
    lastI = i - 2
    nextLeft = "f " + str(vNsLeft[lastI]) + " " + str(vNsLeftish[lastI]) + " " + str(vNsLeftish[i]) + " " + str(vNsLeft[i])
    nextLeftish = "f " + str(vNsLeftish[lastI]) + " " + str(vNsTop[lastI]) + " " + str(vNsTop[i]) + " " + str(vNsLeftish[i])
    nextRightish = "f " + str(vNsRightish[lastI]) + " " + str(vNsTop[lastI]) + " " + str(vNsTop[i]) + " " + str(vNsRightish[i])
    nextRight = "f " + str(vNsRight[lastI]) + " " + str(vNsRightish[lastI]) + " " + str(vNsRightish[i]) + " " + str(vNsRight[i])
    fStrings.append(nextLeft)
    fStrings.append(nextRight)
    fStrings.append(nextLeftish)
    fStrings.append(nextRightish)
lastLeft = "f 2 " + str(vNsLeft[-1]) + " " + str(vNsLeftish[-1])
lastLeftish = "f 2 " + str(vNsLeftish[-1]) + " " + str(vNsTop[-1])
lastRightish = "f 2 " + str(vNsRightish[-1]) + " " + str(vNsTop[-1])
lastRight = "f 2 " + str(vNsRight[-1]) + " " + str(vNsRightish[-1])
fStrings.append(lastLeft)
fStrings.append(lastRight)
fStrings.append(lastLeftish)
fStrings.append(lastRightish)

# importing user defined geometry
with open("aircraftUserDefined.obj") as uDef:
    for x in uDef:
        uString = str(x.rstrip())
        if uString.startswith("#") == True:
            pass #this removes comments from the .obj file
        elif uString.startswith("f ") == True:
            uTup = uString.split(" ")
            uvNs = []
            for u in uTup:
                if u.isnumeric() == True:
                    uvN = int(u) + numV
                    uvNs.append(uvN)
            uStringUpdated = "f "
            for u in uvNs:
                uStringUpdated += str(u)
                uStringUpdated += " "
            fStrings.append(uStringUpdated)
        else:
            fStrings.append(uString)    

# reading the vertices and their corresponding indices, and printing to the .obj file
i = 0
with open("aircraft.obj", "w") as f:
    for x in fStrings:
        writeString = x + "\n"
        if x.startswith("v ") == True:
            i += 1
            print(x, "num =", i)
        else:
            print(x)
        f.write(writeString)

        

