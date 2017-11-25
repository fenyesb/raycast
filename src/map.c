// Fényes Balázs
// homepage: fenyesb.github.io
// repository: github.com/fenyesb/raycast

#include <stdlib.h>

#include "map.h"

int map[MAPSIZE][MAPSIZE] = {{0}};

void MapInit(){

    //a map tömb alaphelyzetbe állítása
    int x, y;
    for(x=0;x<MAPSIZE;x++)
        for(y=0;y<MAPSIZE;y++)
            map[x][y]=rand()%20==0?NEMSZOBA:URES;

    //minden eddig létrehozott szoba adatainak törlése
    for(;next_id>0;next_id--)
    {
        if(r[next_id-1].coins!=NULL)
            free(r[next_id-1].coins);
        r[next_id-1].coins=NULL;
        r[next_id-1].explored=false;
    }

    //az indulószoba beállítása
    r[0].topleft=(PointI){MAPSIZE/2,MAPSIZE/2};
    r[0].size=(PointI){1,1};
    makeRoom(0, false);
    map[MAPSIZE/2][MAPSIZE/2]=0;
    next_id=1;
}

//kell-e átjárót csinálni ebből a szobából ebbe az irányba
bool shouldMakeDoors(int room_id, IRANY dir){
    if(room_id==URES)
        return false;
    PointI c = getRoomCenter(room_id);
    int dx=dir==BALRA?-1:(dir==JOBBRA?1:0);
    int dy=dir==FEL?-1:(dir==LE?1:0);
    int x = c.x+dx*(r[room_id].size.x==3?2:1);
    int y = c.y+dy*(r[room_id].size.y==3?2:1);
    if(x<0||y<0||x>=MAPSIZE||y>=MAPSIZE)
        return false;
    int id = map[x][y];
    if(id==URES)
        return false;
    PointI co = getRoomCenter(id);
    return (dy!=0&&x==co.x)||(dx!=0&&y==co.y);
}
