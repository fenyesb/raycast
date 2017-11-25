// Fényes Balázs
// homepage: fenyesb.github.io
// repository: github.com/fenyesb/raycast

#include <time.h>
#include <stddef.h>
#include <math.h>
#include <SDL.h>
#include <stdio.h>

#include "gamelogic.h"
#include "render.h"
#include "map.h"
#include "player.h"
#include "scores.h"

bool quit=false;
uint8_t* keystate;
bool redraw_request=false;
bool isplaying=false;
int time_remaining=0;
bool is_timer_running = false;
SDL_TimerID sdlt;
int name_ptr = 0;
char name[NAME_MAX_LEN] = {0};

//másodpercenként hívodik meg
uint32_t tick(uint32_t interval, void* param)
{
    time_remaining--;
    redraw_request=true;
    return interval;
}

void GameInit(){
    srand(time(NULL));
    MapInit();
    initPlayer(&player);
    loadscores();
    redraw_request=true;
    if(!is_timer_running){
        is_timer_running=true;
        sdlt=SDL_AddTimer(1000,tick,NULL);
    }
    isplaying=false;
}

//játékos forgatása gomb lenyomása esetén
void RotatePlayerOnKey(SDLKey key, double direction)
{
    if(keystate[key]){
        player.rotvel+=direction*ROT_ACCELERATION;
        redraw_request=true;
    }
}

//játékos gyorsítása gomb lenyomása esetén
void MovePlayerOnKey(SDLKey key,double direction)
{
    if(keystate[key]){
        player.vel.x+=ACCELERATION*cos(player.look_angle+direction);
        player.vel.y+=ACCELERATION*sin(player.look_angle+direction);
        redraw_request=true;
    }
}

