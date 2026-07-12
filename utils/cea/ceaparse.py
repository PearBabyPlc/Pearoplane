#          _______________    ____                     
#         / ____/ ____/   |  / __ \____ ______________ 
#        / /   / __/ / /| | / /_/ / __ `/ ___/ ___/ _ \
#       / /___/ /___/ ___ |/ ____/ /_/ / /  (__  )  __/
#       \____/_____/_/  |_/_/    \__,_/_/  /____/\___/ 
#                                               
#      A python utility for NASA's CEARun by Pearbaby plc
#-------------------------------------------------------------#
#              ceaparse.parseMCC("filename.cgi")              #
#-------------------------------------------------------------#
# For parsing main combustion chamber CEARUN outputs
# Feeds into Bartz correlation integration and suchnot
# (Regenerative cooling and expander cycle capacity, 
#  preliminary materials choices, etc...)
#
# GUIDELINES FOR INPUT:
# Single chamber pressure and mixture ratio
# No subsonic area ratios, as many supersonic as possible
# Exit pressure ratios are fine. Single contraction ratio only
# Include transport properties, and consider ionised species
# Chamber mass flux should be fine. Finite area combustor
#
#-------------------------------------------------------------#
#              ceaparse.parsePB("filename.cgi")               #
#-------------------------------------------------------------#
# For parsing preburner CEARUN outputs, informing
# turbomachinery design and cycle choices
#
# GUIDELINES FOR INPUT:
# Wide variety of chamber pressures and mixture ratios allowed
# No area ratios, infinite area combustor means no contraction
# Include transport properties, and consider ionised species
#
#-------------------------------------------------------------#
#                            TODO                             #
#-------------------------------------------------------------#
#   PEAROPLANE C PROJECT IN GENERAL:
# - Define necessary types and methods for rocket engines in C
# - Probably use PyInstaller to bundle into a single package
#   for the main simulator, so it can read data from resources
#
#   CEAPARSE PYTHON STUFF:
# - Refactor to allow the processing of any NASA CEARUN
#   output into appropriate Python object data (likely need
#   to define additional classes and functions for that cover
#   all the potential input scenarios, and implement a fool-
#   proof input analysing function that creates appropriate
#   objects and calls necessary functions.
#
#   NASA CEA FORTRAN THINGY THINGS:
# - Compile actual NASA CEA Fortran program
# - Wrap binary as necessary for use in the Pearoplane sim
#   (import as a Python module and inclusion as a C header)
#   (Unsure of license obligations with NASA, CHECKB4USHREK)

import numpy as np

def splitList(lst, chunkSize):
    return (lst[i:i + chunkSize] for i in range(0, len(lst), chunkSize))

def parseLine(lineIn):
    lstA = lineIn.split("  ")
    lstB = [x.strip() for x in lstA]
    lstC = list(filter(None, lstB))
    return lstC

def parseBlock(blockIn):
    parsedBlock = []
    for line in blockIn:
        lstP = parseLine(line)
        parsedBlock.append(lstP)
    preparedBlock = parsedBlock
    preparedBlock[0].insert(0, "POSITION")
    preparedBlock[-1].insert(1, "INJ")
    preparedBlock[-2].insert(1, "INJ")
    preparedBlock[-3].insert(1, "INJ")
    return preparedBlock

