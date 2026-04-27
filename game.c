#include "game.h"
#include "math.h"
#include "memory.h"
#include <stdlib.h>

/* Hard mode maze walls */
const Wall hard_walls[] = {
    /* Top row dividers */
    {5, 6},{5, 7},{5, 8},{5, 9},
    {5, 23},{5, 24},{5, 25},{5, 26},

    /* Bottom row dividers */
    {13, 6},{13, 7},{13, 8},{13, 9},
    {13, 23},{13, 24},{13, 25},{13, 26},
    
    /* Center vertical stubs */
    {7, 16},{8, 16},
    {11, 16},{12, 16},
};
const int hard_wall_count = (int)(sizeof(hard_walls) / sizeof(hard_walls[0]));

/* Helpers */
int game_is_wall(int row, int col) {
    for (int i = 0; i < hard_wall_count; i++)
        if (hard_walls[i].row == row && hard_walls[i].col == col) return 1;
    return 0;
}

static int snake_occupies(const Snake *s, int row, int col) {
    for (SnakeNode *n = s->head; n; n = n->next)
        if (n->row == row && n->col == col) return 1;
    return 0;
}

/* Public functions */
void game_free_snake(Snake *s) {
    SnakeNode *cur = s->head;
    while (cur) {
        SnakeNode *nxt = cur->next;
        my_dealloc(cur);
        cur = nxt;
    }
    s->head = s->tail = 0;
    s->length = 0;
}

void game_init(Snake *s, Food *f, int mode) {
    game_free_snake(s);

    /* Start at centre. Snake is frozen until player moves. */
    SnakeNode *h = (SnakeNode *)my_alloc((int)sizeof(SnakeNode));
    h->row = 9; h->col = 16; h->next = 0;
    s->head   = s->tail = h;
    s->length  = 1;
    s->dir_row = 0;
    s->dir_col = 0;

    game_spawn_food(f, s, mode);
}

void game_spawn_food(Food *f, const Snake *s, int mode) {
    /* Spawns food at least 1 cell away from all walls */
    int rmin = PLAY_ROW_MIN + 1;
    int rmax = PLAY_ROW_MAX - 1;
    int cmin = PLAY_COL_MIN + 1;
    int cmax = PLAY_COL_MAX - 1;

    int row, col, tries = 0;
    do {
        row = rmin + my_mod(rand(), rmax - rmin + 1);
        col = cmin + my_mod(rand(), cmax - cmin + 1);
        tries++;
    } while (tries < 500 &&
             (snake_occupies(s, row, col) ||
              (mode == MODE_HARD && game_is_wall(row, col))));
    f->row = row; f->col = col; f->active = 1;
}

int game_tick(Snake *s, Food *f, int mode) {
    if (s->dir_row == 0 && s->dir_col == 0) return 0;

    int nr = s->head->row + s->dir_row;
    int nc = s->head->col + s->dir_col;

    /* Boundary handling */
    if (mode == MODE_EASY) {
        if (nr < PLAY_ROW_MIN) nr = PLAY_ROW_MAX;
        if (nr > PLAY_ROW_MAX) nr = PLAY_ROW_MIN;
        if (nc < PLAY_COL_MIN) nc = PLAY_COL_MAX;
        if (nc > PLAY_COL_MAX) nc = PLAY_COL_MIN;
    } else {
        if (!my_inbounds(nr, PLAY_ROW_MIN, PLAY_ROW_MAX + 1) ||
            !my_inbounds(nc, PLAY_COL_MIN, PLAY_COL_MAX + 1)) return 1;
        if (mode == MODE_HARD && game_is_wall(nr, nc))          return 1;
    }

    /* Will we eat food this tick? */
    int ate = (nr == f->row && nc == f->col && f->active);

    /* Self-collision */
    for (SnakeNode *n = s->head; n; n = n->next) {
        if (!ate && n->next == 0) continue;
        if (n->row == nr && n->col == nc) return 1;
    }

    /* Add new head */
    SnakeNode *nh = (SnakeNode *)my_alloc((int)sizeof(SnakeNode));
    if (!nh) return 1;
    nh->row = nr; nh->col = nc; nh->next = s->head;
    s->head = nh;
    s->length++;

    if (ate) {
        game_spawn_food(f, s, mode);
    } else {
        /* Remove tail */
        SnakeNode *cur = s->head;
        while (cur->next && cur->next->next) cur = cur->next;
        my_dealloc(cur->next);
        cur->next = 0;
        s->tail   = cur;
        s->length--;
    }

    return 0;
}