void GameStep(){

    static SDL_Event ev;
    while(SDL_PollEvent(&ev)){
        if(ev.type==SDL_QUIT)
            quit=true;
        else if(ev.type==SDL_USEREVENT)
            redraw_request=true;
        else if(!isplaying&&ev.type==SDL_KEYDOWN)
        {
            if(ev.key.keysym.sym==SDLK_BACKSPACE){
                name_ptr=max(0,name_ptr-1);
                name[name_ptr]=0;
                redraw_request=true;
            }else if(ev.key.keysym.sym==SDLK_RETURN){
                isplaying=false;
                redraw_request=true;
            }else{
                char* temp = SDL_GetKeyName(ev.key.keysym.sym);
                if(temp[1]==0){
                    char upper = toupper(temp[0]);
                    if((upper>='A'&&upper<='Z')||(upper>='0'&&upper<='9'))
                    {//csak ezek a karakterek elfogadottak
                        name[name_ptr]=upper;
                        name_ptr=min(NAME_MAX_LEN-2,name_ptr+1);
                        redraw_request=true;
                    }
                }
            }
        }
    }
    keystate = SDL_GetKeyState(NULL);

    if(isplaying){
        //gombnyomások kezelése (forgás/mozgás/kilépés)
        RotatePlayerOnKey(SDLK_q,-1);
        RotatePlayerOnKey(SDLK_e,1);
        MovePlayerOnKey(SDLK_w,0);
        MovePlayerOnKey(SDLK_a,-PI/2);
        MovePlayerOnKey(SDLK_s,PI);
        MovePlayerOnKey(SDLK_d,PI/2);
        if(keystate[SDLK_ESCAPE])
            quit=true;

        Point pos_ = (Point){player.pos.x+player.vel.x,player.pos.y+player.vel.y};

        //csak akkor mozoghat a játékos, ha nem megy át egy falon sem
        int i;
        if(player.room_id!=-1){
            Line dr = (Line){(Point){player.pos.x,player.pos.y},(Point){pos_.x,pos_.y},0};
            double dist;
            for(i=0;i<16;i++)
                if(RaySegmentIntersection(dr,r[player.room_id].walls[i],&dist))
                {
                    player.vel.x=0;
                    player.vel.y=0;
                    break;
                }
        }

        //játékos mozgatása, forgatása, lassítása
        player.pos.x+=player.vel.x;
        player.pos.y+=player.vel.y;
        player.look_angle+=player.rotvel;
        player.vel.x*=BRAKING;
        player.vel.y*=BRAKING;
        player.rotvel*=BRAKING;

        //a játékos nem mehet ki a pályából
        player.pos.x=max(0,min(player.pos.x,MAPSIZE*ROOM_SIZE));
        player.pos.y=max(0,min(player.pos.y,MAPSIZE*ROOM_SIZE));

        //ha egy új szobába lépünk be, akkor hozzuk létre az ebbõl induló szobákat
        player.room_id=getPlayerRoom();
        if(!r[player.room_id].explored){
            r[player.room_id].explored=true;
            tryPlaceNeighbourRoom(player.room_id,JOBBRA);
            tryPlaceNeighbourRoom(player.room_id,FEL);
            tryPlaceNeighbourRoom(player.room_id,BALRA);
            tryPlaceNeighbourRoom(player.room_id,LE);
            generateRoomWalls(player.room_id);
        }

        //ha a játékos más mezõre lépett, a közelben lévõ szobák falait újraszámoljuk (hátha keletkezett olyan szobapár, amik között átjáró nyitható)
        PointI pi = getPlayerCoords();
        static PointI pi_last = (PointI){-1,-1};
        if(pi.x!=pi_last.x||pi.y!=pi_last.y){
            int dx,dy;
            for(dx=-2;dx<=2;dx++)
                for(dy=-2;dy<=2;dy++)
                {
                    if(pi.x+dx>=MAPSIZE||pi.x+dx<0||pi.y+dy>=MAPSIZE||pi.y+dy<0)
                        continue;
                    int room_id = map[pi.x+dx][pi.y+dy];
                    if(room_id!=URES&&room_id!=NEMSZOBA)
                        generateRoomWalls(room_id);
                }
            pi_last.x=pi.x;
            pi_last.y=pi.y;
        }

        //a pont, és időszámláló felvillanó effektjének változatása
        if(coin_pickup_effect!=0)
            coin_pickup_effect--;
        if(bonus_time_effect!=0)
            bonus_time_effect--;

        //a játékos közelében lévõ érmék összegyûjtése
        for(i=0;i<r[player.room_id].coin_count;i++)
        {
            Coin* c = &r[player.room_id].coins[i];
            if(!c->collected&&getDistance(c->pos,player.pos)<=COIN_COLLECTION_DISTANCE)
            {
                c->collected=true;
                switch(c->type)
                {
                case NORMAL:
                    player.collected_coins++;
                    coin_pickup_effect=EFFECT_RESET;
                    break;
                case PONTBONUSZ:
                    player.collected_coins+=BONUS_POINTS;
                    coin_pickup_effect=EFFECT_RESET;
                    break;
                case IDOBONUSZ:
                    time_remaining+=BONUS_TIME;
                    bonus_time_effect=EFFECT_RESET;
                    break;
                }
            }
        }

        //ha lejárt az idő, vége a játéknak
        if(time_remaining==0){
            if(is_timer_running)
                SDL_RemoveTimer(sdlt);
            is_timer_running=false;
            savescores();
            isplaying=false;
        }

        //ha még mozgunk, vagy animálni kell, akkor mindenképpen újrarajzoljuk a képernyőt
        if(!near(player.vel.x,0)||!near(player.vel.y,0)||!near(player.rotvel,0)||coin_pickup_effect!=0||bonus_time_effect!=0)
            redraw_request=true;

    }else{
        if(keystate[SDLK_ESCAPE])
            quit=true;
        if(keystate[SDLK_RETURN])
        {
            GameInit();
            time_remaining=TIME_LIMIT;
            isplaying=true;
        }
    }
}
