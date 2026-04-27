CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE

SRCS    = main.c math.c string.c memory.c screen.c keyboard.c game.c scores.c
OBJS    = $(SRCS:.c=.o)
TARGET  = snake

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET) scores.dat
