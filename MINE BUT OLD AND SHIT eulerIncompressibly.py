import math
import matplotlib.pyplot as plt
import time
import csv

# see: https://matthias-research.github.io/pages/tenMinutePhysics/17-fluidSim.pdf

nit = 50 #number of simulation frames/iterations
dims = (10, 10) #define grid
dt = 1 / 20 #define sim timestep, here it's 20fps
g = 9.81 #define gravity
overrelaxation = 1.9 #unused simplification for the "projection" process

initU = 0.04 #initial horizonal velocity, positive = rightward
initV = 0.11  #initial vertical velocity, positive = upward

#create first grid full of cells
xdim = dims[0] + 1
ydim = dims[1] + 1
xrange = range(0, xdim, 1)
yrange = range(0, ydim, 1)
cells = {}
for x in xrange:
    for y in yrange:
        xy = (x, y)
        uvs = [initU, initV, 1]
        cells.update({xy: uvs})

#record first grid
it = 0
grids = []
gridInit = (it, cells)
grids.append(gridInit)   

#apply gravity
def doGravity(gridIn, dt, g):
    itIn = gridIn[0]
    cellsIn = gridIn[1]
    cellsOut = {}
    for cell in cellsIn.items():
        xyIn = cell[0]
        uvsIn = cell[1]
        vOut = uvsIn[1] - (dt * g)
        uvsOut = [uvsIn[0], vOut, uvsIn[2]]
        cellsOut.update({xyIn: uvsOut})
    itOut = itIn + 0.2
    gridOut = (itOut, cellsOut)
    return gridOut

#this renders the fluid incompressible, idk that's important apparently
def doGaussSeidel(gridIn, overrelaxed):
    itIn = gridIn[0]
    cellsInConst = gridIn[1]
    cellsIn = cellsInConst.copy()
    xyList = list(cellsInConst.keys())
    maxX = max(xyList, key=lambda x: x[0])[0]
    maxY = max(xyList, key=lambda x: x[1])[1]
    cellsOut = cellsIn.copy()
    uvsObstacle = (0, 0, 0)
    for cell in cellsInConst.items():
        for cell in cellsIn.items():
            xyIn = cell[0]
            uvsIn = cell[1]
            xR = xyIn[0] + 1
            xyR = (xR, xyIn[1])
            uvsR = cellsIn.get(xyR, uvsObstacle)
            yT = xyIn[1] + 1
            xyT = (xyIn[0], yT)
            uvsT = cellsIn.get(xyT, uvsObstacle)
            if overrelaxed == False:
                d = uvsR[0] - uvsIn[0] + uvsT[1] - uvsIn[1]
            elif overrelaxed == True:
                d = overrelaxation * (uvsR[0] - uvsIn[0] + uvsT[1] - uvsIn[1])
            xL = xyIn[0] - 1
            xyL = (xL, xyIn[1])
            uvsL = cellsIn.get(xyL, uvsObstacle)
            yB = xyIn[1] - 1
            xyB = (xyIn[0], yB)
            uvsB = cellsIn.get(xyB, uvsObstacle)
            s = uvsR[2] + uvsL[2] + uvsT[2] + uvsB[2]
            uOut = uvsIn[0] + (d * uvsL[2]) / s
            uOutR = uvsR[0] - (d * uvsR[2]) / s
            vOut = uvsIn[1] + (d * uvsB[2]) / s
            vOutT = uvsT[1] - (d * uvsT[2]) / s
            uvsOut = (uOut, vOut, 1)
            uvsOutR = (uOutR, uvsR[1], 1)
            uvsOutT = (uvsT[0], vOutT, 1)
            if (xR <= maxX) and (yT <= maxY):
                cellsOut.update({xyIn: uvsOut})
                cellsOut.update({xyR: uvsOutR})
                cellsOut.update({xyT: uvsOutT})
            elif xR <= maxX:
                cellsOut.update({xyIn: uvsOut})
                cellsOut.update({xyR: uvsOutR})
            elif yT <= maxY:
                cellsOut.update({xyIn: uvsOut})
                cellsOut.update({xyT: uvsOutT})
            else:
                cellsOut.update({xyIn: uvsOut})
        cellsIn = cellsOut.copy()
    itOut = itIn + 0.5
    gridOut = (itOut, cellsIn)
    return gridOut

