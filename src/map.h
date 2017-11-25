// Fényes Balázs
// homepage: fenyesb.github.io
// repository: github.com/fenyesb/raycast

#ifndef _MAP_H
#define _MAP_H

#include "player.h"

extern int map[MAPSIZE][MAPSIZE];
void MapInit();
bool shouldMakeDoors(int room_id, IRANY dir);
#endif // _MAP_H
