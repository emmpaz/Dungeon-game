#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#define ROWS 21
#define COLS 80
#define MAX_ROOMS 6
#define MAX_STAIRS 3
#define BORDER_HARDNESS 255
#define ROCK_HARDNESS 250
#define FLOOR_HARDNESS 0

//struct for a room
typedef struct room{
	int rows;
   	int cols;
   	int gridRow;
   	int gridCol;
}room;

//player character
typedef struct PC{
	int gridRow;
	int gridCol;
	char player = '@';

}PC;

//struct for stairs to store where placed on grid
typedef struct stairs{
	int gridRow;
	int gridCol;
	char direction;
}stairs;

//struct for the dungeon
typedef struct dungeon{
	room Rooms[MAX_ROOMS];
	int dungeonGrid[ROWS][COLS];
	stairs Stairs[MAX_STAIRS];
	PC pc;
}dungeon;

void printBoard(char board[ROWS][COLS]){
	for(int i = 0; i < ROWS;i++){
	  for(int j = 0; j < COLS; j++){
	  	printf("%c", board[i][j]);
	  }
	  printf("\n");
	}
}

int main(){

 srand(time(0));
 dungeon Dungeon;
 //random dimensions for each room in the array
 for(int i = 0; i < MAX_ROOMS; i++){
	Dungeon.Rooms[i].rows = (rand() % 4) + 4;
	Dungeon.Rooms[i].cols = (rand() % 4) + 5;
 }
  //initializing the board to white spaces
  for(int i = 0; i < ROWS; i++){
    for(int j = 0; j < COLS; j++){
    	Dungeon.dungeonGrid[i][j] = ROCK_HARDNESS;
    }
  }

  //adding horizontal borders
  for(int i=0; i<COLS; i++){
    Dungeon.dungeonGrid[0][i] = BORDER_HARDNESS;
    Dungeon.dungeonGrid[ROWS - 1][i] = BORDER_HARDNESS;
  }
  //adding vertical borders
  for(int i=1; i<ROWS-1; i++){
    Dungeon.dungeonGrid[i][0] = BORDER_HARDNESS;
    Dungeon.dungeonGrid[i][COLS - 1] = BORDER_HARDNESS;
  }

  int roomsPlaced = 0;
  int trys=0;
  //this loop should continue until all rooms are placed in free spaces
  while(roomsPlaced < MAX_ROOMS){
  	int freeSpace = 1;
	int randRow = (rand() % 13) + 1;
	int randCol = (rand() % 70) + 1;
	//checking if room + border will fit
	for(int i=-1; i<Dungeon.Rooms[roomsPlaced].rows+1; i++){
	  for(int j=-1; j<Dungeon.Rooms[roomsPlaced].cols+1; j++){
	  	if(Dungeon.dungeonGrid[randRow + i][randCol + j] != ROCK_HARDNESS){
	  		freeSpace = -1;
	  		break;
	  	}
	  }
	  if(freeSpace == -1){
	  	break;
	  }
	}
	//if fits it will place
	if(freeSpace == 1){
	  for(int i=0; i<Dungeon.Rooms[roomsPlaced].rows; i++){
	    for(int j=0; j<Dungeon.Rooms[roomsPlaced].cols;j++){
	    	//placing player in first room top left
	    	if(roomsPlaced == 0){
	    	  pc.gridRow = randRow;
	    	  pc.gridCol = randCol;
	    	}
	    	Dungeon.Rooms[roomsPlaced].gridRow = randRow;
	    	Dungeon.Rooms[roomsPlaced].gridCol = randCol;		
	     	Dungeon.dungeonGrid[randRow + i][randCol + j] = 0;
	    }
	  }
	  roomsPlaced++;
	}
}
//this loops through each room and its neigbor in the array to connect them together with corridors
for(int i = 0; i < MAX_ROOMS-1;i++){
        //finds the direction in which a room finds its neighboor room
	int directionY = (Dungeon.Rooms[i+1].gridRow - Dungeon.Rooms[i].gridRow);
	int directionX = (Dungeon.Rooms[i+1].gridCol - Dungeon.Rooms[i].gridCol);
	
	//Y direction
	for(int j = Dungeon.Rooms[i].gridRow; j != Dungeon.Rooms[i+1].gridRow; j += (directionY>0) ? 1 : -1){
		if(Dungeon.dungeonGrid[j][Dungeon.Rooms[i].gridCol] == ROCK_HARDNESS)
			Dungeon.dungeonGrid[j][Dungeon.Rooms[i].gridCol] = 0;
	}
	//X direction
	for(int k = Dungeon.Rooms[i].gridCol; k != Dungeon.Rooms[i+1].gridCol; k += (directionX>0) ? 1 : -1){
		if(Dungeon.dungeonGrid[Dungeon.Rooms[i].gridRow+directionY][k] == ROCK_HARDNESS)
			Dungeon.dungeonGrid[Dungeon.Rooms[i].gridRow+directionY][k] = 0;
	}
}
//this loop will place stairs randomly on the map but making sure it is on a floor
int stairsPlaced = 0;
while(stairsPlaced < MAX_STAIRS){
	int randRow = (rand() % 13) + 1;
	int randCol = (rand() % 70) + 1;
	if(Dungeon.dungeonGrid[randRow][randCol] == FLOOR_HARDNESS){
		Dungeon.Stairs[stairsPlaced].gridRow = randRow;
		Dungeon.Stairs[stairsPlaced].gridCol = randCol;
		//will switch between upstairs and downstairs
		if(stairsPlaced % 2 == 0){
			Dungeon.Stairs[stairsPlaced++].direction = '<';
			Dungeon.dungeonGrid[randRow][randCol] = 0;
		}
		else{
			Dungeon.Stairs[stairsPlaced++].direction = '>';
			Dungeon.dungeonGrid[randRow][randCol] = 0;
		}
		
	} 

}

char dungeonChar[ROWS][COLS];

//declaring char dungeon
for(int i = 0; i < ROWS; i++){
    for(int j = 0; j < COLS; j++){
    	if(Dungeon.dungeonGrid[i][j] == FLOOR_HARDNESS){
    		dungeonChar[i][j] = '#';
    	}
    	else if(Dungeon.dungeonGrid[i][j] == BORDER_HARDNESS){
    		if(i == 0 || i == ROWS-1)
    		  dungeonChar[i][j] = '-';
    		else
    		  dungeonChar[i][j] = '|';
    	}
    	else{
    		dungeonChar[i][j] = ' ';
    	}
    }
  }
 
//placing rooms on char dungeon 
for(int i = 0; i < MAX_ROOMS; i++){
  for(int j = 0; j<Dungeon.Rooms[i].rows;j++){
  	for(int k = 0; k<Dungeon.Rooms[i].cols;k++){
  		dungeonChar[Dungeon.Rooms[i].gridRow+j][Dungeon.Rooms[i].gridCol+k] = '.';
  	}
  }
}

//placing stairs on char dungeon
for(int i = 0; i < MAX_STAIRS; i++){
  dungeonChar[Dungeon.Stairs[i].gridRow][Dungeon.Stairs[i].gridCol] = Dungeon.Stairs[i].direction;
}


printBoard(dungeonChar);


  return 0;
  
  
  
}