#this moves the fluid then calculates what the velocities at the original coords are 
def doAdvection(gridIn, dt):
    itIn = gridIn[0]
    cellsIn = gridIn[1]
    cellsOff = {}
    cellsOn = {}
    cellsOut = {}
    for cell in cellsIn.items():
        xyIn = cell[0]
        uvsOff = cell[1]
        xIn = xyIn[0]
        yIn = xyIn[1]
        uIn = uvsOff[0]
        vIn = uvsOff[1]
        xOff = xIn + (uIn * dt)
        yOff = yIn + (vIn * dt)
        xyOff = (xOff, yOff)
        cellsOff.update({xyOff: uvsOff})
    for cell in cellsIn.items():
        xyIn = cell[0]
        xIn = xyIn[0]
        yIn = xyIn[1]
        for coff in cellsOff.items():
            xyOff = coff[0]
            uvsOff = coff[1]
            xOff = xyOff[0]
            yOff = xyOff[1]
            xDist = abs(xIn - xOff)
            yDist = abs(yIn - yOff)
            dist = math.sqrt(xDist**2 + yDist**2)
            distWeight = 1 / dist
            cellsOn.update({distWeight: uvsOff})
        cellsDist = dict(sorted(cellsOn.items()))
        cellsDlst = list(cellsDist.items())
        cellNa = cellsDlst[0]
        cellNb = cellsDlst[1]
        uvsNa = cellNa[1]
        uvsNb = cellNb[1]
        uMean = ((uvsNa[0] * cellNa[0]) + (uvsNb[0] * cellNb[0])) / (cellNa[0] + cellNb[0])
        vMean = ((uvsNa[1] * cellNa[0]) + (uvsNb[1] * cellNb[0])) / (cellNa[0] + cellNb[0])
        sNrst = uvsNa[2]
        uvsMean = (uMean, vMean, sNrst)
        cellsOut.update({xyIn: uvsMean})
    itOut = itIn + 0.3
    gridOut = (itOut, cellsOut)
    return gridOut

#run simulation
it = 0
itTimes = []
totalStart = time.perf_counter()
while it < nit:
    itStart = time.perf_counter()
    gridIn = grids[it]
    gridGrav = doGravity(gridIn, dt, g)
    gridProj = doGaussSeidel(gridGrav, False)
    gridOut = doAdvection(gridProj, dt)
    it += 1
    grids.append(gridOut)
    itEnd = time.perf_counter()
    itTime = itEnd - itStart
    itTimes.append(itTime)

#performance shit
totalEnd = time.perf_counter()
totalTime = totalEnd - totalStart
print()
print("Total time:", totalTime)
it = 0
for ti in itTimes:
    it += 1
    print("Iteration, time:", it, ti)

#record the whole simulation as a csv for later 
with open("euler.csv", 'w', newline='') as csvfile:
    writer = csv.writer(csvfile, delimiter=' ', quotechar='|', quoting=csv.QUOTE_MINIMAL)
    writer.writerow(['iteration', 'x-y-u-v'])
    for grid in grids:
        writer.writerow([])
        it = grid[0]
        itStr = str("Iteration number " + str(it))
        writer.writerow([itStr])
        cells = grid[1]
        clls = cells.items()
        for clll in clls:
            clxy = clll[0]
            cluv = clll[1]
            writer.writerow([clxy[0], clxy[1], cluv[0], cluv[1]])

#get the final grid output and prepare it for plotting
finalGrid = grids[-1]
finalCells = finalGrid[1]
finalCellsList = finalCells.items()
plotX = []
plotY = []
plotU = []
plotV = []
for cell in finalCellsList:
    xy = cell[0]
    x = xy[0]
    y = xy[1]
    uvs = cell[1]
    u = uvs[0]
    v = uvs[1]
    plotX.append(x)
    plotY.append(y)
    plotU.append(u)
    plotV.append(v)

#plot that shit
plt.style.use('dark_background')
fig, ax1 = plt.subplots(1)
ax1.quiver(plotX, plotY, plotU, plotV, color='white')
ax1.grid()
plt.savefig("euler.pdf")
plt.show()
