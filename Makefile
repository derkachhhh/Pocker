
CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = poker_game
SRCS = main.c deck.c hand_evaluation.c montecarlo.c utils.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET) $(OBJS)
