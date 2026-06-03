import math

gamDiatomic = 1.4
CpiAir = 1005
Theta = 3055 + 5/9

def debug():
    print("egg")
    pass

### ISENTROPIC RELATIONS ###

def gam(T):
    a = math.e**(Theta / T) / (math.e**(Theta / T) - 1)**2
    b = 1 + (gamDiatomic - 1) * ((Theta / T)**2 * a)
    return 1 + ((gamDiatomic - 1) / b)

def Cp(T):
    a = math.e**(Theta / T) / (math.e**(Theta / T) - 1)**2
    b = 1 + ((gamDiatomic - 1) / gamDiatomic) * ((Theta / T)**2 * a)
    return CpiAir * b

def SoS(T):
    gamma = gam(T)
    return math.sqrt(287 * gamma * T)

def Tt_T(gam, M):
    return 1 + ((gam - 1) / 2) * M**2

def Pt_P(gam, M):
    return Tt_T(gam, M)**(gam / (gam - 1))

def T_Tt(gam, M):
    return 1 / Tt_T(gam, M)

def P_Pt(gam, M):
    return 1 / Pt_P(gam, M)

def A_Astar(gam, M):
    a = (gam + 1) / (2 * (gam - 1))
    b = ((gam + 1) / 2)**(-a)
    return b * ((Tt_T(gam, M)**a) / M)

def getRho(P, T):
    return P / (287 * T)

### SHOCK RELATIONS ###

def MachAngle(M):
    return math.asin(1 / M)

def cotaOblique(s, gam, M):
    a = ((gam + 1) * M**2) / (2 * (M**2 * math.sin(s)**2 - 1))
    return math.tan(s) * (a - 1)

def acot(x):
    return math.atan(1 / x)

def aOblique(s, gam, M):
    cota1 = ((gam + 1) * M**2) / (2 * (M**2 * math.sin(s)**2 - 1))
    cota = math.tan(s) * (cota1 - 1)
    return math.atan(1 / cota)

def MsinsaOblique(s, gam, M):
    b = (gam - 1) * M**2 * math.sin(s)**2 + 2
    c = 2 * gam * M**2 * math.sin(s)**2 - (gam - 1)
    return b / c

def MoutOblique(s, a, gam, M):
    b = (gam - 1) * M**2 * math.sin(s)**2 + 2
    c = 2 * gam * M**2 * math.sin(s)**2 - (gam - 1)
    return math.sqrt((b / c) / math.sin(s - a)**2)

def TrOblique(s, gam, M):
    a = 2 * gam * M**2 * math.sin(s)**2 - (gam - 1)
    b = (gam - 1) * M**2 * math.sin(s)**2 + 2
    c = (gam + 1)**2 * M**2 * math.sin(s)**2
    return (a * b) / c

def PrOblique(s, gam, M):
    a = 2 * gam * M**2 * math.sin(s)**2 - (gam - 1)
    return a / (gam + 1)

def rhorOblique(s, gam, M):
    a = (gam + 1) * M**2 * math.sin(s)**2
    b = (gam - 1) * M**2 * math.sin(s)**2 + 2
    return a / b

def PtrOblique(s, gam, M):
    a = (gam + 1) * M**2 * math.sin(s)**2
    b = (gam - 1) * M**2 * math.sin(s)**2 + 2
    c = 2 * gam * M**2 * math.sin(s)**2 - (gam - 1)
    d = (a / b)**(gam / (gam - 1))
    ee = ((gam + 1) / c)**(1 / (gam - 1))
    return d * ee

def aLimit(gam, M):
    b = 4 / (3 * math.sqrt(3) * (gam + 1))
    return b * ((M**2 - 1)**(3 / 2) / M**2)

def PrNormal(gam, M):
    return (2 * gam * M**2 - (gam - 1)) / (gam + 1) 

def PtrNormal(gam, M):
    a = ((gam + 1) * M**2) / ((gam - 1) * M**2 + 2)
    b = (gam + 1) / (2 * gam * M**2 - (gam - 1))
    c = gam / (gam - 1)
    d = 1 / (gam - 1) 
    return a**c * b**d

def TrNormal(gam, M):
    a = 2 * gam * M**2 - (gam - 1)
    b = (gam - 1) * M**2 + 2 
    return (a * b) / ((gam + 1)**2 * M**2)

def rhorNormal(gam, M):
    b = (gam - 1) * M**2 + 2 
    return ((gam + 1) * M**2) / b

def MoutNormal(gam, M):
    a = (gam - 1) * M**2 + 2
    b = 2 * gam * M**2 - (gam - 1)
    return math.sqrt(a / b)

### RAYLEIGH ###

def Ttstar_Tt(gam, M):
    a = (1 + gam * M**2)**2
    b = 2 * (gam + 1) * M**2 * Tt_T(gam, M)
    return a / b

def Tstar_T(gam, M):
    a = (1 + gam * M**2)**2
    return a / ((gam + 1)**2 * M**2)

def Ptstar_Pt(gam, M):
    a = 1 + gam * M**2
    b = (2 * Tt_T(gam, M)) / (gam + 1)
    return a / ((gam + 1) * b**(gam / (gam - 1)))

def Pstar_P(gam, M):
    return (1 + gam * M**2) / (gam + 1)

### PRANDTL-MEYER ###

def PMangle(gam, M):
    a = math.sqrt((gam + 1) / (gam - 1))
    b = math.sqrt(((gam - 1) / (gam + 1)) * (M**2 - 1))
    c = math.atan(math.sqrt(M**2 - 1))
    return a * math.atan(b) - c

