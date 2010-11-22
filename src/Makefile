OBJS = util.o qaputil.o qaplocal.o memetic.o
TARGET = qap
LIBS = -lm
MYFLAGS = -Wall
CFLAGS = -O2
CC = gcc

all: $(TARGET)

qap: $(OBJS) main.o
	$(CC) $(MYFLAGS) $(CFLAGS) $(LIBS) -o $@ $(OBJS) main.c

%.o: %.c
	$(CC) $(MYFLAGS) $(CFLAGS) -c $<

clean:
	rm -rf $(OBJS) $(TARGET) main.o
