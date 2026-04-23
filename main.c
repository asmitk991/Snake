/* 
 * A simple snake game. 
 * Use WASD or arrows to move. Don't hit walls or yourself. 
 */

#include "math.h"
#include "memory.h"
#include "screen.h"
#include "keyboard.h"
#include "string.h"

#include <stdio.h>    /* allowed: terminal I/O */
#include <stdlib.h>   /* allowed: exit()       */
#define _BSD_SOURCE
#include <unistd.h>   /* usleep — timing hardware abstraction */

/* screen size */
#define BOARD_ROWS    20
#define BOARD_COLS    40

#define PLAY_ROW_MIN  2
#define PLAY_ROW_MAX  (BOARD_ROWS - 1)
#define PLAY_COL_MIN  2
#define PLAY_COL_MAX  (BOARD_COLS - 1)

#define PLAY_ROWS     (PLAY_ROW_MAX - PLAY_ROW_MIN + 1)   /* 18 */
#define PLAY_COLS     (PLAY_COL_MAX - PLAY_COL_MIN + 1)   /* 38 */

#define TICK_US       130000   /* ~7.7 fps */

/* one piece of the snake */
typedef struct Segment {
    int row;
    int col;
    struct Segment *next;
} Segment;

/* variables we need throughout */
static Segment *snake_head = 0;
static Segment *snake_tail = 0;
static int      snake_len  = 0;

static int dir_row   =  0;
static int dir_col   =  1;   /* start moving right */

static int food_row  =  0;
static int food_col  =  0;

static int score     =  0;
static int game_over =  0;

static int rand_seed =  7;   /* tick-based pseudo-random seed */

/* functions defined later */
static Segment *new_segment(int row, int col);
static void     free_segment(Segment *s);
static void     snake_push_head(int row, int col);
static void     snake_pop_tail(void);
static int      on_snake(int row, int col);
static void     spawn_food(void);
static void     update_direction(int key);
static void     tick(void);
static void     render(void);

/* main entry */
int main(void) {
    mem_init();
    keyboard_init();
    printf("\033[?25l");   /* hide cursor */

    /* Snake starts as ONE segment in the centre */
    int start_row = PLAY_ROW_MIN + my_div(PLAY_ROWS, 2);
    int start_col = PLAY_COL_MIN + my_div(PLAY_COLS, 2);
    snake_push_head(start_row, start_col);

    spawn_food();

    screen_clear();
    while (!game_over) {
        int key = key_pressed();
        if (key == KEY_QUIT) break;
        update_direction(key);
        tick();
        render();
        usleep(TICK_US);
        rand_seed++;
    }

    /* Game-over message */
    int mid_row = my_div(BOARD_ROWS, 2);
    int mid_col = my_div(BOARD_COLS, 2) - 7;
    screen_move(mid_row,     mid_col); screen_putstr("  GAME OVER  ");
    screen_move(mid_row + 1, mid_col); screen_putstr("   Score: ");
    char sbuf[16];
    my_itoa(score, sbuf);
    screen_putstr(sbuf);
    screen_move(BOARD_ROWS + 2, 1);
    screen_flush();

    /* Free all remaining segments */
    Segment *cur = snake_head;
    while (cur) {
        Segment *tmp = cur->next;
        my_dealloc(cur);
        cur = tmp;
    }

    printf("\033[?25h");   /* restore cursor */
    keyboard_restore();
    return 0;
}

/* helpers for snake parts */
static Segment *new_segment(int row, int col) {
    Segment *s = (Segment *)my_alloc((int)sizeof(Segment));
    if (!s) return 0;
    s->row  = row;
    s->col  = col;
    s->next = 0;
    return s;
}

static void free_segment(Segment *s) {
    my_dealloc(s);
}

static void snake_push_head(int row, int col) {
    Segment *s = new_segment(row, col);
    if (!s) return;
    s->next    = snake_head;
    snake_head = s;
    if (!snake_tail) snake_tail = s;
    snake_len++;
}

static void snake_pop_tail(void) {
    if (!snake_head) return;
    if (snake_head == snake_tail) {
        free_segment(snake_head);
        snake_head = snake_tail = 0;
        snake_len  = 0;
        return;
    }
    Segment *prev = snake_head;
    while (prev->next != snake_tail) prev = prev->next;
    free_segment(snake_tail);
    snake_tail       = prev;
    snake_tail->next = 0;
    snake_len--;
}

static int on_snake(int row, int col) {
    Segment *cur = snake_head;
    while (cur) {
        if (cur->row == row && cur->col == col) return 1;
        cur = cur->next;
    }
    return 0;
}

/* put food somewhere random */
static void spawn_food(void) {
    int r, c, attempts = 0;
    do {
        rand_seed = my_mod(my_mul(rand_seed, 31337) + 12345, 32768);
        r = PLAY_ROW_MIN + my_mod(my_abs(rand_seed), PLAY_ROWS);
        rand_seed = my_mod(my_mul(rand_seed, 31337) + 12345, 32768);
        c = PLAY_COL_MIN + my_mod(my_abs(rand_seed), PLAY_COLS);
        attempts++;
    } while (on_snake(r, c) && attempts < 500);
    food_row = r;
    food_col = c;
}

/* handle user typing stuff */
static void update_direction(int key) {
    if (key == KEY_UP    && dir_row !=  1) { dir_row = -1; dir_col =  0; }
    if (key == KEY_DOWN  && dir_row != -1) { dir_row =  1; dir_col =  0; }
    if (key == KEY_LEFT  && dir_col !=  1) { dir_row =  0; dir_col = -1; }
    if (key == KEY_RIGHT && dir_col != -1) { dir_row =  0; dir_col =  1; }
}

/* move everything forward by one step */
static void tick(void) {
    if (!snake_head) return;

    int new_row = snake_head->row + dir_row;
    int new_col = snake_head->col + dir_col;

    /* Wall collision */
    if (!my_inbounds(new_row, PLAY_ROW_MIN, PLAY_ROW_MAX + 1) ||
        !my_inbounds(new_col, PLAY_COL_MIN, PLAY_COL_MAX + 1)) {
        game_over = 1;
        return;
    }

    /* Self collision */
    if (on_snake(new_row, new_col)) {
        game_over = 1;
        return;
    }

    /* Advance head */
    snake_push_head(new_row, new_col);

    /* Ate food? Grow (keep tail). Otherwise move (pop tail). */
    if (new_row == food_row && new_col == food_col) {
        score++;
        spawn_food();
    } else {
        snake_pop_tail();
    }
}

/* draw the board on the screen */
static void render(void) {
    screen_clear();
    screen_draw_border(BOARD_ROWS, BOARD_COLS);

    /* Food */
    screen_move(food_row, food_col);
    screen_putchar('*');

    /* Snake — '@' head, 'o' body */
    Segment *cur = snake_head;
    int first = 1;
    while (cur) {
        screen_move(cur->row, cur->col);
        screen_putchar(first ? '@' : 'o');
        first = 0;
        cur = cur->next;
    }

    /* HUD */
    screen_move(BOARD_ROWS + 1, 1);
    screen_putstr("Score: ");
    char sbuf[16];
    my_itoa(score, sbuf);
    screen_putstr(sbuf);
    screen_putstr("  |  Length: ");
    my_itoa(snake_len, sbuf);
    screen_putstr(sbuf);
    screen_putstr("  |  WASD / Arrows  |  Q quit");

    screen_flush();
}
