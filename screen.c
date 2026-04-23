#include "screen.h"
#include "string.h"
#include <stdio.h>   /* allowed: terminal I/O */

/* stuff for terminal control */

static void put_escape(const char *seq) {
    putchar('\033');
    putchar('[');
    int i = 0;
    while (seq[i] != '\0') { putchar(seq[i]); i++; }
}

void screen_clear(void) {
    put_escape("2J");       /* wipe the screen */
    put_escape("H");        /* cursor to top left */
}

/* jump to specific row/col */
void screen_move(int row, int col) {
    /* build the sequence manually */
    char rbuf[8], cbuf[8];
    my_itoa(row, rbuf);
    my_itoa(col, cbuf);

    putchar('\033');
    putchar('[');
    int i = 0;
    while (rbuf[i]) { putchar(rbuf[i]); i++; }
    putchar(';');
    i = 0;
    while (cbuf[i]) { putchar(cbuf[i]); i++; }
    putchar('H');
}

void screen_putchar(char c) {
    putchar(c);
}

void screen_putstr(const char *s) {
    int i = 0;
    while (s[i] != '\0') { putchar(s[i]); i++; }
}

/* draw the outer box */
void screen_draw_border(int rows, int cols) {
    /* Top edge */
    screen_move(1, 1);
    putchar('+');
    for (int c = 1; c < cols - 1; c++) putchar('-');
    putchar('+');

    /* Side edges */
    for (int r = 2; r < rows; r++) {
        screen_move(r, 1);       putchar('|');
        screen_move(r, cols);    putchar('|');
    }

    /* Bottom edge */
    screen_move(rows, 1);
    putchar('+');
    for (int c = 1; c < cols - 1; c++) putchar('-');
    putchar('+');
}

void screen_flush(void) {
    fflush(stdout);
}
