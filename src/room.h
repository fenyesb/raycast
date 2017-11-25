// Fényes Balázs
// homepage: fenyesb.github.io
// repository: github.com/fenyesb/raycast

#ifndef _ROOM_H
#define _ROOM_H

#include "geometry.h"
#include "defines.h"

typedef struct{
    Point pos;
    ERMETIPUS type;
    int color;
    bool collected;
} Coin;

typedef struct{
    PointI topleft;
    PointI size;
    Line walls[WALL_COUNT];
    bool explored;
    Coin* coins;
    int coin_count;
    int type;
    int id;
} Room;

void makeCoin(Coin* c, Point p);
extern Room r[MAXROOMS];
extern int next_id;
PointI getRoomCenter(int room_id);
void makeRoom(int id, bool addcoins);
Point getRoomCenterF(int room_id);
void generateRoomWalls(int room_id);
void tryPlaceNeighbourRoom(int room_id, IRANY dir);
void initRandomColor();
int getRandomColor();
void rotate(double* x, double* y, double a);

#endif // _ROOM_H
