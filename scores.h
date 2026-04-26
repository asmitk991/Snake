#ifndef SCORES_H
#define SCORES_H

#define MAX_SCORES   10
#define NAME_LEN     32
#define SCORES_FILE  "scores.dat"

typedef struct {
    char name[NAME_LEN];
    int  score;
    int  mode;    /* 0=easy 1=medium 2=hard */
} ScoreEntry;

void scores_load   (void);
void scores_save   (void);
void scores_add    (const char *name, int score, int mode);
int  scores_get_high(int mode);
void scores_get_top3(ScoreEntry *out, int *count, int mode);

#endif
