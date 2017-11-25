// Fényes Balázs
// homepage: fenyesb.github.io
// repository: github.com/fenyesb/raycast

#ifndef _DEFINES_H
#define _DEFINES_H

#define WINDOWED

#ifdef WINDOWED
#define FULLSCREEN 0
#define WIDTH 640
#define HEIGHT 400
#define BLACK_BAR_HEIGHT 10
#define FONTSIZE 24
#else
#define FULLSCREEN 1
#define WIDTH 569
#define HEIGHT 320
#define BLACK_BAR_HEIGHT 7
#define FONTSIZE 20
#endif

#define MAXROOMS 100
#define ROOM_SIZE 80
#define MAPSIZE 50
#define PI 3.14159265359
#define EPSILON 1e-5
#define COIN_COLLECTION_DISTANCE 10
#define MINIMAP_MAX_ROOM_DISTANCE 3*ROOM_SIZE
#define COINSIZE 1
#define TIME_LIMIT 20
#define FOCAL_LEN 0.3
#define MAX_RENDER_DISTANCE 3*ROOM_SIZE
#define WALL_HEIGHT 6
#define NAME_MAX_LEN 20
#define SCORE_COUNT 5
#define ACCELERATION 0.3
#define ROT_ACCELERATION 0.008
#define BRAKING 0.8
#define WALL_COUNT 16
#define EFFECT_RESET 30
#define EFFECT_COLORVALUE 8
#define BONUS_TIME 3
#define BONUS_POINTS 5
#define COINS_PER_ROOM_MAX 20

typedef enum {FEL, BALRA, JOBBRA, LE} IRANY;
typedef enum {NORMAL, IDOBONUSZ, PONTBONUSZ} ERMETIPUS;
typedef enum {SZOGLETES, KEREK, FOLYOSO} SZOBATIPUS;

#define URES -1
#define NEMSZOBA MAXROOMS

#define min(lval, rval) (((lval)<(rval))?(lval):(rval))
#define max(lval, rval) (((lval)>(rval))?(lval):(rval))
#define clamp(minval, maxval, val) min((maxval),max((minval),(val)))
#define near(lval, rval) (fabs((lval)-(rval))<EPSILON)

#define X(f) ((f)-player.pos.x+WIDTH/2)
#define Y(f) ((f)-player.pos.y+HEIGHT/2)

#endif // _DEFINES_H
