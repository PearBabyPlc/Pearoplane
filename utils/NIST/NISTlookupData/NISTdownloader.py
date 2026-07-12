import wget
from time import sleep

# this is just for oxygen, methane/hydrogen download process is fairly similar/
# Also don't run this program please, it probably counts as an attempted DoS attack lol.
# I've already downloaded a bunch of the data, no need to strain the NIST servers all over again.

oxrange = range(1, 601, 1)
ox1 = r"https://webbook.nist.gov/cgi/fluid.cgi?Action=Data&Wide=on&ID=C7782447&Type=IsoBar&Digits=5&P="
ox2 = r"&THigh=600&TLow=&TInc=&RefState=DEF&TUnit=K&PUnit=bar&DUnit=kg%2Fm3&HUnit=kJ%2Fkg&WUnit=m%2Fs&VisUnit=Pa*s&STUnit=N%2Fm"

for x in oxrange:
    url = str(ox1 + str(x) + ox2)
    filename = str("O2_" + str(x) + "b.txt")
    wget.download(url, filename)
    sleep(1)
