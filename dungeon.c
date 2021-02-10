#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#define ROWS 21
#define COLS 80
#define MAX_ROOMS 6

//struct for a room
typedef struct room{
	int rows;
   	int cols;
   	int gridRow;
   	int gridCol;
	
}room;

//struct for the dungeon
typedef struct dungeon{
	room Rooms[MAX_ROOMS];
	char dungeonGrid[ROWS][COLS];

}dungeon;

int main(){

 srand(time(0));
 dungeon Dungeon;
 //random dimensions for each room in the array
 for(int i = 0; i < MAX_ROOMS; i++){
	Dungeon.Rooms[i].rows = (rand() % 4) + 3;
	Dungeon.Rooms[i].cols = (rand() % 4) + 4;
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
  while(roomsPlaced < 6){
  	int freeSpace = 1;
	int randRow = (rand() % 14) + 1;
	int randCol = (rand() % 71) + 1;
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
	   	if(i==0&&j==0)
	   	  Dungeon.dungeonGrid[randRow + i][randCol + j] = (roomsPlaced+1) + '0';
	   	else			
	     	  Dungeon.dungeonGrid[randRow + i][randCol + j] = '.';
	    }
	  }
	  roomsPlaced++;
	}

}

	int x1=-1;
	int x2=-1;
	int y1=-1;
	int y2=-1;
	for(int i=0; i<ROWS; i++)
	{
		for(int j = 0; j < COLS; j++){
			if(isdigit(Dungeon.dungeonGrid[i][j]))
			{
				x2=x1;
				y2=y1;
				x1=i;
				y1=j;
				if(x2==-1)
					break;
				printf("%d %d %d %d\n",x1,x2,y1,y2);
				int k=0;
				if(x1>x2)
				{
				while(x1-k>x2)
				{
					if(Dungeon.dungeonGrid[x1-k][y1] == ' ')
					{
						Dungeon.dungeonGrid[x1-k][y1]='#';
					}
					k++;
				}
				k=x1-k;
				}
				else
				{
					while(x1+k<x2)
					{
						if(Dungeon.dungeonGrid[x1+k][y1] == ' ')
						{
							Dungeon.dungeonGrid[x1+k][y1]='#';
						}
						k++;
					}
				k=x1+k;
				}
				int g=0;
				if(y1<y2)
				{
					while(y1+g<y2)
					{
						if(Dungeon.dungeonGrid[k][y1+g] == ' ')
						{
							Dungeon.dungeonGrid[k][y1+g]='#';
						}
						g++;
					}
				}
				else
				{
					while(y1-g>y2)
					{
						if(Dungeon.dungeonGrid[k][y1-g] == ' ')
						{
							Dungeon.dungeonGrid[k][y1-g]='#';
						}
						g++;
					}
				}
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