class Position:
    def __init__(self, position, P, T, Cp, gamma, SoS, M, mu, Pr, AR, Cstar, Isp, rhoThroat):
        try:
            self.position = str(position)
            self.P = float(P)
            self.T = float(T)
            self.Cp = float(Cp)
            self.gamma = float(gamma)
            self.SoS = float(SoS)
            self.M = float(M)
            self.mu = float(mu)
            self.Pr = float(Pr)
            self.rhoThroat = float(rhoThroat)
            if (AR == "INJ") or (AR == "Ae/At"):
                self.AR = float(0)
                self.Cstar = float(0)
                self.Isp = float(0)
            else:
                try:
                    self.AR = float(AR)
                    self.Cstar = float(Cstar)
                    self.Isp = float(Isp)
                except:
                    self.AR = "WARNING! Bad input!"
                    self.Cstar = "WARNING! Bad input!"
                    self.Isp = "WARNING! Bad input!"
        except:
            exceptString = str("         position: " + str(self.position) +
                         ", P: " + str(P) +
                         ", T: " + str(T) +
                         ", \n         Cp: " + str(Cp) +
                         ", gam: " + str(gamma) +
                         ", SoS: " + str(SoS) +
                         ", M:" + str(M) +
                         ", \n         mu: " + str(mu) +
                         ", Pr: " + str(Pr) +
                         ", AR: " + str(AR) +
                         ", Cstar: " + str(Cstar) +
                         ", Isp: " + str(Isp))
            print()
            print("WARNING! class Position and its methods (__str__, debug) failed to initialise!")
            print("         Bad data input at an unexpected location - AR, Cstar and Isp are OK")
            print("         as their INJECTOR data is expected to be a string. Bad input below:")
            print(exceptString)
            print()

    def __str__(self):
        try:
            stringReturn = str("Position: " + str(self.position) + "\nPressure (bar): " + str(self.P) +
                               "\nTemperature (Kelvin): " + str(self.T) + "\nCp (kJ/kg-K): " + str(self.Cp) +
                               "\ngamma: " + str(self.gamma) + "\nSpeed of sound (m/s): " + str(self.SoS) +
                               "\nMach number: " + str(self.M) + "\nViscosity (millipoise): " + str(self.mu) +
                               "\nPrandtl number: " + str(self.Pr) + "\nArea / throatArea: " + str(self.AR) +
                               "\nC* (m/s): " + str(self.Cstar) + "\nIsp/exhaust velocity (m/s): " + str(self.Isp) +
                               "\nrho at throat (kg/m3): "+ str(self.rhoThroat))
            return stringReturn
        except:
            return "ERROR! Position string return not initialised!"

    def summary(self):
        summaryString = str(str(self.position) + ": A/At = " + str(self.AR) +
                            ", T = " + str(self.T) + "K, P = " + str(self.P) +
                            "bar, V = " + str(self.Isp) + "m/s, rhoThroat = " + str(self.rhoThroat) + "kg/m3")
        #return summaryString
        print(summaryString)

    def debug(self):
        self.position = "Debug"
        self.P = 0
        self.T = 0
        self.Cp = 0
        self.gamma = 0
        self.SoS = 0
        self.M = 0
        self.mu = 0
        self.Pr = 0
        self.AR = 0
        self.Cstar = 0
        self.Isp = 0

class Preburner:
    def __init__(self, OF, percentFuel, P, Pch, T, rho, Cp, gamma, SoS, mu, Pr, massFractions):
        try:
            self.OF = float(OF)
            self.percentFuel = float(percentFuel)
            self.P = float(P)
            self.Pch = float(Pch)
            self.T = float(T)
            self.rho = float(rho)
            self.Cp = float(Cp)
            self.gamma = float(gamma)
            self.SoS = float(SoS)
            self.mu = float(mu)
            self.Pr = float(Pr)
            self.massFractions = dict(massFractions)
        except:
            print("WARNING! class Preburner failed to initialise!")
            print("         (Bad input data (missing, wrong type)")

    def __str__(self):
        try:
            massFracList = self.massFractions.items()
            massFracStrings = []
            for x in massFracList:
                appendMassFrac = str("\n" + str(x[0]) + ": " + str(x[1]))
                massFracStrings.append(appendMassFrac)
            massFractionString = "".join(map(str, massFracStrings))
            stringReturn = str("O/F ratio: " + str(self.OF) +
                               "\n % fuel:" + str(self.percentFuel) +
                               "\nPressure (bar): " + str(self.P) +
                               "\nCritical pressure (bar)" + str(self.Pch) +
                               "\nTemperature (Kelvin): " + str(self.T) +
                               "\nDensity (kg/m3): " + str(self.rho) +
                               "\nCp (kJ/kg-K): " + str(self.Cp) +
                               "\ngamma: " + str(self.gamma) +
                               "\nSpeed of sound (m/s): " + str(self.SoS) +
                               "\nViscosity (millipoise): " + str(self.mu) +
                               "\nPrandtl number: " + str(self.Pr) +
                               "\nMass fractions listed below:" + massFractionString)
            return stringReturn
        except:
            return "ERROR! Preburner string return not initialised!"

    def summary(self):
        print("O/F ratio =", self.OF, "| % fuel =", self.percentFuel)
        print("P (bar) =", self.P, "| T (K) =", self.T)
        TPRoundme = self.P / self.Pch
        TPR = round(TPRoundme, 3)
        print("Best TPR =", TPR, "| rho (kg/m3) =", self.rho)
        CpJoules = self.Cp * 1000

    def debug(self):
        self.OF = 4.0
        self.P = 450.0
        self.Pch = 180.0
        self.T = 1300.0
        self.rho = 45.0
        self.Cp = 10000.0
        self.gamma = 1.15
        self.SoS = 1200.0
        self.mu = 1.22
        self.Pr = 0.98
        self.massFractions = {"H2": 0.4, "O2": 0.6}

