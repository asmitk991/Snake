/*
 * Snake Game
 */

#include "math.h"
#include "memory.h"
#include "screen.h"
#include "keyboard.h"
#include "string.h"
#include "game.h"
#include "scores.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

/* Game state machine */
#define STATE_HOME      0
#define STATE_PLAYING   1
#define STATE_GAMEOVER  2

/* Tick speed per mode (microseconds) */
static const int TICK_US[3] = { 200000, 150000, 110000 };

/* Global game variables */
static char  player_name[NAME_LEN];
static int   current_mode  = MODE_MEDIUM;
static int   current_score = 0;
static int   high_score    = 0;
static int   game_state    = STATE_HOME;

static Snake snake;
static Food  food;

/* Forward declarations */
static void do_home      (void);
static void do_game      (void);
static void do_gameover  (void);
static void render_header(void);
static void render_game  (void);
static void render_hard_walls(void);

/* Home screen functions */
static void draw_home(void) {
    screen_clear();

    /* Title banner */
    screen_move(3, 5);
    screen_putstr_colored("╔══════════════════════╗", COL_BOLD, COL_GREEN);
    screen_move(4, 5);
    screen_putstr_colored("║   S N A K E   2 . 0  ║", COL_BOLD, COL_GREEN);
    screen_move(5, 5);
    screen_putstr_colored("╚══════════════════════╝", COL_BOLD, COL_GREEN);

    /* Mode indicator */
    const char *mnames[3] = { "Easy", "Med", "Hard" };
    screen_move(8, 7);
    screen_putstr_colored("Diff: ", COL_BOLD, COL_WHITE);
    int col = (current_mode == MODE_EASY)   ? COL_GREEN  :
              (current_mode == MODE_MEDIUM) ? COL_YELLOW : COL_RED;
    screen_putstr_colored(mnames[current_mode], COL_BOLD, col);

    screen_move(10, 3);
    screen_putstr_colored("[E] Easy [M] Med [H] Hard", 0, COL_CYAN);

    /* Divider */
    screen_move(12, 3);
    screen_putstr_colored("──────────────────────────", 0, COL_WHITE);

    screen_move(14, 3);
    screen_putstr_colored("Name: ", COL_BOLD, COL_YELLOW);

    screen_flush();
}

static void do_home(void) {
    /* Phase 1: pick mode, then enter name */
    while (1) {
        draw_home();

        /* Show cursor temporarily for name input */
        printf("\033[?25h");
        screen_flush();

        /* Non-blocking mode selection loop */
        while (1) {
            int k = key_pressed();
            if (k == 'e' || k == 'E') { current_mode = MODE_EASY;   draw_home(); }
            if (k == 'm' || k == 'M') { current_mode = MODE_MEDIUM; draw_home(); }
            if (k == 'h' || k == 'H') { current_mode = MODE_HARD;   draw_home(); }
            if (k == '\n' || k == '\r') break;
            /* Let screen_move position the cursor for input */
            screen_move(14, 9);
            screen_flush();
            usleep(50000);
        }

        /* Blocking name read */
        screen_move(14, 9);
        screen_flush();
        read_line(player_name, NAME_LEN);

        /* Validate name: must be non-empty */
        if (my_strlen(player_name) > 0) break;
    }

    printf("\033[?25l");   /* hide cursor again */
    high_score    = scores_get_high(current_mode);
    current_score = 0;
    game_init(&snake, &food, current_mode);
    game_state = STATE_PLAYING;
}

/* Playing state functions */
static void render_header(void) {
    /* Row 1: player name | score | high score | mode */
    screen_move(1, 1);
    screen_color(COL_BOLD, COL_WHITE);

    const char *mnames[3] = { "EZ", "MED", "HRD" };
    int mcol[3]           = { COL_GREEN, COL_YELLOW, COL_RED };

    screen_putstr("P:");
    screen_color(COL_BOLD, COL_CYAN);
    
    /* Truncate player name to 5 chars to fit */
    char tname[6];
    int i=0;
    while(player_name[i] && i<5) { tname[i] = player_name[i]; i++; }
    tname[i] = '\0';
    screen_putstr(tname);

    screen_color(COL_BOLD, COL_WHITE);
    screen_putstr(" S:");
    screen_color(COL_BOLD, COL_YELLOW);
    char sbuf[12]; my_itoa(current_score, sbuf); screen_putstr(sbuf);

    screen_color(COL_BOLD, COL_WHITE);
    screen_putstr(" B:");
    screen_color(COL_BOLD, COL_GREEN);
    my_itoa(high_score, sbuf); screen_putstr(sbuf);

    screen_color(COL_BOLD, COL_WHITE);
    screen_putstr(" M:");
    screen_color(COL_BOLD, mcol[current_mode]);
    screen_putstr(mnames[current_mode]);

    screen_color_reset();
}

static void render_hard_walls(void) {
    screen_color(COL_BOLD, COL_MAGENTA);
    for (int i = 0; i < hard_wall_count; i++) {
        screen_move(hard_walls[i].row, hard_walls[i].col);
        putchar('#');
    }
    screen_color_reset();
}

