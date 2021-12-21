#include <iostream>
#include <raylib.h>
#include <math.h>
#include <chrono>

#include "mariocircuit-1.h"
#include "outofbounds.h"
#include "bg_trees.h"
#include "bg_hills.h"
#include "mario_spritesheet.h"
#include "mariocircuit-1collision.h"

//#include make header file that only stores a 2d array of the image (using the color compression)
using namespace std;

int main(void)
{
    const float mode7tomapsize = 1.024;

    //mode 7 transformations
    int xPrime = 0;
    int yPrime = 0;

    float a = 1.0f;
    float b = 0.0f;
    float c = 0.0f;
    float d = 1.0f;

    float h = 315.5f; //yeah its weird but its perfectly centered on the starting line
    float v = 808.0f;

    float xn = 128.0f + h;
    float yn = 112.0f + v;

    float aChange = 8000;

    //time  
    float previousTime = 0.0f;
    float deltaTime = 0.0f;

    //kart physics

    float angle = 4.71239f;
    float forwardX = 0.0f;
    float forwardY = 0.0f;

    float velocity = 0.0f;
    float acceleration = 0.25f;
    float maxVelocity = 1.25f;
    float drag = 0.125f;

    float offroadVelocityMultiplier = 0.5f;

    float torque = 0.0f;
    float torqueAcceleration = 0.06f;
    float maxTorque = 0.08f;
    float torqueDrag = 0.015f;

    float drifting = false;

    //kart drawing

    enum KartState
    {
        LOOKBACK = 1,
        WIN = 2,
        LOSE = 3,
        SCALE0 = 4,
        SCALE1 = 5,
        SCALE2 = 6,
        SCALE3 = 7,
        SCALE4 = 8,
        SCALE5 = 9,
        SCALE6 = 10,
        SCALE7 = 11,
        SCALE8 = 12,
        SCALE9 = 13
    };

    int kartPosX = 128;
    int kartPosY = 0;
    int kartSize = 64;
    int halfKartSize = kartSize >> 1;

    KartState kartState = SCALE0;
    int kartAnimationFrame = 0;

    int spriteReversed = 0; //0 = not reversed, 1 = reversed
    float timeAboutToDrift = 0.0;

    //map
    float mapScaling = 0.5;

    //horizon
    float horizonHeight = 32;
    float hillTurnSpeed = -50; //multiplier for how fast the hills move (based on angle)
    float treeTurnSpeed = -90; //multiplier for how fast the trees move (based on angle)
    int hillTextureWidth = 768;
    int treeTextureWidth = 1280;

    //initialization
    const int screenWidth = 256;
    const int screenHeight = 224;

    InitWindow(screenWidth, screenHeight, "Super Mario Kart (raylib)");

    SetTargetFPS(60); 

    //game loop
    while (!WindowShouldClose())
    {
        //update up/down velocity
        if(IsKeyDown(KEY_W)) { velocity += acceleration; }
        if(IsKeyDown(KEY_S)) { velocity -= acceleration; }
        
        //update left/right turning
        if(IsKeyDown(KEY_A)) { torque += (torqueAcceleration * std::min(velocity, 1.0f)); }
        if(IsKeyDown(KEY_D)) { torque -= (torqueAcceleration * std::min(velocity, 1.0f)); }

        //clamp velocity and make it move toward zero
        if(velocity < 0) { velocity += drag; }
        if(velocity > 0) { velocity -= drag; }  
        if(velocity > maxVelocity) { velocity = maxVelocity; }
        if(velocity < -maxVelocity) { velocity = -maxVelocity; }
        
        //clamp turning force and make it move toward zero
        if(torque > 0) { torque -= torqueDrag; }
        if(torque < 0) { torque += torqueDrag; }
        if(torque < 0.02 && torque > -0.02) { torque = 0.0; } 
        if(torque > maxTorque) { torque = maxTorque; }
        if(torque < -maxTorque) { torque = -maxTorque; }

        //get the forward vector and sideways vector
        forwardX = sin(angle);
        forwardY = cos(angle);

        //collision
        //(0,0,0) = track, (255,255,255) = offroad, (127,127,127) = endline, (90,90,90) = coin, (100,100,100) = wall/bounceback, (200,200,200) = block
        
        //check if on track
        if(mariocircuit1CollisionColorLookup[mariocircuit1ColorList[(int)floor(h * mode7tomapsize)][(int)floor(v * mode7tomapsize)]] == 0xffffffff) { velocity *= offroadVelocityMultiplier; cout << "Offroad" << endl; }

        //update mode 7 variables
        v -= forwardY * velocity;
        h -= forwardX * velocity;
        yn -= forwardY * velocity;
        xn -= forwardX * velocity;
        angle += torque;

        BeginDrawing();
            //draw horizon/bg
            for (int y = 0; y < horizonHeight; y++)
            {
                for(int x = 0; x < 256; x++)
                {
                    DrawPixel(x, y, GetColor(hillsColorLookup[hillsColorList[y][abs(static_cast <int>(x + angle * hillTurnSpeed) % hillTextureWidth)]]));
                    DrawPixel(x, y, GetColor(treesColorLookup[treesColorList[y][abs(static_cast <int>(x + angle * treeTurnSpeed) % hillTextureWidth)]]));
                }
            }

            //draw track
            for (int y = 0; y < 224 - horizonHeight; y++)
            {
                float scl = aChange / 224 / (y+1);
                a = scl * forwardY * mapScaling; //using forwardY because it is already calculated and calculating cos and sin again is a waste
                b = scl * forwardX * mapScaling;

                float xPrimebase = a * (h - xn) + b * (y +  v - yn) + xn;
                float yPrimebase = -b * (h - xn) + a * (y+ v - yn) + yn;

                for (int x = 0; x < 256; x++)
                {
                    xPrime = floor(xPrimebase + a * x);
                    yPrime = floor(yPrimebase + -b * x);
                    if(xPrime < 1024 && xPrime > 0 && yPrime > 0 && yPrime < 1024)
                    {
                        DrawPixel(x, y + horizonHeight, GetColor(mariocircuit1ColorLookup[mariocircuit1ColorList[xPrime][yPrime]]));
                        //DrawPixel(x, y + horizonHeight, GetColor(mariocircuit1CollisionColorLookup[mariocircuit1CollisionColorList[(int)floor(xPrime / 8)][(int)floor(yPrime / 8)]]));
                    }
                    else
                    {
                        DrawPixel(x, y + horizonHeight, GetColor(outOfBoundsColorLookup[outOfBoundsColorList[abs(xPrime % 8)][abs(yPrime % 8)]]));
                    }
                }
            }

        //draw kart
        if(torque < 0.01) { kartAnimationFrame = 0; spriteReversed = 0; }
        if(torque > -0.01) { kartAnimationFrame = 0; spriteReversed = 0; }

        if(torque > 0.01) { kartAnimationFrame = 1; spriteReversed = 1; }
        if(torque < -0.01) { kartAnimationFrame = 1; spriteReversed = 0; }

        if(torque > 0.05) { kartAnimationFrame = 2; spriteReversed = 1; timeAboutToDrift += deltaTime; }
        if(torque < -0.05) { kartAnimationFrame = 2; spriteReversed = 0; timeAboutToDrift += deltaTime; }

        for (int x = 0; x < kartSize; x++)
        {
            for (int y = 0; y < kartSize; y++)
            {
                DrawPixel((kartPosX - halfKartSize) + x, (kartPosY - halfKartSize) + y + (112 + halfKartSize), GetColor(marioSpritesheetColorLookup[marioSpritesheetColorList[kartState * kartSize + y][kartAnimationFrame * kartSize + x + (spriteReversed * kartSize * 11)]]));
            }
        }
            

        EndDrawing();

        deltaTime = (chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() / 1000000000 - previousTime);
        previousTime = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() / 1000000000;
    }

    //deinitialization
    CloseWindow();
    return 0;
}

/*
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
*/