// Fényes Balázs
// homepage: fenyesb.github.io
// repository: github.com/fenyesb/raycast

#include <math.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_ttf.h>

#include "render.h"
#include "player.h"
#include "defines.h"
#include "gamelogic.h"
#include "scores.h"

SDL_Surface* screen;
TTF_Font *bigfont, *normalfont, *smallfont;
int bonus_time_effect=0, coin_pickup_effect=0;

void RenderInit()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    screen=SDL_SetVideoMode(WIDTH, HEIGHT, 0, SDL_ANYFORMAT|(FULLSCREEN?SDL_FULLSCREEN:0));
    SDL_ShowCursor(SDL_FALSE);
    SDL_WM_SetCaption("Raytracing játék", "Raytracing játék");
    TTF_Init();
    //a használt betűtípusok forrása: http://www.1001freefonts.com/
    bigfont = TTF_OpenFont("destructobeambb_reg.ttf",FONTSIZE);
    normalfont = TTF_OpenFont("nulshock bd.ttf", (int)(FONTSIZE*0.8));
    smallfont=TTF_OpenFont("renegado.ttf",FONTSIZE/2);
}

//szöveg rajzolása a képernyőre fekete árnyékkal
void drawString(char* str, int x, int y, SDL_Color sdlc, TTF_Font* font)
{
    SDL_Surface* felirat = TTF_RenderUTF8_Blended(font,str,(SDL_Color){0,0,0});
    SDL_Rect hova = (SDL_Rect){x,y,0,0};
    SDL_BlitSurface(felirat,NULL,screen,&hova);
    felirat = TTF_RenderUTF8_Blended(font,str,sdlc);
    hova = (SDL_Rect){x-2,y-2,0,0};
    SDL_BlitSurface(felirat,NULL,screen,&hova);
    SDL_FreeSurface(felirat);
}

//egy int kiírása a képernyőre
void drawInt(int d, int x, int y, SDL_Color sdlc){
    static char buf[10]={0};
    sprintf(buf,"%d",d);
    drawString(buf,x,y,sdlc, bigfont);
}

//az adott oszlop rajzolásánál mekkora szöggel kell elforgatni a kamera sugarát
double getViewAngle(int col){
    return atan2((double)col/WIDTH-0.5,FOCAL_LEN);
}

//meg kell rajzolni ezt a szobát?
bool shouldRender(Room room){
    int dx, dy;
    PointI p = getPlayerCoords();
    for(dx=-1;dx<=1;dx++)
        for(dy=-1;dy<=1;dy++)
            if(abs(dx)+abs(dy)<=1&&map[p.x+dx][p.y+dy]==room.id)
                return true;
    return false;
}

//a rajzolt oszlop színe a metszéspont távolságától is függ, ezt adja meg ez a függvény
int getFogColor(Point pt)
{
    return clamp(0,255,(100-getDistance(player.pos,pt))*3);
}

//vonalrajzoló segédfüggvény
void drawLine(Line l, int color)
{
    aalineColor(screen,X(l.p1.x),Y(l.p1.y),X(l.p2.x),Y(l.p2.y),color);
}

//a minitérkép kirajzolása
void drawMinimap(){
    int room_id,coin_id,wall_id;
    for(room_id=0;room_id<next_id;room_id++){
        for(wall_id=0;wall_id<WALL_COUNT;wall_id++){
            if(getDistance(player.pos,getRoomCenterF(room_id))>MINIMAP_MAX_ROOM_DISTANCE)
                continue;
            Line w = r[room_id].walls[wall_id];
            drawLine(w,(w.color&~0xFF)|getFogColor(getMidpoint(w)));
        }
        for(coin_id=0;coin_id<r[room_id].coin_count;coin_id++){
            Coin c = r[room_id].coins[coin_id];
            if(!c.collected)
                ellipseColor(screen,X(c.pos.x),Y(c.pos.y),
                         3,3,0xFFFFFF00|getFogColor(c.pos));
        }
    }
    ellipseColor(screen,X(player.pos.x),Y(player.pos.y),5,5,~0);
    Line aim = getPlayerRay(10,0);
    drawLine(aim,~0);
}

