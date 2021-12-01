import pygame
import time
from math import floor, sin, cos
from enum import Enum
from pygame import display, mixer
import multiprocessing

#initialgiadization

pygame.init()
screen = pygame.display.set_mode([256, 224])
pygame.display.set_caption("Super Mario Kart")

#images

#mariocircuit1 track
backgroundImage = pygame.image.load("mariocircuit-1.png").convert()
backgroundSurface = pygame.Surface((1024, 1024))
backgroundSurface.blit(backgroundImage, (0, 0))
backgroundTextureColorList = []

#"optimization" this turns the image into a massive list, because looking up a list value is faster than looking up a texture
tempList = []
for y in range(1024):
    for x in range(1024):
        tempList.append(backgroundSurface.get_at((x,y)))
    backgroundTextureColorList.append(tempList)
    tempList = []

#collisionmap
#(0,0,0) = track, (255,255,255) = offroad, (127,127,127) = endline, (90,90,90) = coin


#repeating texture
repeatingTexture = pygame.image.load("mariocircuit_oob.png").convert()
repeatingSurface = pygame.Surface((8, 8))
repeatingSurface.blit(repeatingTexture, (0,0))
repeatingTextureColorList = []

tempList = []
for y in range(8):
    for x in range(8):
        tempList.append(repeatingSurface.get_at((x,y)))
    repeatingTextureColorList.append(tempList)
    tempList = []
print(len(repeatingTextureColorList[0]))

#trees
treeTexture = pygame.image.load("mariocircuit_bg_trees.png").convert()
treeSurface = pygame.Surface((1280, 32))
treeSurface.blit(treeTexture, (0, 0))
treeTextureColorList = []

tempList = []
for y in range(32):
    for x in range(1280):
        tempList.append(treeSurface.get_at((x,y)))
    treeTextureColorList.append(tempList)
    tempList = []

#hills
hillTexture = pygame.image.load("mariocircuit_bg_hills.png").convert()
hillSurface = pygame.Surface((768, 32))
hillSurface.blit(hillTexture, (0,0))
hillTextureColorList = []

tempList = []
for y in range(32):
    for x in range(768):
        tempList.append(hillSurface.get_at((x,y)))
    hillTextureColorList.append(tempList)
    tempList = []

#mario sprites
marioTexture = pygame.image.load("mario_spritesheet.png").convert()
marioSpritesSurface = pygame.Surface((1408, 704))
marioSpritesSurface.blit(marioTexture, (0,0))
marioTextureColorList = []

tempList = []
for y in range(704):
    for x in range(1408):
        tempList.append(marioSpritesSurface.get_at((x,y)))
    marioTextureColorList.append(tempList)
    tempList = []

#sounds
mixer.music.load("mariocircuitmusic_start.ogg")
mixer.music.play(0)

#Mode 7 transformations
xPrime = 0
yPrime = 0

a = 1.0
b = 0.0
c = 0.0
d = 1.0

h = 792.1 #yeah its weird but its perfectly centered on the starting line
v = 470.0

xn = 128 + h
yn = 112 + v

aChange = 8000

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
maxTorque = 0.13
torqueDrag = 0.02

drifting = False

#kart drawing

class KartState(Enum):
    LOOKBACK = 1
    WIN = 2
    LOSE = 3
    SCALE0 = 4
    SCALE1 = 5
    SCALE2 = 6
    SCALE3 = 7
    SCALE4 = 8
    SCALE5 = 9
    SCALE6 = 10
    SCALE7 = 11
    SCALE8 = 12
    SCALE9 = 13


kartPosX = 0
kartPosY = 0
kartSize = 64

kartState = KartState.SCALE0
kartAnimationFrame = 0

spriteReversed = 0 # 0 = not reversed, 1 = reversed
timeAboutToDrift = 0.0

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

def drawToScreen(destx, desty, samplex, sampley, colorList):
    screen.set_at((destx, desty), colorList[sampley][samplex])

numCores = multiprocessing.cpu_count()

