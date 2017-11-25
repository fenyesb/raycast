// Fényes Balázs
// homepage: fenyesb.github.io
// repository: github.com/fenyesb/raycast

#include <math.h>

#include "player.h"

Player player;

//a játékos "szemébõl" kiinduló sugár
//da: ezzel a szöggel elforgatja a sugarat
Line getPlayerRay(double length,double da){
    return (Line){
        (Point){player.pos.x,player.pos.y},
        (Point){player.pos.x+length*cos(player.look_angle+da),player.pos.y+length*sin(player.look_angle+da)}};
}

//az a szoba, ahol a játékos jelenleg tartózkodik
int getPlayerRoom(){
    return map[(int)player.pos.x/ROOM_SIZE][(int)player.pos.y/ROOM_SIZE];
}

//a játékos paramétereinek alaphelyzetbe állítása
void initPlayer(Player* p)
{
    p->pos=(Point){(MAPSIZE/2+0.5)*ROOM_SIZE+EPSILON,(MAPSIZE/2+0.5)*ROOM_SIZE+EPSILON};
    p->collected_coins=0;
    p->look_angle=0;
    p->room_id=0;
    p->rotvel=0;
    p->vel=(Point){0,0};
}

//a játékos egész koordinátái
PointI getPlayerCoords(){
    return (PointI){(int)player.pos.x/ROOM_SIZE,(int)player.pos.y/ROOM_SIZE};
}
