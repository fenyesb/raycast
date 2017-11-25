// Fényes Balázs
// homepage: fenyesb.github.io
// repository: github.com/fenyesb/raycast

#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>

#include "render.h"
#include "gamelogic.h"

int main(int argc, char *argv[])
{
    RenderInit();
    GameInit();
    int nextFrame = SDL_GetTicks(), delay, target = 1000 / 60;

    while(1)
    {
        GameStep();
        if(quit)
            break;

        if(nextFrame > SDL_GetTicks())
            RenderFrame();

        delay = nextFrame - SDL_GetTicks();
        if(delay > 0)
            SDL_Delay(delay);

        nextFrame += target;
    }

    SDL_Quit();
    return 0;
}
