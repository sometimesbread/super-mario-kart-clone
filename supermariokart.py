import pygame
import os
import time
import datetime
import math
from math import sin
from math import cos
from pygame import display
from pygame import mixer

#initialgiasdization

pygame.init()
screen = pygame.display.set_mode([256, 224])
pygame.display.set_caption("Super Mario Kart")

#images

#mariocircuit1 track
backgroundImage = pygame.image.load("mariocircuit-1.png").convert()
backgroundSurface = pygame.Surface((1024, 1024))
backgroundSurface.blit(backgroundImage, (0, 0))

#repeating texture
repeatingTexture = pygame.image.load("mariocircuit_oob.png").convert()
repeatingSurface = pygame.Surface((8, 8))
repeatingSurface.blit(repeatingTexture, (0,0))

#trees
treeTexture = pygame.image.load("mariocircuit_bg_trees.png").convert()
treeSurface = pygame.Surface((1280, 32))
treeSurface.blit(treeTexture, (0, 0))

#hills
hillTexture = pygame.image.load("mariocircuit_bg_hills.png").convert()
hillSurface = pygame.Surface((768, 32))
hillSurface.blit(hillTexture, (0,0))

#sounds
mixer.music.load("mariocircuitmusic_start.mp3")
mixer.music.play()

#Mode 7 transformations
xPrime = 0
yPrime = 0

a = 1.0
b = 0.0
c = 0.0
d = 1.0

xn = 128
yn = 112

h = 0.0
v = 0.0

aChange = 13000

#time
previousTime = 0.0
deltaTime = 0.0

#input
wKey = False
aKey = False
sKey = False
dKey = False

#kart physics

angle = 0.0
forwardX = 0.0
forwardY = 0.0

velocity = 0.0
acceleration = 1
maxVelocity = 5
drag = 0.5

torque = 0.0
torqueAcceleration = 0.04
maxTorque = 0.1
torqueDrag = 0.02

#map
mapScaling = 0.5

#horizon
horizonHeight = 32
hillTurnSpeed = -50 #multiplier for how fast the hills move (based on angle)
treeTurnSpeed = -90 #multiplier for how fast the trees move (based on angle)
hillTextureWidth = 768
treeTextureWidth = 1280

def calculateVelocity():
    global velocity
    if(velocity > 0):
        velocity -= drag
    if(velocity < 0):
        velocity += drag
    if(velocity > maxVelocity):
        velocity = maxVelocity
    if(velocity < -maxVelocity):
        velocity = -maxVelocity

def calculateTorque():
    global torque
    if(torque > 0):
        torque -= torqueDrag
    if(torque < 0):
        torque += torqueDrag
    if(torque < 0.02 and torque > -0.02):
        torque = 0.0 
    if(torque > maxTorque):
        torque = maxTorque
    if(torque < -maxTorque):
        torque = -maxTorque
    
startTime = time.time_ns() / 1000000000
running = True
while running:
    xPrime = 0
    yPrime = 0
    deltaTime = (time.time_ns() / 1000000000 - previousTime)
    if(time.time() / 1000000000 - startTime > 36.7):
        mixer.music.stop()
        mixer.music.unload()
        mixer.music.load("mariocircuitmusic_loop.mp3")
        mixer.music.play()
        startTime = time.time()
        
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_w:
                wKey = True
            if event.key == pygame.K_a:
                aKey = True
            if event.key == pygame.K_s:
                sKey = True
            if event.key == pygame.K_d:
                dKey = True
        if event.type == pygame.KEYUP:
            if event.key == pygame.K_w:
                wKey = False
            if event.key == pygame.K_a:
                aKey = False
            if event.key == pygame.K_s:
                sKey = False
            if event.key == pygame.K_d:
                dKey = False

    screen.fill((250, 250, 250))

    #movement
    if wKey:
        velocity += acceleration
    if sKey:
        velocity -= acceleration
        
    if aKey:
        torque += torqueAcceleration
    if dKey:
        torque -= torqueAcceleration
    calculateVelocity()
    calculateTorque()
    v -= forwardY * velocity
    h -= forwardX * velocity
    yn -= forwardY * velocity
    xn -= forwardX * velocity
    angle += torque

    forwardX = sin(angle)
    forwardY = cos(angle)

    for y in range(horizonHeight):
        for x in range(256):
            screen.set_at((x, y), hillSurface.get_at((int(x + angle * hillTurnSpeed) % hillTextureWidth, y)))
            if(treeTexture.get_at((int(x + angle * treeTurnSpeed) % hillTextureWidth, y)) != ((0,0,0))):
                screen.set_at((x, y), treeTexture.get_at((int(x + angle * treeTurnSpeed) % hillTextureWidth, y)))
            

    for y in range(224 - horizonHeight):
        scl = aChange / (y+1) / 224
        a = scl * cos(angle) * mapScaling
        b = scl * sin(angle) * mapScaling
        c = scl * -sin(angle) * mapScaling
        d = scl * cos(angle) * mapScaling
        #a = 3 * cos(angle)
        #d = 3 * cos(angle)
        for x in range(256):
            xPrime = math.floor(a * (x + h - xn) + b * (y + v - yn) + xn)
            yPrime = math.floor(c * (x + h - xn) + d * (y + v - yn) + yn)
            try:               
                screen.set_at((x, y + horizonHeight), backgroundSurface.get_at((xPrime, yPrime)))
            except:
                screen.set_at((x, y + horizonHeight), repeatingSurface.get_at((xPrime % 8, yPrime % 8)))
    
    display.update()
    previousTime = time.time_ns() / 1000000000

pygame.quit()