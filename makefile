CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -g -O2
LDFLAGS = -lpthread -lncurses -lavcodec -lavformat -lavutil -lswscale

SRC = src/main.c \
    src/video.c \
    src/ascii.c \
    src/ncurses_renderer.c \
    src/network.c

OBJ = $(SRC:.c=.o)
BIN = ascii_video_call

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJ) $(BIN)