#NO SUBSONIC AREA RATIOS
def doParseMCC(filenameString):
    lines = []
    
    with open(filenameString) as cearun:
        for line in cearun:
            lines.append(line)
    
    lineIndicies = {}
    it = -1
    for line in lines:
        it += 1
        lineCopy = str(line)
        if lineCopy.lstrip().startswith("INJECTOR") == True:
            lineIndicies.update({it: "blockStart"})
        elif lineCopy.lstrip().startswith("Isp, M/SEC") == True:
            lineIndicies.update({it: "blockEnd"})
    
    #creating the blocks
    blockIndiciesList = list(lineIndicies.keys())
    blockIndicies = splitList(blockIndiciesList, 2)
    blocks = []
    for indicies in blockIndicies:
        blockLo = indicies[0]
        blockHi = indicies[1]
        blockHii = blockHi + 1
        blockRange = range(blockLo, blockHii)
        blockLines = []
        for index in blockRange:
            line = lines[index]
            lineCopy = str(line)
            if lineCopy.isspace() == True:
                pass
            else:
                blockLines.append(line)
        blocks.append(blockLines)

    #amountOfBlocks = len(blocks)
    #print("Number of blocks =", amountOfBlocks)
    #trim the blocks down
    trimmedBlocks = []
    rhoLines = []
    for block in blocks:
        trimmedBlock = []
        for line in block:
            lineCopy = str(line)
            if lineCopy.lstrip().startswith("INJECTOR") == True:
                trimmedBlock.append(line.rstrip())
            elif lineCopy.lstrip().startswith("P, BAR") == True:
                trimmedBlock.append(line.rstrip())
            elif lineCopy.lstrip().startswith("T, K") == True:
                trimmedBlock.append(line.rstrip())
            elif lineCopy.lstrip().startswith("RHO, KG/CU M") == True:
                rhoLines.append(line)
            elif lineCopy.lstrip().startswith("Cp, KJ/(KG)(K)") == True:
                trimmedBlock.append(line.rstrip())
            elif lineCopy.lstrip().startswith("GAMMAs") == True:
                trimmedBlock.append(line.rstrip())
            elif lineCopy.lstrip().startswith("SON VEL,M/SEC") == True:
                trimmedBlock.append(line.rstrip())
            elif lineCopy.lstrip().startswith("MACH NUMBER") == True:
                trimmedBlock.append(line.rstrip())
            elif lineCopy.lstrip().startswith("VISC,MILLIPOISE") == True:
                trimmedBlock.append(line.rstrip())
            elif lineCopy.lstrip().startswith("PRANDTL NUMBER") == True:
                trimmedBlock.append(line.rstrip())
            elif lineCopy.lstrip().startswith("WITH FROZEN REACTIONS") == True:
                trimmedBlock.append(line.rstrip())
            elif lineCopy.lstrip().startswith("Ae/At") == True:
                trimmedBlock.append(line.rstrip())
            elif lineCopy.lstrip().startswith("CSTAR, M/SEC") == True:
                trimmedBlock.append(line.rstrip())
            elif lineCopy.lstrip().startswith("Isp, M/SEC") == True:
                trimmedBlock.append(line.rstrip())
            else:
                pass
        trimmedBlocks.append(trimmedBlock)

    #trim stage 2 (removing frozen reactions and unneeded Cp under equilibrium reactions)
    trimtBlocks = trimmedBlocks
    trimmedBlocks = []
    for block in trimtBlocks:
        blockIndices = range(0, len(block), 1)
        trimmedBlock = []
        trimmedBlockIndices = list(blockIndices)
        for index in blockIndices:
            testLine = block[index]
            if testLine.lstrip().startswith("WITH FROZEN REACTIONS") == True:
                unusedCp = index - 2
                frozenCp = index + 1
                frozenPr = index + 2
                trimmedBlockIndices.remove(index)
                trimmedBlockIndices.remove(unusedCp)
                trimmedBlockIndices.remove(frozenCp)
                trimmedBlockIndices.remove(frozenPr)
            else:
                pass
        for index in trimmedBlockIndices:
            passLine = block[index]
            trimmedBlock.append(passLine)
        trimmedBlocks.append(trimmedBlock)

    arrays = []
    #print()
    #print("before transposition:")
    for block in trimmedBlocks:
        preparedBlock = parseBlock(block)
        array = np.asarray(preparedBlock)
        sherpa = np.shape(array)
        #print(sherpa)
        #array.transpose() no idea how the fuck I got away with transposing twice
        arrays.append(array)

    #print("after transposition:")
    arraysT = []
    for array in arrays:
        arrayT = array.transpose()
        sherbert = np.shape(arrayT)
        #print(sherbert)
        arraysT.append(arrayT)
    
    arraysTup = tuple(arraysT)
    stack = np.vstack(arraysTup)
    uniqueStack = np.unique(stack, axis=0)

    #briefly figure out rho
    rhoLine = rhoLines[0]
    rhoLstA = rhoLine.split(" ")
    rhoLstB = [x.strip() for x in rhoLstA]
    rhoLstC = list(filter(None, rhoLstB))
    rhoA = float(rhoLstC[7])
    rhoB = float(rhoLstC[8])
    rhoThroat = rhoA * 10**rhoB
    
    positions = []
    for line in uniqueStack:
        if (line[1] == "P, BAR"):
            pass
        else:
            pos = Position(line[0], line[1], line[2], line[3],
                           line[4], line[5], line[6], line[7],
                           line[8], line[9], line[10], line[11], rhoThroat)
            positions.append(pos)

    #sorting
    positions.sort(key=lambda x: x.AR)
    it = -1
    for x in positions:
        it +=1
        if x.position == "INJECTOR":
            injIndex = it
        elif x.position == "COMB END":
            combEndIndex = it
    insertCombEndHere = injIndex + 1
    positions.insert(insertCombEndHere, positions[combEndIndex])
    removeCombEndHere = combEndIndex + 1
    positions.pop(removeCombEndHere)
    positions[0].AR = positions[1].AR
    positions[0].Cstar = positions[1].Cstar
    return positions

