#ifndef SCREEN_H
#define SCREEN_H

/* Terminal dimensions */
#define SCREEN_COLS  32
#define SCREEN_ROWS  16

/* ANSI color codes (foreground) */
#define COL_RESET    0
#define COL_RED      31
#define COL_GREEN    32
#define COL_YELLOW   33
#define COL_BLUE     34
#define COL_MAGENTA  35
#define COL_CYAN     36
#define COL_WHITE    37
#define COL_BOLD     1

/* Basic drawing */
void screen_alt_enter(void);
void screen_alt_exit (void);
void screen_clear    (void);
void screen_move     (int row, int col);
void screen_putchar  (char c);
void screen_putstr   (const char *s);
void screen_flush    (void);

/* Border & maze walls */
void screen_draw_border(int rows, int cols);

/* Color helpers */
void screen_color    (int attr, int fg);
void screen_color_reset(void);

/* Composite draw helpers */
void screen_putchar_colored(char c, int attr, int fg);
void screen_putstr_colored (const char *s, int attr, int fg);

#endif
