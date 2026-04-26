#include "screen.h"
#include "string.h"
#include <stdio.h>

/* Low-level escape helpers */
static void put_digits(int n) {
    char buf[8];
    my_itoa(n, buf);
    int i = 0;
    while (buf[i]) { putchar(buf[i]); i++; }
}

/* Enter alternate screen to prevent scrolling */
void screen_alt_enter(void) {
    fputs("\033[?1049h", stdout);
    fputs("\033[H",      stdout);
    fputs("\033[2J",     stdout);
    fflush(stdout);
}

void screen_alt_exit(void) {
    fputs("\033[?1049l", stdout);
    fflush(stdout);
}

void screen_clear(void) {
    /* Clear alternate screen */
    fputs("\033[H",  stdout);
    fputs("\033[2J", stdout);
    fputs("\033[H",  stdout);
}

void screen_move(int row, int col) {
    /* Move cursor */
    putchar('\033'); putchar('[');
    put_digits(row); putchar(';'); put_digits(col);
    putchar('H');
}

void screen_putchar(char c)       { putchar(c); }

void screen_putstr(const char *s) {
    int i = 0;
    while (s[i]) { putchar(s[i]); i++; }
}

void screen_flush(void) { fflush(stdout); }

/* Color */
void screen_color(int attr, int fg) {
    /* Set color attributes */
    putchar('\033'); putchar('[');
    put_digits(attr); putchar(';'); put_digits(fg);
    putchar('m');
}

void screen_color_reset(void) {
    putchar('\033'); putchar('['); putchar('0'); putchar('m');
}

void screen_putchar_colored(char c, int attr, int fg) {
    screen_color(attr, fg);
    putchar(c);
    screen_color_reset();
}

void screen_putstr_colored(const char *s, int attr, int fg) {
    screen_color(attr, fg);
    screen_putstr(s);
    screen_color_reset();
}

/* Border */
void screen_draw_border(int rows, int cols) {
    screen_color(COL_BOLD, COL_WHITE);

    /* Top */
    screen_move(2, 1); putchar('+');
    for (int c = 1; c < cols - 1; c++) putchar('-');
    putchar('+');

    /* Sides */
    for (int r = 3; r < rows; r++) {
        screen_move(r, 1);    putchar('|');
        screen_move(r, cols); putchar('|');
    }

    /* Bottom */
    screen_move(rows, 1); putchar('+');
    for (int c = 1; c < cols - 1; c++) putchar('-');
    putchar('+');

    screen_color_reset();
}
