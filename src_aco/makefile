CC = gcc
CFLAGS = -Wall -ggdb -D DEBUG -lm
OBJS = main.o ant.o QAP.o util.o
BIN = main

all: $(OBJS)
	$(CC) -o $(BIN) $(CFLAGS) $(OBJS)

clean:
	rm $(OBJS) $(BIN)
