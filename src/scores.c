// Fényes Balázs
// homepage: fenyesb.github.io
// repository: github.com/fenyesb/raycast

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "scores.h"
#include "gamelogic.h"
#include "player.h"

Score scores[SCORE_COUNT];

//a pontszámlista betöltése, vagy ha nincs ilyen, akkor minden eddigi eredmény 0 pontos lesz
void loadscores()
{
    int i=-1;
    FILE* f = fopen("scores.txt","r");
    if(!f)//nem létezik a fájl
        for(i=0;i<SCORE_COUNT;i++)
            scores[i]=(Score){"JATEKOS",0};
    else{
        static char buf[NAME_MAX_LEN+10];
        while(fgets(buf, NAME_MAX_LEN+10,f)&&++i<SCORE_COUNT)
        {
            sscanf(buf,"%s %d",scores[i].name,&scores[i].points);
            if(scores[i].name[0]=='-')
                scores[i].name[0]=0;
        }
        fclose(f);
    }
}

int _cmpfunc(const void* a, const void* b)
{
    return ((Score*)b)->points-((Score*)a)->points;
}

//a játékos eredményének eltárolása a pontszámlistában
void savescores()
{
    Score temp[SCORE_COUNT+1];
    memcpy(temp,scores,sizeof(Score)*SCORE_COUNT);
    memcpy(temp[SCORE_COUNT].name,name,sizeof(char)*NAME_MAX_LEN);
    temp[SCORE_COUNT].points=player.collected_coins;
    qsort(temp,SCORE_COUNT+1,sizeof(Score),_cmpfunc);
    memcpy(scores,temp,sizeof(Score)*SCORE_COUNT);
    FILE* f = fopen("scores.txt","w+");
    int i;
    for(i=0;i<SCORE_COUNT;i++)
        fprintf(f,"%s %d\n",scores[i].name[0]==0?"JATEKOS":scores[i].name,scores[i].points);
    fclose(f);
}
