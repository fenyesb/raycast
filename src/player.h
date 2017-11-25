// Fényes Balázs
// homepage: fenyesb.github.io
// repository: github.com/fenyesb/raycast

#ifndef _PLAYER_H
#define _PLAYER_H

#include "geometry.h"
#include "room.h"
#include "map.h"

typedef struct{
    Point pos;
    Point vel;
    double rotvel;
    double look_angle;
    int room_id;
    int collected_coins;
} Player;

extern Player player;
Line getPlayerRay(double length,double da);
void initPlayer(Player* p);
int getPlayerRoom();
PointI getPlayerCoords();

#endif // _PLAYER_H