def doParsePB(filenameString):
    lines = []
    
    with open(filenameString) as cearun:
        for line in cearun:
            lines.append(line)

    #get block start and end indices
    lineIndices = {}
    it = -1
    for line in lines:
        it += 1
        lineCopy = str(line)
        if lineCopy.lstrip().startswith("O/F=") == True:
            lineIndices.update({it: "blockStart"})
        elif lineCopy.lstrip().startswith("NOTE. WEIGHT FRACTION") == True:
            blockEndIt = it - 4
            lineIndices.update({blockEndIt: "blockEnd"})

    #split into blocks
    blockIndicesList = list(lineIndices.keys())
    blockIndices = splitList(blockIndicesList, 2)
    blocks = []
    for indices in blockIndices:
        blockLo = indices[0]
        blockHi = indices[1]
        blockHii = blockHi + 1
        blockRange = range(blockLo, blockHii)
        blockLines = []
        for index in blockRange:
            line = lines[index]
            lineCopy = str(line)
            if lineCopy.isspace() == True:
                pass
            else:
                blockLines.append(line)
        blocks.append(blockLines)
    
    strippedBlocks = []
    for block in blocks:
        strippedBlock = []
        for line in block:
            lineStripped = line.strip()
            strippedBlock.append(lineStripped)
        strippedBlocks.append(strippedBlock)

    #just for debugging
    #print("blocks stripped down")    
    #for x in strippedBlocks:
        #print()
        #it = -1
        #for y in x:
            #it += 1
            #print("strip index", it, "|", y)

    preburners = []
    #in this case, blocks are actually the configs practically speaking, so may as well just initialise the objects here
    for block in strippedBlocks:
        it = -1
        splitIndex = int()
        for line in block:
            it += 1
            if line.startswith("O/F=") == True:
                OFline = parseLine(line)
                OF = OFline[1]
                percentFuel = OFline[2].split(" ")[-1]
            elif line.startswith("P, BAR") == True:
                Pline = parseLine(line)
                P = Pline[1]
                Pch = Pline[2]
            elif line.startswith("T, K") == True:
                Tline = parseLine(line)
                T = Tline[1]
            elif line.startswith("RHO, KG/CU M") == True:
                rhoLine = parseLine(line)
                rhoLineSplit = rhoLine[1].split(" ")
                rho = float(rhoLineSplit[0]) * (10**float(rhoLineSplit[1]))
            elif (line.startswith("Cp, KJ/(KG)(K)") == True) and (it < 14):
                CpLine = parseLine(line)
                Cp = CpLine[1]
            elif line.startswith("GAMMAs") == True:
                gammaLine = parseLine(line)
                gamma = gammaLine[1]
            elif line.startswith("SON VEL,M/SEC") == True:
                SoSline = parseLine(line)
                SoS = SoSline[1]
            #elif line.startswith("MACH NUMBER") == True:
                #Mline = parseLine(line)
                #print(Mline)
            elif line.startswith("VISC,MILLIPOISE") == True:
                muLine = parseLine(line)
                mu = muLine[1]
            elif (line.startswith("PRANDTL NUMBER") == True) and (it < 24):
                PrLine = parseLine(line)
                Pr = PrLine[1]
            #elif line.startswith("CSTAR, M/SEC") == True:
                #CstarLine = parseLine(line)
                #print(CstarLine)
            elif line.startswith("MASS FRACTIONS") == True:
                splitIndex = it
        it = -1
        massFractions = {}
        #this is a really shockingly bad bit of code but it gets the job done
        for line in block:
            it += 1
            if it > splitIndex:
                splitLineA = line.split(" ")
                massFracCompound = splitLineA[0]
                splitLineA.pop(0)
                splitLineB = []
                for x in splitLineA:
                    try:
                        y = float(x)
                        splitLineB.append(y)
                    except:
                        pass
                massFracAmount = splitLineB[0]
                massFractions.update({massFracCompound: massFracAmount})
        preburner = Preburner(OF, percentFuel, P, Pch, T, rho, Cp, gamma, SoS, mu, Pr, massFractions)
        preburners.append(preburner)

    return preburners

def parseMCC(filenameStr):
    try:
        filenameString = str(filenameStr)
        return doParseMCC(filenameString)
    except:
        exceptString = str("ERROR! Failed ingesting file: " + str(filenameStr))
        print(exceptString)

def parsePB(filenameStr):
    try:
        filenameString = str(filenameStr)
        return doParsePB(filenameString)
    except:
        exceptString = str("ERROR! Failed ingesting file: " + str(filenameStr))
        print(exceptString)