void RenderFrame(){
    if(!redraw_request)
        return;
    redraw_request=false;

    //háttér rajzolása
    static SDL_Rect re;
    re = (SDL_Rect) {0,0,WIDTH,HEIGHT/2-BLACK_BAR_HEIGHT/2};
    SDL_FillRect(screen,&re,SDL_MapRGB(screen->format, 30, 30, 200));
    re = (SDL_Rect) {0,HEIGHT/2-BLACK_BAR_HEIGHT/2,WIDTH,BLACK_BAR_HEIGHT};
    SDL_FillRect(screen,&re,SDL_MapRGB(screen->format,0,0,0));
    re = (SDL_Rect){0,HEIGHT/2+BLACK_BAR_HEIGHT/2,WIDTH,HEIGHT/2-BLACK_BAR_HEIGHT/2};
    SDL_FillRect(screen,&re,SDL_MapRGB(screen->format, 140,30,20));

    //falak/érmék rajzolása
    int col,i,room_id,min_dist_color=0xFF;
    for(col=0;col<WIDTH;col++){
        double min_dist = MAX_RENDER_DISTANCE+1, dist;
        Line lookat = getPlayerRay(min_dist,getViewAngle(col));
        for(room_id=0;room_id<next_id;room_id++){
            if(shouldRender(r[room_id])){

                //legközelebbi fal megkeresése
                for(i=0;i<WALL_COUNT;i++){
                    Line w = r[room_id].walls[i];
                    if(RaySegmentIntersection(lookat,w,&dist)&&dist<min_dist)
                    {
                        min_dist=dist;
                        min_dist_color=w.color;
                    }
                }

                //legközelebbi érme megkeresése
                for(i=0;i<r[room_id].coin_count;i++){
                    Coin c = r[room_id].coins[i];
                    if(c.collected)
                        continue;
                    Line coin1 = (Line){
                        (Point){c.pos.x-COINSIZE,c.pos.y},
                        (Point){c.pos.x+COINSIZE,c.pos.y},0};
                    Line coin2 = (Line){
                        (Point){c.pos.x,c.pos.y-COINSIZE},
                        (Point){c.pos.x,c.pos.y+COINSIZE},0};
                    if(RaySegmentIntersection(lookat,coin1,&dist)&&dist<min_dist)
                    {
                        min_dist=dist;
                        min_dist_color=c.color;
                    }
                    if(RaySegmentIntersection(lookat,coin2,&dist)&&dist<min_dist)
                    {
                        min_dist=dist;
                        min_dist_color=c.color;
                    }
                }
            }
        }

        //a legközelebbi metszésponthoz tartozó színnel a távolságnak megfelelő oszlop rajzolása
        if(min_dist<=MAX_RENDER_DISTANCE){
            int distcolor = clamp(0,255,255-min_dist);
            double z = min_dist*cos(getViewAngle(col)); //a halszemeffektus kiküszöbölése miatt
            double h = HEIGHT * WALL_HEIGHT / z;
            lineColor(screen,col,HEIGHT/2-h/2,col,HEIGHT/2+h/2,0xFF);
            lineColor(screen,col,HEIGHT/2-h/2+1,col,HEIGHT/2+h/2-1,(min_dist_color&~0xFF)|distcolor);
        }
    }

    if(isplaying){
        drawMinimap();
        drawInt(player.collected_coins,WIDTH/2-30,HEIGHT/2+10,(SDL_Color){255,255,coin_pickup_effect*8});
        drawInt(time_remaining,WIDTH/2+20,HEIGHT/2+10,(SDL_Color){255,bonus_time_effect*8,bonus_time_effect*8});
    }else{
        drawString("Mozgás: W/A/S/D, Forgás: Q/E", 20,20,(SDL_Color){255,255,255},normalfont);
        drawString("Kilépés: ESCAPE, Indítás: ENTER",20,40,(SDL_Color){255,255,255},normalfont);
        drawString("Adja meg a nevét (csak A-Z és 0-9):",20,60,(SDL_Color){255,255,255},normalfont);
        char editbox[NAME_MAX_LEN+1];
        sprintf(editbox,"%s|",name);
        drawString(editbox,20,80,(SDL_Color){0,255,255},bigfont);
        for(i=0;i<SCORE_COUNT;i++){
            char buf[NAME_MAX_LEN+10];
            sprintf(buf, "%d %s",scores[i].points,scores[i].name);
            drawString(buf, 20, 120+20*i,(SDL_Color){200,200,200},normalfont);
        }
    }
    drawString("készítette: Fényes Balázs",20,HEIGHT-40,(SDL_Color){150,150,150},smallfont);
    SDL_Flip(screen);
}
