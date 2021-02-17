#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <endian.h>
#include <string.h>
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
	int8_t gridRow;
	int8_t gridCol;
	char playerChar;
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
  dungeon Dungeon;
  Dungeon.pc.playerChar = '@';
  
  //start of load
  //creating path 
  char* home = getenv("HOME");
  char* gameDir = ".rlg327";
  char* saveDir = "dungeon";
  char* path = malloc((strlen(home) + strlen(gameDir) + strlen(saveDir) + 3) * sizeof(char));
  sprintf(path, "%s/%s/%s", home, gameDir, saveDir);

  //opening save file
  FILE *saveFile;
  if((saveFile = fopen(path, "r")) == NULL) {
    saveFile = fopen(path, "w");
    fclose(saveFile);
    if((saveFile = fopen(path, "r")) == NULL) {
      printf("Error opening save file");
      exit(1);
    }
  }
  
  //init vars
  char name[13];
  name[12] = '\0';
  int version, size;
  int16_t numberOfRooms, numberOfUpStairs, numberOfDownStairs;
  int8_t** roomCords; //roomCords[roomIndex][(0=x top left pos, 1=y top left pos, 2=x size(width), 3=y size(height)]
  int8_t** upStairCords; //upStairCords[stairIndex][(0=x pos, 1=y pos)]
  int8_t** downStairCords; //downStairCords[stairIndex][(0=x pos, 1=y pos)]
  
  //temp *******could break code if nothing is read in
  int8_t hardness[ROWS][COLS]; 
  for(int i = 0; i<ROWS; i++) {
    for(int j = 0; j<COLS; j++) {
      hardness[i][j] = 0;
    }
  }
  
  //checking if has min bytes for needed for data to be read in
  fseek(saveFile, 0, SEEK_END);
  if(ftell(saveFile)  > 1708) {
    fseek(saveFile, 0, SEEK_SET);
    //reading in data
    fread(name, sizeof(char), 12, saveFile);
    fread(&version, sizeof(int), 1, saveFile);
    version = be32toh(version);
    fread(&size, sizeof(int), 1, saveFile);
    size = be32toh(size);
    fread(&Dungeon.pc.gridRow, sizeof(int8_t), 1, saveFile);
    fread(&Dungeon.pc.gridCol, sizeof(int8_t), 1, saveFile);
    fread(hardness, sizeof(int8_t), 1680, saveFile);
    fread(&numberOfRooms, sizeof(int16_t), 1, saveFile);
    numberOfRooms = be16toh(numberOfRooms);
    roomCords = malloc(sizeof(int8_t *) * numberOfRooms);
    for(int i = 0; i < numberOfRooms; i++) roomCords[i] = malloc(4 * sizeof(int8_t));
    fread(roomCords, sizeof(int8_t), numberOfRooms * 4, saveFile);
    fread(&numberOfUpStairs, sizeof(int16_t), 1, saveFile);
    numberOfUpStairs = be16toh(numberOfUpStairs);
    upStairCords = malloc(sizeof(int8_t *) * numberOfUpStairs);
    for(int i = 0; i < numberOfUpStairs; i++) upStairCords[i] = malloc(2 * sizeof(int8_t));
    fread(upStairCords, sizeof(int8_t), numberOfUpStairs * 2, saveFile);
    fread(&numberOfDownStairs, sizeof(int16_t), 1, saveFile);
    numberOfDownStairs = be16toh(numberOfDownStairs);
    downStairCords = malloc(sizeof(int8_t *) * numberOfDownStairs);
    for(int i = 0; i < numberOfDownStairs; i++) downStairCords[i] = malloc(2*sizeof(int8_t));
    fread(downStairCords, sizeof(int8_t), numberOfDownStairs * 2, saveFile);
  }
  fclose(saveFile);
  //end of load
  
  //debug
  //printf("load size: %i\n", size);
  //printf("calculated load size: %i\n", 12 + 4 + 4 + 2 + 1680 + 2 + numberOfRooms * 4 + 2 + numberOfUpStairs * 2 + 2 + numberOfDownStairs * 2);
  
 srand(time(0));
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
	    	  Dungeon.pc.gridRow = randRow;
	    	  Dungeon.pc.gridCol = randCol;
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
  //start of save
  if((saveFile = fopen(path, "w")) == NULL) {
    printf("Error opening save file");
    exit(1);
  }
  
  //formating for write vars. new vars likly not needed. the function calls "be#toh()" need to be called on vars befor writing. watch out as lots of int8_t = int
  
  //hardness
    for(int i = 0; i<ROWS; i++) {
      for(int j = 0; j<COLS; j++) {
        hardness[i][j] = Dungeon.dungeonGrid[i][j]; // think this converts from int32_t to in8_t
    }
  }
  //rooms
  numberOfRooms = sizeof(Dungeon.Rooms) / sizeof(room); //hopefully gets length of dungeon rooms array
  //printf("rooms: %i\n", numberOfRooms); //debug
  free(roomCords);
  roomCords = malloc(sizeof(int8_t *) * numberOfRooms);
  for(int i = 0; i < numberOfRooms; i++) roomCords[i] = malloc(4 * sizeof(int8_t));
  for(int i = 0; i<numberOfRooms; i++) {
    roomCords[i][0] = Dungeon.Rooms[i].gridRow;
    roomCords[i][1] = Dungeon.Rooms[i].gridCol;
    roomCords[i][2] = Dungeon.Rooms[i].rows;
    roomCords[i][3] = Dungeon.Rooms[i].cols;
  }
  
  //stairs
  //printf("stairs: %i\n", sizeof(Dungeon.Stairs) / sizeof(stairs)); //debug
  numberOfUpStairs = 0;
  numberOfDownStairs = 0;
  for(int i = 0; i<sizeof(Dungeon.Stairs) / sizeof(stairs); i++) { //hopefully gets length of dungeon stairs array
    if(Dungeon.Stairs[i].direction != '\0') {
      if(Dungeon.Stairs[i].direction == '<') {
        numberOfUpStairs ++;
      } else if(Dungeon.Stairs[i].direction == '>') {
        numberOfDownStairs ++;
      }
    }
  }
  free(upStairCords);
  free(downStairCords);
  upStairCords = malloc(sizeof(int8_t *) * numberOfUpStairs + 1);
  for(int i = 0; i < numberOfUpStairs + 1; i++) upStairCords[i] = malloc(2 * sizeof(int8_t));
  downStairCords = malloc(sizeof(int8_t *) * numberOfDownStairs + 1);
  for(int i = 0; i < numberOfDownStairs + 1; i++) downStairCords[i] = malloc(2*sizeof(int8_t));
  for(int i = 0; i<sizeof(Dungeon.Stairs) / sizeof(stairs); i++) {
    if(Dungeon.Stairs[i].direction != '\0') {
      if(Dungeon.Stairs[i].direction == '<') {
        upStairCords[i][0] = Dungeon.Stairs[i].gridRow;
        upStairCords[i][1] = Dungeon.Stairs[i].gridCol;
      } else if(Dungeon.Stairs[i].direction == '>') {
        downStairCords[i][0] = Dungeon.Stairs[i].gridRow;
        downStairCords[i][1] = Dungeon.Stairs[i].gridCol;
      }
    }
  }
  size = 12 + 4 + 4 + 2 + 1680 + 2 + numberOfRooms * 4 + 2 + numberOfUpStairs * 2 + 2 + numberOfDownStairs * 2;
  //printf("save size: %i\n", size); //debug
  
  //writing data
  fwrite(name, sizeof(char), 12, saveFile);
  version = htobe32(version);
  fwrite(&version, sizeof(int), 1, saveFile);
  size = htobe32(size);
  fwrite(&size, sizeof(int), 1, saveFile); //todo
  fwrite(&Dungeon.pc.gridRow, sizeof(int8_t), 1, saveFile);
  fwrite(&Dungeon.pc.gridCol, sizeof(int8_t), 1, saveFile);
  fwrite(hardness, sizeof(int8_t), 1680, saveFile);
  numberOfRooms = htobe16(numberOfRooms);
  fwrite(&numberOfRooms, sizeof(int16_t), 1, saveFile);
  numberOfRooms = be16toh(numberOfRooms);
  fwrite(roomCords, sizeof(int8_t), numberOfRooms * 4, saveFile);
  numberOfUpStairs = htobe16(numberOfUpStairs);
  fwrite(&numberOfUpStairs, sizeof(int16_t), 1, saveFile);
  numberOfUpStairs = be16toh(numberOfUpStairs);
  fwrite(upStairCords, sizeof(int8_t), numberOfUpStairs * 2, saveFile);
  numberOfDownStairs = htobe16(numberOfDownStairs);
  fwrite(&numberOfDownStairs, sizeof(int16_t), 1, saveFile);
  numberOfDownStairs = htobe16(numberOfDownStairs);
  fwrite(downStairCords, sizeof(int8_t), numberOfDownStairs * 2, saveFile);
  fclose(saveFile);
  //end of save
  
  //debug
  //printf("calculated save size: %i\n", 12 + 4 + 4 + 2 + 1680 + 2 + numberOfRooms * 4 + 2 + numberOfUpStairs * 2 + 2 + numberOfDownStairs * 2);

  printBoard(dungeonChar);

  return 0;
}
