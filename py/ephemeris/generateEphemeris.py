import sys

def getEphem(body):
    ephemA = []
    filename = f"./EpochJ2000/{body}.txt"
    defname = f"{body}"
    DEFNAME = defname.upper()

    physicalData = {}
    with open(filename) as file:
        startCopying = False
        for line in file:
            strLine = str(line).lstrip("\n")

            #todo extract physicalData

            if "$$SOE" in strLine:
                startCopying = True
            if startCopying == True:
                ephemA.append(strLine)
                if "$$EOE" in strLine:
                    startCopying = False
    ephemA.pop(0)
    ephemA.pop(0)
    ephemA.pop(-1)
    ephemB = []
    for x in ephemA:
        l = x.split(" ")
        ephemB.append(l)
    ephemA = ephemB

    #EC eccentricity
    #IN inclination deg
    #OM longitude of ascending node deg
    #W argument of periapsis deg
    #MA mean anomaly at epoch deg
    #A semimajor axis km
    #QR periapsis km
    #AD apoapsis km

    ecc = ephemA[0][2]
    inc = ephemA[0][-1] #or[6]
    LAN = ephemA[1][2]
    aPe = ephemA[1][5]
    MAE = ephemA[2][5]
    TAE = ephemA[2][-1]
    SMA = ephemA[-1][3]
    inc = inc.strip("\n")
    TAE = TAE.strip("\n")
    print(f"#ifdef {DEFNAME}")
    print(f"#define {DEFNAME}_ECC {ecc}")
    print(f"#define {DEFNAME}_INC {inc}")
    print(f"#define {DEFNAME}_LAN {LAN}")
    print(f"#define {DEFNAME}_APE {aPe}")
    print(f"#define {DEFNAME}_MAE {MAE}")
    print(f"#define {DEFNAME}_TAE {TAE}")
    print(f"#define {DEFNAME}_SMA {SMA}")
    print(f"#endif")
    print()

#print()
#print("typedef struct {\ndouble ecc;\ndouble incDeg;\ndouble LANdeg;\ndouble aPeDeg;\ndouble MAEdeg;\ndouble TAEdeg;\ndouble SMA;\n} Ephemeris;\n")
#for x in bodies:
#    defname = f"{x}_"
#    DEFNAME = defname.upper()
#    print(f"Ephemeris {x} = (Ephemeris){{{DEFNAME}ECC, {DEFNAME}INC, {DEFNAME}LAN, {DEFNAME}APE, {DEFNAME}MAE, {DEFNAME}TAE, {DEFNAME}SMA}};")

bodies = ["Mercury",
          "Venus",
          "Earth", "Luna",
          "Mars", "Phobos", "Deimos",
          "Ceres",
          "Jupiter", "Io", "Europa", "Ganymede", "Callisto",
          "Saturn", "Mimas", "Enceladus", "Tethys", "Dione", "Rhea", "Titan", "Iapetus",
          "Uranus", "Miranda", "Ariel", "Umbriel", "Titania", "Oberon",
          "Neptune", "Proteus", "Triton", "Nereid"]

o = sys.stdout
with open("ephemeris.h", "w") as f:
    sys.stdout = f
    for x in bodies:
        getEphem(x)
sys.stdout = o
