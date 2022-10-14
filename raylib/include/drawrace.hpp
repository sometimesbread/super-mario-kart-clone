void DrawHorizon(int height, int width, float angle, float fgTurnSpeed, float bgTurnSpeed, int texturewidth, unsigned int bgColorLookup[], int bgTexture[32][768], unsigned int fgColorLookup[], int fgTexture[32][1280])
{
    for (int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            DrawPixel(x, y, GetColor(bgColorLookup[bgTexture[y][abs(static_cast<int>(x + angle * bgTurnSpeed) % texturewidth)]]));
            DrawPixel(x, y, GetColor(fgColorLookup[fgTexture[y][abs(static_cast<int>(x + angle * fgTurnSpeed) % texturewidth)]]));
        }
    }
}

void DrawTrack(int screenHeight, int horizonHeight, float mapScale, int screenWidth, float a, float b, float h, float v, float xn, float yn, float aChange, float angleSin, float angleCos)
{
    int xPrime = 0;
    int yPrime = 0;
    for (int y = 0; y < screenHeight - horizonHeight; y++)
    { 
        //calculate depth
        float scl = aChange / screenHeight / (y+1);
        //float scl = 1;
        a = scl * angleCos * mapScale;
        b = scl * angleSin * mapScale;
    
        //calculate the "bases" of xprime and yprime so i dont murder the cpu
        float xPrimebase = a * (h - xn) + b * (y +  v - yn) + xn;
        float yPrimebase = -b * (h - xn) + a * (y + v - yn) + yn;

        for (int x = 0; x < screenWidth; x++)
        {
            xPrime = floor(xPrimebase + a * x);
            yPrime = floor(yPrimebase + -b * x);
    
            //check if the current pixel is in the map, and draw the map texture if it is. if it not, draw the repeating texture
            if(xPrime < 1024 && xPrime > 0 && yPrime > 0 && yPrime < 1024)
            {
                DrawPixel(x, y + horizonHeight, GetColor(mariocircuit1ColorLookup[mariocircuit1ColorList[xPrime][yPrime]]));
    
                //show collision map instead of color map (debug)
                //DrawPixel(x, y + horizonHeight, GetColor(mariocircuit1CollisionColorLookup[mariocircuit1CollisionColorList[128 - (int)floor(xPrime / 8)][128 - (int)floor(yPrime / 8)]]));
            }
            else
            {
                DrawPixel(x, y + horizonHeight, GetColor(outOfBoundsColorLookup[outOfBoundsColorList[abs(xPrime) % 8][abs(yPrime) % 8]]));
            }
        }
    }
}