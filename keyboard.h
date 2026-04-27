#ifndef KEYBOARD_H
#define KEYBOARD_H

/* Keyboard input */

/* Key codes */
#define KEY_UP     'w'
#define KEY_DOWN   's'
#define KEY_LEFT   'a'
#define KEY_RIGHT  'd'
#define KEY_QUIT   'q'
#define KEY_NONE    0

void keyboard_init(void);
void keyboard_restore(void);
int  key_pressed(void);
void read_line(char *buf, int max_len);

#endif
