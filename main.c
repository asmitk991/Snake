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

static int player_row = 0;
static int player_col = 0;
static int dir_row = 0;
static int dir_col = 0;

static void update_direction(int key);
static void tick(void);
static void render(void);

int main(void) {
    mem_init();
    keyboard_init();
    printf("\033[?25l");

    player_row = PLAY_ROW_MIN + my_div(PLAY_ROW_MAX - PLAY_ROW_MIN, 2);
    player_col = PLAY_COL_MIN + my_div(PLAY_COL_MAX - PLAY_COL_MIN, 2);

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
    return 0;
}

static void update_direction(int key) {
    if (key == KEY_UP) dir_row = -1, dir_col = 0;
    if (key == KEY_DOWN) dir_row = 1, dir_col = 0;
    if (key == KEY_LEFT) dir_row = 0, dir_col = -1;
    if (key == KEY_RIGHT) dir_row = 0, dir_col = 1;
}

static void tick(void) {
    int next_row = player_row + dir_row;
    int next_col = player_col + dir_col;

    if (!my_inbounds(next_row, PLAY_ROW_MIN, PLAY_ROW_MAX + 1) ||
        !my_inbounds(next_col, PLAY_COL_MIN, PLAY_COL_MAX + 1)) {
        return;
    }

    player_row = next_row;
    player_col = next_col;
}

static void render(void) {
    screen_clear();
    screen_draw_border(BOARD_ROWS, BOARD_COLS);

    screen_move(player_row, player_col);
    screen_putchar('@');

    screen_move(BOARD_ROWS + 1, 1);
    screen_putstr("Phase 1  |  Move: WASD / Arrows  |  Q quit");
    screen_flush();
}
