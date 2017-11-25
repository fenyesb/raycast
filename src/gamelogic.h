// Fényes Balázs
// homepage: fenyesb.github.io
// repository: github.com/fenyesb/raycast

#ifndef _GAMELOGIC_H
#define _GAMELOGIC_H

#include <stdbool.h>
#include <SDL.h>

#include "defines.h"
#include "geometry.h"

void GameInit();
void GameStep();
extern bool quit;
void RotatePlayerOnKey(SDLKey key, double direction);
void MovePlayerOnKey(SDLKey key,double direction);
extern int time_remaining;
extern bool redraw_request;
extern char name[NAME_MAX_LEN];
extern bool isplaying;

#endif // _GAMELOGIC_H
