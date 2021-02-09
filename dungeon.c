#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#define ROWS 21
#define COLS 80
#define MAX_ROOMS 6

typedef struct room{
	int rows;
   	int cols;
	
}room;

int main(){

 srand(time(0));
 room Rooms[MAX_ROOMS];
 //random dimensions for each room in the array
 for(int i = 0; i < MAX_ROOMS; i++){
	Rooms[i].rows = (rand() % 4) + 3;
	Rooms[i].cols = (rand() % 4) + 4;
 }
  //initializing the board to white spaces
  char dungeon[ROWS][COLS];
  for(int i = 0; i < ROWS; i++){
    for(int j = 0; j < COLS; j++){
    	dungeon[i][j] = ' ';
    }
  }
  //adding horizontal borders
  for(int i=0; i<COLS; i++){
    dungeon[0][i] = '-';
    dungeon[ROWS - 1][i] = '-';
  }
  //adding vertical borders
  for(int i=1; i<ROWS-1; i++){
    dungeon[i][0] = '|';
    dungeon[i][COLS - 1] = '|';
  }
  int roomsPlaced = 0;
  int trys=0;
  //this loop should continue until all rooms are placed in free spaces
  while(roomsPlaced < 6){
  	int freeSpace = 1;
	int randRow = (rand() % 14) + 1;
	int randCol = (rand() % 71) + 1;
	printf("Starting room #%d\n", roomsPlaced+1);
	for(int i=0; i<Rooms[roomsPlaced].rows; i++){
	  for(int j=0; j<Rooms[roomsPlaced].cols; j++){
	  	if(dungeon[randRow][randCol + j] == '.' || isdigit(dungeon[randRow][randCol + j])){
	  		freeSpace = -1;
	  		printf("bad spot...restarting\n");
	  		break;
	  	}
	  }
	  if(freeSpace == -1){
	  printf("breaking...out\n");
	  	break;
	  }
	}
	
	if(freeSpace == 1){
	
	  for(int i=0; i<Rooms[roomsPlaced].rows; i++){
	    for(int j=0; j<Rooms[roomsPlaced].cols;j++){
	   	if(i==0&&j==0)
	   	  dungeon[randRow][randCol + j] = (roomsPlaced+1) + '0';
	   	else			
	     	  dungeon[randRow][randCol + j] = '.';
	    }
	    randRow++;
	  }
	  printf("room #%d placed successfully...\n", roomsPlaced+1);	
	  roomsPlaced++;
	}

}
  for(int i = 0; i < ROWS; i++){
    for(int j = 0; j < COLS; j++){
	printf("%c", dungeon[i][j]);
    }
    printf("\n");
  }
  return 0;
  
}
