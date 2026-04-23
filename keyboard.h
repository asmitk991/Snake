#ifndef KEYBOARD_H
#define KEYBOARD_H

/* keyboard input setup */

/* Direction key codes returned by key_pressed() */
#define KEY_UP     'w'
#define KEY_DOWN   's'
#define KEY_LEFT   'a'
#define KEY_RIGHT  'd'
#define KEY_QUIT   'q'
#define KEY_NONE    0

void keyboard_init(void);    /* set terminal to raw/non-blocking mode  */
void keyboard_restore(void); /* restore terminal on exit               */
int  key_pressed(void);      /* non-blocking: returns key code or KEY_NONE */
void read_line(char *buf, int max_len);  /* blocking full-line read */

#endif
