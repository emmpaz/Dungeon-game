OBJS	= dungeon.o heap.o dice.o object_descriptions.o
SOURCE	= dungeon.cpp heap.c
HEADER	= heap.h
OUT	= dungeon
CC	 = g++
FLAGS	 = -g -c -Wall
LFLAGS	 = -lncurses

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

dungeon.o: dungeon.cpp
	$(CC) $(FLAGS) dungeon.cpp 

heap.o: heap.c
	$(CC) $(FLAGS) heap.c 


clean:
	rm -f $(OBJS) $(OUT)