static void render_game(void) {
    screen_clear();
    render_header();
    screen_draw_border(BOARD_ROWS, BOARD_COLS);

    if (current_mode == MODE_HARD)
        render_hard_walls();

    /* Food */
    if (food.active) {
        screen_move(food.row, food.col);
        screen_putchar_colored('*', COL_BOLD, COL_YELLOW);
    }

    /* Snake body */
    screen_color(0, COL_RED);
    for (SnakeNode *n = snake.head; n; n = n->next) {
        screen_move(n->row, n->col);
        putchar('o');
    }

    /* Snake head */
    screen_move(snake.head->row, snake.head->col);
    screen_putchar_colored('@', COL_BOLD, COL_RED);

    screen_color_reset();

    /* Footer hint */
    screen_move(BOARD_ROWS + 1, 2);
    if (snake.dir_row == 0 && snake.dir_col == 0)
        screen_putstr_colored("Press an ARROW KEY to start!", COL_BOLD, COL_YELLOW);
    else
        screen_putstr_colored("Q Quit", 0, COL_WHITE);

    screen_flush();
}

static void do_game(void) {
    int key = key_pressed();

    /* Direction update */
    if (key == KEY_UP    && snake.dir_row != 1)  { snake.dir_row = -1; snake.dir_col = 0; }
    if (key == KEY_DOWN  && snake.dir_row != -1) { snake.dir_row =  1; snake.dir_col = 0; }
    if (key == KEY_LEFT  && snake.dir_col != 1)  { snake.dir_row =  0; snake.dir_col = -1; }
    if (key == KEY_RIGHT && snake.dir_col != -1) { snake.dir_row =  0; snake.dir_col = 1; }

    if (key == KEY_QUIT) {
        /* Force game over */
        scores_add(player_name, current_score, current_mode);
        high_score = scores_get_high(current_mode);
        game_state = STATE_GAMEOVER;
        return;
    }

    int dead = game_tick(&snake, &food, current_mode);

    if (dead) {
        scores_add(player_name, current_score, current_mode);
        high_score = scores_get_high(current_mode);
        game_state = STATE_GAMEOVER;
        return;
    }

    /* Score: +10 per food eaten */
    current_score = (snake.length - 1) * 10;
    if (current_score > high_score) high_score = current_score;

    render_game();
    
    /* Terminal characters are roughly twice as tall as they are wide.
       To make vertical and horizontal speeds feel the same visually,
       we sleep longer when moving vertically. */
    int delay = TICK_US[current_mode];
    if (snake.dir_row != 0) {
        delay = delay + my_div(delay, 2); /* x 1.5 for vertical movement */
    }
    usleep(delay);
}

/* Game over screen functions */
static void render_gameover(void) {
    screen_clear();

    /* Big GAME OVER banner */
    screen_move(3, 20);
    screen_putstr_colored("╔═══════════════════╗", COL_BOLD, COL_RED);
    screen_move(4, 20);
    screen_putstr_colored("║    G A M E  O V E R    ║", COL_BOLD, COL_RED);
    screen_move(5, 20);
    screen_putstr_colored("╚═══════════════════╝", COL_BOLD, COL_RED);

    /* Player + score */
    screen_move(7, 20);
    screen_putstr_colored("Player : ", COL_BOLD, COL_WHITE);
    screen_putstr_colored(player_name, COL_BOLD, COL_CYAN);

    screen_move(8, 20);
    screen_putstr_colored("Score  : ", COL_BOLD, COL_WHITE);
    char sbuf[12]; my_itoa(current_score, sbuf);
    screen_putstr_colored(sbuf, COL_BOLD, COL_YELLOW);

    /* Divider */
    screen_move(10, 16);
    screen_putstr_colored("─────── TOP SCORERS ───────", COL_BOLD, COL_GREEN);

    /* Top 3 leaderboard */
    ScoreEntry top[3]; int tc = 0;
    scores_get_top3(top, &tc, current_mode);

    const char *medals[3] = { "1.", "2.", "3." };
    int mcols[3]          = { COL_YELLOW, COL_WHITE, COL_RED };

    for (int i = 0; i < tc; i++) {
        screen_move(11 + i, 16);
        screen_putstr_colored(medals[i], COL_BOLD, mcols[i]);
        screen_putstr(" ");
        screen_putstr_colored(top[i].name, 0, COL_CYAN);

        /* Pad name to 12 chars */
        int pad = 12 - my_strlen(top[i].name);
        for (int p = 0; p < pad; p++) putchar(' ');

        my_itoa(top[i].score, sbuf);
        screen_putstr_colored(sbuf, COL_BOLD, COL_YELLOW);
    }

    /* Options */
    screen_move(16, 19);
    screen_putstr_colored("[R] Restart   [Q] Quit to Menu", COL_BOLD, COL_WHITE);

    screen_flush();
}

static void do_gameover(void) {
    render_gameover();

    /* Blocking wait for R or Q */
    while (1) {
        int k = key_pressed();
        if (k == 'r' || k == 'R') {
            /* Restart */
            current_score = 0;
            game_free_snake(&snake);
            game_init(&snake, &food, current_mode);
            game_state = STATE_PLAYING;
            return;
        }
        if (k == 'q' || k == 'Q') {
            /* Return to home */
            game_free_snake(&snake);
            game_state = STATE_HOME;
            return;
        }
        usleep(50000);
    }
}

/* Main */
int main(void) {
    mem_init();
    scores_load();
    srand((unsigned int)time(0));

    keyboard_init();
    screen_alt_enter();
    printf("\033[?25l"); /* Hide cursor */
    screen_flush();
    game_state = STATE_HOME;

    while (1) {
        if      (game_state == STATE_HOME)     do_home();
        else if (game_state == STATE_PLAYING)  do_game();
        else if (game_state == STATE_GAMEOVER) do_gameover();
        else break;
    }

    /* Cleanup */
    game_free_snake(&snake);
    screen_move(BOARD_ROWS + 3, 1);
    screen_flush();
    printf("\033[?25h");
    screen_alt_exit();
    keyboard_restore();
    return 0;
}
