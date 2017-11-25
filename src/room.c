// Fényes Balázs
// homepage: fenyesb.github.io
// repository: github.com/fenyesb/raycast

#include <stdlib.h>
#include <math.h>

#include "room.h"
#include "map.h"

int _randomcolor;
int next_id;
Room r[MAXROOMS];

//érme létrehozása
void makeCoin(Coin* c, Point p){
    c->collected=false;
    c->pos.x=p.x;
    c->pos.y=p.y;
    c->type=(rand()%30)==0?PONTBONUSZ:(rand()%10)==0?IDOBONUSZ:NORMAL;
    static int ermeszin[] = {0xFFFF00FF,0xFF0000FF,0x00FFFFFF};
    c->color=ermeszin[c->type];
}

//egy szobához tartozó véletlenszerű alapszín előállítása
void initRandomColor()
{
    _randomcolor=((rand()%0x41)<<24)|((rand()%0x41)<<16)|((rand()%0x41)<<8);
    _randomcolor*=2;
    _randomcolor+=0x404040FF; //minden komponens 0x40 és 0xC0 között van
}

//egy szobán belül a különböző falszakaszok színeinek előállítása
int getRandomColor(){
    //minden komponens 0x7F és 0xFF között lesz
    return _randomcolor+(((rand()%0x40)<<24)|((rand()%0x40)<<16)|((rand()%0x40)<<8));
}

//szoba középpontja
PointI getRoomCenter(int room_id){
    return (PointI){r[room_id].topleft.x+r[room_id].size.x/2,r[room_id].topleft.y+r[room_id].size.y/2};
}

//forrás: http://stackoverflow.com/questions/1340729/how-do-you-generate-a-random-double-uniformly-distributed-between-0-and-1-from-c/26853142#26853142
double randbetween(double M, double N)
{
    return M + (rand() / ( RAND_MAX / (N-M) ) ) ;
}

//szoba létrehozása (falak + színek + érmék)
void makeRoom(int room_id, bool addcoins){
    if(room_id>=MAXROOMS)
        return;
    int i;

    r[room_id].type=rand()%3;
    generateRoomWalls(room_id);

    //érmék generálása
    if(addcoins){
        r[room_id].coin_count = rand()%(COINS_PER_ROOM_MAX+1);
        r[room_id].coins=malloc(sizeof(Coin)*r[room_id].coin_count);
        for(i=0;i<r[room_id].coin_count;i++)
        {
            Room* room = &r[room_id];
            switch(room->type)
            {
            case 0://normál
                makeCoin(&room->coins[i],(Point){randbetween(0.2,0.8),randbetween(0.2,0.8)});
                break;
            case 1://kerek
                {
                    double ra = randbetween(0,2*PI);
                    double rd = randbetween(0,0.35);
                    makeCoin(&room->coins[i],(Point){rd*cos(ra)+0.5,rd*sin(ra)+0.5});
                }
                break;
            case 2://folyosó
                if(rand()%2)
                    makeCoin(&room->coins[i],(Point){randbetween(0.2,0.8),randbetween(0.5-0.08/room->size.y,0.5+0.08/room->size.y)});
                else
                    makeCoin(&room->coins[i],(Point){randbetween(0.5-0.08/room->size.x,0.5+0.08/room->size.x),randbetween(0.2,0.8)});
                break;
            }
            room->coins[i].pos.x=ROOM_SIZE*(room->topleft.x+room->size.x*room->coins[i].pos.x);
            room->coins[i].pos.y=ROOM_SIZE*(room->topleft.y+room->size.y*room->coins[i].pos.y);
        }
    }else{
        r[room_id].coin_count=0;
        r[room_id].coins=NULL;
    }

    //falak színeinek beállítása
    initRandomColor();
    for(i=0;i<WALL_COUNT;i++)
        r[room_id].walls[i].color=getRandomColor();

    r[room_id].id=room_id;
}

//szoba középpontja, valódi koordinátákkal
Point getRoomCenterF(int room_id){
    PointI c = getRoomCenter(room_id);
    return (Point){c.x*ROOM_SIZE,c.y*ROOM_SIZE};
}

//pont forgatása adott szöggel
void rotate(double* x, double* y, double a)
{
    double xnew = *x*cos(a)-*y*sin(a);
    *y=*y*cos(a)+*x*sin(a);
    *x=xnew;
}

