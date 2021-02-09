#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
 for(int i = 0; i < MAX_ROOMS; i++){
	Rooms[i].rows = (rand() % 4) + 3;
	Rooms[i].cols = (rand() % 4) + 4;
 }

  char dungeon[ROWS][COLS];
  for(int i = 0; i < ROWS; i++){
    for(int j = 0; j < COLS; j++){
    	dungeon[i][j] = ' ';
    }
  }
  
  for(int i=0; i<COLS; i++){
    dungeon[0][i] = '-';
    dungeon[ROWS - 1][i] = '-';
  }
  
  for(int i=1; i<ROWS-1; i++){
    dungeon[i][0] = '|';
    dungeon[i][COLS - 1] = '|';
  }
  bool complete = false;
  bool done= false;
  int roomsPlaced = 0;
	  	
	  	for(int k=0; k<MAX_ROOMS;k++){
	  		int randRow = (rand() % 15) + 1;
	  		int randCol = (rand() % 71) + 1;
			for(int i=0; i<Rooms[k].rows; i++){
			  	for(int j=0; j<Rooms[k].cols;j++){
			  			
		  			dungeon[randRow][randCol + j] = '.';
		  		}
		  		randRow++;
		  	}
		  	roomsPlaced++;	
	  	}

  for(int i = 0; i < ROWS; i++){
    for(int j = 0; j < COLS; j++){
	printf("%c", dungeon[i][j]);
    }
    printf("\n");
  }
  return 0;
  
}
