#ifndef SCREEN_H
#define SCREEN_H

/* drawing to the terminal */

#define SCREEN_COLS  40
#define SCREEN_ROWS  20

void screen_clear(void);
void screen_move(int row, int col);        /* position cursor */
void screen_putchar(char c);
void screen_putstr(const char *s);
void screen_draw_border(int rows, int cols);
void screen_flush(void);                   /* fflush wrapper */

#endif
