#include "scores.h"
#include "string.h"
#include "math.h"
#include <stdio.h>

static ScoreEntry entries[MAX_SCORES];
static int        entry_count = 0;

/* File I/O */
void scores_load(void) {
    FILE *fp = fopen(SCORES_FILE, "r");
    if (!fp) { entry_count = 0; return; }
    entry_count = 0;
    while (entry_count < MAX_SCORES) {
        ScoreEntry e;
        if (fscanf(fp, "%31s %d %d", e.name, &e.score, &e.mode) != 3) break;
        entries[entry_count++] = e;
    }
    fclose(fp);
}

void scores_save(void) {
    FILE *fp = fopen(SCORES_FILE, "w");
    if (!fp) return;
    for (int i = 0; i < entry_count; i++)
        fprintf(fp, "%s %d %d\n", entries[i].name, entries[i].score, entries[i].mode);
    fclose(fp);
}

/* Add / update */
void scores_add(const char *name, int score, int mode) {
    /* Update existing entry for same name+mode */
    for (int i = 0; i < entry_count; i++) {
        if (my_strcmp(entries[i].name, name) == 0 && entries[i].mode == mode) {
            if (score > entries[i].score) entries[i].score = score;
            scores_save();
            return;
        }
    }
    /* Insert new */
    if (entry_count < MAX_SCORES) {
        my_strcpy(entries[entry_count].name, name);
        entries[entry_count].score = score;
        entries[entry_count].mode  = mode;
        entry_count++;
    } else {
        /* Replace the lowest score if this beats it */
        int low = 0;
        for (int i = 1; i < entry_count; i++)
            if (entries[i].score < entries[low].score) low = i;
        if (score > entries[low].score) {
            my_strcpy(entries[low].name, name);
            entries[low].score = score;
            entries[low].mode  = mode;
        }
    }
    scores_save();
}

/* Queries */
int scores_get_high(int mode) {
    int high = 0;
    for (int i = 0; i < entry_count; i++)
        if (entries[i].mode == mode && entries[i].score > high)
            high = entries[i].score;
    return high;
}

void scores_get_top3(ScoreEntry *out, int *count, int mode) {
    ScoreEntry buf[MAX_SCORES];
    int fc = 0;
    for (int i = 0; i < entry_count; i++)
        if (entries[i].mode == mode) buf[fc++] = entries[i];

    /* Bubble sort descending */
    for (int i = 0; i < fc - 1; i++)
        for (int j = 0; j < fc - i - 1; j++)
            if (buf[j].score < buf[j+1].score) {
                ScoreEntry tmp = buf[j]; buf[j] = buf[j+1]; buf[j+1] = tmp;
            }

    *count = fc < 3 ? fc : 3;
    for (int i = 0; i < *count; i++) out[i] = buf[i];
}