startTime = time.time_ns() / 1000000000
isLoop = False
running = True
while running:
    xPrime = 0
    yPrime = 0
    
    if(time.time_ns() / 1000000000 - startTime > 36.53 and not isLoop):
        mixer.music.stop()
        mixer.music.unload()
        mixer.music.load("mariocircuitmusic_loop.ogg")
        mixer.music.play(-1)
        isLoop = True
        
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

    #movement
    if wKey:
        velocity += acceleration
    if sKey:
        velocity -= acceleration
        
    if aKey:
        torque += torqueAcceleration * min(velocity, 1)
    if dKey:
        torque -= torqueAcceleration * min(velocity, 1)
    calculateVelocity()
    calculateTorque()

    forwardX = sin(angle)
    forwardY = cos(angle)

    v -= forwardY * velocity
    h -= forwardX * velocity
    yn -= forwardY * velocity
    xn -= forwardX * velocity
    angle += torque

    #draw horizon

    for y in range(horizonHeight):
        for x in range(256):
            drawToScreen(x, y, int(x + angle * hillTurnSpeed) % hillTextureWidth, y, hillTextureColorList)
            if(treeTexture.get_at((int(x + angle * treeTurnSpeed) % hillTextureWidth, y)) != ((0,0,0))):
                drawToScreen(x, y, int(x + angle * treeTurnSpeed) % hillTextureWidth, y, treeTextureColorList)
            
    #draw track

    #probably do something like this https://www.geeksforgeeks.org/multiprocessing-python-set-1/ because this code sucks

    for y in range(224 - horizonHeight):
        scl = aChange / 224 / (y+1)
        a = scl * forwardY * mapScaling #using forwardY because it is already calculated and calculating cos and sin again is a waste
        b = scl * forwardX * mapScaling

        xPrimebase = a * (h - xn) + b * (y +  v - yn) + xn
        yPrimebase = -b * (h - xn) + a * (y+ v - yn) + yn
        
        #inputs = [a, b]

        #processed_list = Parallel(n_jobs=numCores)(delayed(modeSevenTransformation)(i, inputs) for i in inputs)

        for x in range(256):
            xPrime = floor(xPrimebase + a * x)
            yPrime = floor(yPrimebase + -b * x)
            if(xPrime < 1024 and xPrime > 0 and yPrime > 0 and yPrime < 1024):
                drawToScreen(x, y + horizonHeight, xPrime, yPrime, backgroundTextureColorList)
            else:
                drawToScreen(x, y + horizonHeight, xPrime % 8, yPrime % 8, repeatingTextureColorList)

    #draw kart

    if(torque < 0.01):
        kartAnimationFrame = 0
        spriteReversed = 0
    
    if(torque > -0.01):
        kartAnimationFrame = 0
        spriteReversed = 0

    if(torque > 0.01):
        kartAnimationFrame = 1
        spriteReversed = 1

    if(torque < -0.01):
        kartAnimationFrame = 1
        spriteReversed = 0

    if(torque > 0.05):
        kartAnimationFrame = 2
        spriteReversed = 1
        timeAboutToDrift += deltaTime
    
    if(torque < -0.05):
        kartAnimationFrame = 2
        spriteReversed = 0
        timeAboutToDrift += deltaTime

    #if(torque > 0.08):
    #    kartAnimationFrame = 3
    #    spriteReversed = 1
    
    #if(torque < -0.08):
    #    kartAnimationFrame = 3
    #    spriteReversed = 0

    #if(torque > 0.15):
    #    kartAnimationFrame = 4
    #    spriteReversed = 1
    
    #if(torque < -0.15):
    #    kartAnimationFrame = 4
    #    spriteReversed = 0

    kartPosX = 128 - kartSize
    kartPosY = 128 - kartSize


    for x in range(kartSize):
        for y in range(kartSize):
            #if marioSpritesSurface.get_at(((kartAnimationFrame * kartSize + x + spriteReversed * (kartSize * 11)), kartState.value * kartSize + y)) !=((0,0,0)):
            if marioTextureColorList[kartState.value * kartSize + y][kartAnimationFrame * kartSize + x + spriteReversed * (kartSize * 11)] != (0,0,0):
                drawToScreen((kartPosX + x + floor(kartSize / 2)), kartPosY + y + floor(kartSize / 2), (kartAnimationFrame * kartSize + x + spriteReversed * (kartSize * 11)), kartState.value * kartSize + y, marioTextureColorList)

    display.update()
    deltaTime = (time.time_ns() / 1000000000 - previousTime)
    print(1/deltaTime)
    previousTime = time.time_ns() / 1000000000

pygame.quit()