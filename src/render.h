// Fényes Balázs
// homepage: fenyesb.github.io
// repository: github.com/fenyesb/raycast

#ifndef _RENDER_H
#define _RENDER_H

#include <SDL.h>
#include <stdbool.h>
#include <SDL_ttf.h>

#include "room.h"

extern SDL_Surface* screen;
extern int coin_pickup_effect, bonus_time_effect;
void RenderInit();
void drawInt(int d, int x, int y, SDL_Color sdlc);
double getViewAngle(int col);
bool shouldRender(Room room);
void RenderFrame();
void drawMinimap();
int getFogColor(Point pt);
void drawLine(Line l, int color);
void drawString(char* str, int x, int y, SDL_Color sdlc, TTF_Font* font);

#endif // _RENDER_H
