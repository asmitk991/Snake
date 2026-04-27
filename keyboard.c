#include "keyboard.h"
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

static struct termios orig_termios;

/* Setup terminal for non-blocking keys */
void keyboard_init(void) {
    tcgetattr(STDIN_FILENO, &orig_termios);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN]  = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    /* Set file descriptor to non-blocking */
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

/* Restore terminal settings */
void keyboard_restore(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);

    /* Remove non-blocking flag */
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
}

/* Check if a key was pressed */
int key_pressed(void) {
    unsigned char c = 0;
    int n = (int)read(STDIN_FILENO, &c, 1);
    if (n <= 0) return KEY_NONE;

    /* Handle arrow keys */
    if (c == '\033') {
        unsigned char seq[2] = {0, 0};
        read(STDIN_FILENO, &seq[0], 1);
        read(STDIN_FILENO, &seq[1], 1);
        if (seq[0] == '[') {
            if (seq[1] == 'A') return KEY_UP;
            if (seq[1] == 'B') return KEY_DOWN;
            if (seq[1] == 'C') return KEY_RIGHT;
            if (seq[1] == 'D') return KEY_LEFT;
        }
        return KEY_NONE;
    }
    return (int)c;
}

/* Wait for a full line of text */
void read_line(char *buf, int max_len) {
    /* Switch back to canonical mode */
    struct termios canonical = orig_termios;
    tcsetattr(STDIN_FILENO, TCSANOW, &canonical);
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);

    int i = 0;
    int c;
    while (i < max_len - 1) {
        c = getchar();
        if (c == '\n' || c == EOF) break;
        buf[i++] = (char)c;
    }
    buf[i] = '\0';

    /* Restore raw/non-blocking mode */
    keyboard_init();
}
