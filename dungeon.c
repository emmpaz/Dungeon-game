#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#define ROWS 21
#define COLS 80
#define MAX_ROOMS 6
#define MAX_STAIRS 3

//struct for a room
typedef struct room{
	int rows;
   	int cols;
   	int gridRow;
   	int gridCol;
	
}room;

//struct for stairs to store where placed on grid
typedef struct stairs{
	int gridRow;
	int gridCol;
	char direction;
}stairs;

//struct for the dungeon
typedef struct dungeon{
	room Rooms[MAX_ROOMS];
	char dungeonGrid[ROWS][COLS];
	stairs Stairs[MAX_STAIRS];
}dungeon;


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
    	Dungeon.dungeonGrid[i][j] = ' ';
    }
  }
  //adding horizontal borders
  for(int i=0; i<COLS; i++){
    Dungeon.dungeonGrid[0][i] = '-';
    Dungeon.dungeonGrid[ROWS - 1][i] = '-';
  }
  //adding vertical borders
  for(int i=1; i<ROWS-1; i++){
    Dungeon.dungeonGrid[i][0] = '|';
    Dungeon.dungeonGrid[i][COLS - 1] = '|';
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
	  	if(Dungeon.dungeonGrid[randRow + i][randCol + j] != ' '){
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
	    	Dungeon.Rooms[roomsPlaced].gridRow = randRow;
	    	Dungeon.Rooms[roomsPlaced].gridCol = randCol;
	   	/*if(i==0&&j==0)
	   	  Dungeon.dungeonGrid[randRow + i][randCol + j] = (roomsPlaced+1) + '0';
	   	else	*/		
	     	  Dungeon.dungeonGrid[randRow + i][randCol + j] = '.';
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
		if(Dungeon.dungeonGrid[j][Dungeon.Rooms[i].gridCol] == ' ')
			Dungeon.dungeonGrid[j][Dungeon.Rooms[i].gridCol] = '#';
	}
	//X direction
	for(int k = Dungeon.Rooms[i].gridCol; k != Dungeon.Rooms[i+1].gridCol; k += (directionX>0) ? 1 : -1){
		if(Dungeon.dungeonGrid[Dungeon.Rooms[i].gridRow+directionY][k] == ' ')
			Dungeon.dungeonGrid[Dungeon.Rooms[i].gridRow+directionY][k] = '#';
	}
}
//this loop will place stairs randomly on the map but making sure it is on a floor
int stairsPlaced = 0;
while(stairsPlaced < MAX_STAIRS){
	int randRow = (rand() % 13) + 1;
	int randCol = (rand() % 70) + 1;
	if(Dungeon.dungeonGrid[randRow][randCol] == '.' || Dungeon.dungeonGrid[randRow][randCol] == '#'){
		Dungeon.Stairs[stairsPlaced].gridRow = randRow;
		Dungeon.Stairs[stairsPlaced].gridCol = randCol;
		//will switch between upstairs and downstairs
		if(stairsPlaced % 2 == 0){
			Dungeon.Stairs[stairsPlaced++].direction = '<';
			Dungeon.dungeonGrid[randRow][randCol] = '<';
		}
		else{
			Dungeon.Stairs[stairsPlaced++].direction = '>';
			Dungeon.dungeonGrid[randRow][randCol] = '>';
		}
		
	} 

}
//printing board
  for(int i = 0; i < ROWS; i++){
    for(int j = 0; j < COLS; j++){
	printf("%c", Dungeon.dungeonGrid[i][j]);
    }
    printf("\n");
  }
  return 0;
  
  
  
}
