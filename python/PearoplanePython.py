import pygame
import math
import sys
from random import randint

pygame.init()
W, H = 1200, 800
screen = pygame.display.set_mode((W, H))
clock = pygame.time.Clock()

CENTER = (W // 2, H // 2)

screen.fill((0, 0, 0))

def randomColour():
    return (randint(0, 255), randint(0, 255), randint(0, 255))

def point(p):
    s = 1;
    pygame.draw.rect(screen, randomColour(), [p[0] - s/2, p[1] - s/2, 4, 4])

def to_screen(p):
    return (((p[0] + 1)/2 * W), ((1-(p[1] + 1) / 2) * H))

def project(p):
    return ((p[0] / p[2]), (p[1] / p[2]))

def translate_z(p, dz):
    return (p[0], p[1], (p[2] + dz))

def rotate_xz(p, angle):
    c = math.cos(angle)
    s = math.sin(angle)
    return ((p[0] * c - p[2] * s),
            p[1],
            (p[0] * s + p[2] * c))

def rotate_xy(p, angle):
    c = math.cos(angle)
    s = math.sin(angle)
    return ((p[0] * c - p[1] * s),
            (p[0] * s + p[1] * c),
             p[2])

def rotate_yz(p, angle):
    c = math.cos(angle)
    s = math.sin(angle)
    return (p[0],
            (p[1] * c - p[2] * s),
            (p[1] * s + p[2] * c))

t = 1

objLines = []
with open("teapot.obj") as f:
    for fl in f:
        flString = str(fl)
        if flString.startswith("v "):
            objLines.append(flString.rstrip())

verts = []
shrink = 0.15
for x in objLines:
    objListTemp = x.split(" ")
    objList = [x for x in objListTemp if not x.isalpha()]
    print(objList)
    objX = float(objList[0]) * shrink
    objY = (float(objList[1]) * shrink * 1.5) - 0.3
    objZ = float(objList[2]) * shrink
    verts.append((objX, objY, objZ))

#verts = [(0.25, 0.25, 0.25),
#         (-0.25, 0.25, 0.25),
#         (-0.25, -0.25, 0.25),
#         (0.25, -0.25, 0.25),
#         (0.25, 0.25, -0.25),
#         (-0.25, 0.25, -0.25),
#         (-0.25, -0.25, -0.25),
#         (0.25, -0.25, -0.25)]

#lines = [(0, 1), (1, 2), (2, 3), (3, 0),
#         (4, 5), (5, 6), (6, 7), (7, 4),
#         (0, 4), (1, 5), (2, 6), (3, 7)]

ang = 0.0

while True:
    ang += math.pi * (1 / 120)
    for e in pygame.event.get():
        if e.type == pygame.QUIT:
            pygame.quit()
            sys.exit()

    screen.fill((0, 0, 0))

    #for l in lines:
    #    v1 = verts[l[0]]
    #    v2 = verts[l[1]]
    #    p1 = to_screen(project(translate_z(rotate_xz(v1, ang), t)))
    #    p2 = to_screen(project(translate_z(rotate_xz(v2, ang), t)))
    #    pygame.draw.line(screen, (255, 0, 255), (p1[0], p1[1]), (p2[0], p2[1]))

    for v in verts:
        p = rotate_xz(v, float(ang))
        p = rotate_xy(p, float(ang * 0.5))
        p = rotate_yz(p, float(ang * 1.5))
        p = translate_z(p, t)
        p = project(p)
        point(to_screen(p))

    pygame.display.flip()
    clock.tick(60)