//a falak és átjárók előállítása a szoba típusától függően
void generateRoomWalls(int room_id){
    Line* lines = malloc(sizeof(Line)*WALL_COUNT);
    static IRANY dirs[] = {JOBBRA, LE, BALRA, FEL};
    static double wall_data[][WALL_COUNT] = {{0.4,-0.4,0.4,-0.2,0.4,-0.2,0.4,0,0.4,0,0.4,0.2,0.4,0.2,0.4,0.4}, //normál
                                      {0.4,-0.4,0.4,-0.1,0.4,-0.1,0.5,-0.1,0.5,0.1,0.4,0.1,0.4,0.1,0.4,0.4},//normál, átjáróval
                                      {0.2828,-0.2828,0.4,-0.2,0.4,-0.2,0.4,0,0.4,0,0.4,0.2,0.4,0.2,0.2828,0.2828}, //kerek
                                      {0.2828,-0.2828,0.4,-0.1,0.4,-0.1,0.5,-0.1,0.5,0.1,0.4,0.1,0.4,0.1,0.2828,0.2828}, //kerek, átjáróval
                                      {0.1,0.1,0.4,0.1, 0.4,0.1,0.4,0, 0.4,0,0.4,-0.1, 0.4,-0.1,0.1,-0.1}, //folyosó
                                      {0.1,0.1,0.3,0.1, 0.3,0.1,0.5,0.1, 0.5,-0.1,0.3,-0.1, 0.3,-0.1,0.1,-0.1}}; //folyosó, átjáróval

    //létrehozzuk a szoba mind a négy oldalához tartozó falakat a wall_data[][] alapján
    int i,j;
    for(i=0;i<4;i++){
        int type = r[room_id].type*2+(shouldMakeDoors(room_id,dirs[i])?1:0);
        for(j=0;j<4;j++){
            double x1=wall_data[type][4*j],
                   y1=wall_data[type][4*j+1],
                   x2=wall_data[type][4*j+2],
                   y2=wall_data[type][4*j+3];
            rotate(&x1,&y1,i*PI/2);
            rotate(&x2,&y2,i*PI/2);
            lines[4*i+j]=(Line){(Point){x1,y1},(Point){x2,y2},~0};
        }
    }

    //az ajtók mindig azonos szélességűek maradnak, hogy össze lehessen kapcsolni a különböző méretű szobákat
    for(i=0;i<WALL_COUNT;i++)
    {
        if(r[room_id].size.x==3){
            if(fabs(fabs(lines[i].p1.x)-0.1)<EPSILON)
                lines[i].p1.x/=3;
            if(fabs(fabs(lines[i].p2.x)-0.1)<EPSILON)
                lines[i].p2.x/=3;
        }
        if(r[room_id].size.y==3){
            if(fabs(fabs(lines[i].p1.y)-0.1)<EPSILON)
                lines[i].p1.y/=3;
            if(fabs(fabs(lines[i].p2.y)-0.1)<EPSILON)
                lines[i].p2.y/=3;
        }

        //a [-0.5, 0.5] koordináták átalakítása játékbeli koordinátákká (a szobák mérete ROOM_SIZE többszörösei)
        lines[i].p1.x=((lines[i].p1.x+0.5)*r[room_id].size.x+r[room_id].topleft.x)*ROOM_SIZE;
        lines[i].p1.y=((lines[i].p1.y+0.5)*r[room_id].size.y+r[room_id].topleft.y)*ROOM_SIZE;
        lines[i].p2.x=((lines[i].p2.x+0.5)*r[room_id].size.x+r[room_id].topleft.x)*ROOM_SIZE;
        lines[i].p2.y=((lines[i].p2.y+0.5)*r[room_id].size.y+r[room_id].topleft.y)*ROOM_SIZE;
    }

    if(r[room_id].coin_count==0||r[room_id].coins!=NULL)//a szoba már korábban létre volt hozva, a falszínek maradjanak meg
        for(i=0;i<WALL_COUNT;i++)
            lines[i].color=r[room_id].walls[i].color;

    for(i=0;i<WALL_COUNT;i++)
        r[room_id].walls[i] = lines[i];
}

//megpróbál egy adott szobából egy adott irányba egy véletlenszerű szobát elhelyezni
void tryPlaceNeighbourRoom(int room_id, IRANY dir){
    if(next_id>=MAXROOMS)
        return;
    PointI c = getRoomCenter(room_id);
    int dx=dir==BALRA?-1:(dir==JOBBRA?1:0);
    int dy=dir==FEL?-1:(dir==LE?1:0);
    int x = c.x+dx*(r[room_id].size.x/2+1);
    int y = c.y+dy*(r[room_id].size.y/2+1);
    if(x<2||y<2||x>=MAPSIZE-2||y>=MAPSIZE-2)//különben pl. 3x3-as generálódna map[][] határain kívül
        return;

    //a szoba lehet 1x1, 1x3, 3x1 vagy 3x3 méretű
    int sx =rand()&1?1:3, sy=rand()&1?1:3;
    r[next_id].size=(PointI){sx,sy};

    //a méret és az irány alapján meghatározzuk, hogy mik a szoba bal-felső sarkának koordinátái
    switch(dir)
    {
    case JOBBRA:
        r[next_id].topleft=(PointI){x,sy==1?y:y-1};
        break;
    case BALRA:
        r[next_id].topleft=(PointI){sx==1?x:x-2,sy==1?y:y-1};
        break;
    case FEL:
        r[next_id].topleft=(PointI){sx==1?x:x-1,sy==1?y:y-2};
        break;
    case LE:
        r[next_id].topleft=(PointI){sx==1?x:x-1,y};
        break;
    }

    //ha nem lehet lehelyezni a map[][] tömbbe a szobát, mert nem üres az a terület, visszatérünk a fv.-ből
    for(x=0;x<r[next_id].size.x;x++)
        for(y=0;y<r[next_id].size.y;y++)
            if(map[x+r[next_id].topleft.x][y+r[next_id].topleft.y]!=URES)
                return;

    makeRoom(next_id, true);

    //a szoba azonosítójának beírása a map[][] tömbbe
    for(x=0;x<r[next_id].size.x;x++)
        for(y=0;y<r[next_id].size.y;y++)
            map[x+r[next_id].topleft.x][y+r[next_id].topleft.y]=next_id;

    next_id++;
}
