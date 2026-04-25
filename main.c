/*
 * Phase 1:
 * Move a single character inside a static box using WASD or arrow keys.
 */

#include "math.h"
#include "memory.h"
#include "screen.h"
#include "keyboard.h"

#include <stdio.h>
#include <unistd.h>

#define BOARD_ROWS    20
#define BOARD_COLS    40

#define PLAY_ROW_MIN  2
#define PLAY_ROW_MAX  (BOARD_ROWS - 1)
#define PLAY_COL_MIN  2
#define PLAY_COL_MAX  (BOARD_COLS - 1)

#define TICK_US       130000

typedef struct Player {
    int row;
    int col;
    int dir_row;
    int dir_col;
} Player;

static Player *player = 0;

static void update_direction(int key);
static void tick(void);
static void render(void);

int main(void) {
    mem_init();
    player = (Player *)my_alloc((int)sizeof(Player));
    if (!player) {
        printf("memory allocation failed\n");
        return 1;
    }

    keyboard_init();
    printf("\033[?25l");

    player->row = PLAY_ROW_MIN + my_div(PLAY_ROW_MAX - PLAY_ROW_MIN, 2);
    player->col = PLAY_COL_MIN + my_div(PLAY_COL_MAX - PLAY_COL_MIN, 2);
    player->dir_row = 0;
    player->dir_col = 0;

    render();

    while (1) {
        int key = key_pressed();
        if (key == KEY_QUIT) break;

        update_direction(key);
        tick();
        render();
        usleep(TICK_US);
    }

    screen_move(BOARD_ROWS + 2, 1);
    screen_flush();

    printf("\033[?25h");
    keyboard_restore();
    my_dealloc(player);
    player = 0;
    return 0;
}

static void update_direction(int key) {
    if (key == KEY_UP) player->dir_row = -1, player->dir_col = 0;
    if (key == KEY_DOWN) player->dir_row = 1, player->dir_col = 0;
    if (key == KEY_LEFT) player->dir_row = 0, player->dir_col = -1;
    if (key == KEY_RIGHT) player->dir_row = 0, player->dir_col = 1;
}

static void tick(void) {
    int next_row = player->row + player->dir_row;
    int next_col = player->col + player->dir_col;

    if (!my_inbounds(next_row, PLAY_ROW_MIN, PLAY_ROW_MAX + 1) ||
        !my_inbounds(next_col, PLAY_COL_MIN, PLAY_COL_MAX + 1)) {
        return;
    }

    player->row = next_row;
    player->col = next_col;
}

static void render(void) {
    screen_clear();
    screen_draw_border(BOARD_ROWS, BOARD_COLS);

    screen_move(player->row, player->col);
    screen_putchar('@');

    screen_move(BOARD_ROWS + 1, 1);
    screen_putstr("Phase 1  |  Move: WASD / Arrows  |  Q quit");
    screen_flush();
}
