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