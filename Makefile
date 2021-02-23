OBJS	= dungeon.o heap.o
SOURCE	= dungeon.c heap.c
HEADER	= heap.h
OUT	= dungeon
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

dungeon.o: dungeon.c
	$(CC) $(FLAGS) dungeon.c 

heap.o: heap.c
	$(CC) $(FLAGS) heap.c 


clean:
	rm -f $(OBJS) $(OUT)
