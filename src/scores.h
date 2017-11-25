// Fényes Balázs
// homepage: fenyesb.github.io
// repository: github.com/fenyesb/raycast

#ifndef _SCORES_H
#define _SCORES_H

#include "defines.h"

typedef struct {
    char name[NAME_MAX_LEN];
    int points;
} Score;
extern Score scores[SCORE_COUNT];
void savescores();
void loadscores();

#endif // _SCORES_H
