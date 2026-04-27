#ifndef GAME_H
#define GAME_H

/* Board layout (18 total rows) */
#define BOARD_ROWS   16
#define BOARD_COLS   32

/* Play area boundaries */
#define PLAY_ROW_MIN  3
#define PLAY_ROW_MAX  15
#define PLAY_COL_MIN  2
#define PLAY_COL_MAX  31

/* Difficulty modes */
#define MODE_EASY    0
#define MODE_MEDIUM  1
#define MODE_HARD    2

/* Data structures */
typedef struct SnakeNode {
    int row, col;
    struct SnakeNode *next;
} SnakeNode;

typedef struct {
    SnakeNode *head;
    SnakeNode *tail;
    int        length;
    int        dir_row;
    int        dir_col;
} Snake;

typedef struct {
    int row, col;
    int active;
} Food;

typedef struct {
    int row, col;
} Wall;

/* Public API */
void game_init      (Snake *s, Food *f, int mode);
void game_free_snake(Snake *s);
int  game_tick      (Snake *s, Food *f, int mode);
void game_spawn_food(Food *f, const Snake *s, int mode);
int  game_is_wall   (int row, int col);

extern const Wall hard_walls[];
extern const int  hard_wall_count;

#endif
