#pragma once

void UpdateCheckpoint(unsigned int currentCheckpointColor, int &currentCheckpoint, int &currentLap)
{
    switch(currentCheckpointColor)
    {
        case 0xa0a0aff:
        if(currentCheckpoint == 5 || currentCheckpoint == 1) { currentLap++; currentCheckpoint = 0; }
        break;

        case 0x141414ff:
        if(currentCheckpoint == 0 || currentCheckpoint == 2) { currentCheckpoint = 1; }
        break;

        case 0x1e1e1eff:
        if(currentCheckpoint == 1 || currentCheckpoint == 3) { currentCheckpoint = 2; }
        break;

        case 0x282828ff:
        if(currentCheckpoint == 2 || currentCheckpoint == 4) { currentCheckpoint = 3; }
        break;

        case 0x323232ff:
        if(currentCheckpoint == 3 || currentCheckpoint == 5) { currentCheckpoint = 4; }
        break;

        case 0x3c3c3cff:
        if(currentCheckpoint == 0) { currentLap--; currentCheckpoint = 5; } if(currentCheckpoint == 4) { currentCheckpoint = 5; }
        break;

    }
}