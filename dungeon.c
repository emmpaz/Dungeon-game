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

int main(int argc, char *argv[]){
  dungeon Dungeon;
  char dungeonChar[ROWS][COLS];
  for(int i=0; i<ROWS; i++)
    {
      for(int j=0; j<COLS; j++)
	{
	  dungeonChar[i][j]=' ';
	}
    }
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
  //init vars
  char name[13];
  name[12] = '\0';
  u_int32_t version, size;
  u_int16_t numberOfRooms, numberOfUpStairs, numberOfDownStairs;
  u_int8_t* roomCords; // *(roomCords + (roomindex)*4 + (0 = x, 1 = y, 2 = rows, 3 = cols)))
  u_int8_t* upStairCords; // *(upStairCords + (stairindex)*2 + (0 = x, 1 = y)))
  u_int8_t* downStairCords; // *(downStairCords + (stairindex)*2 + (0 = x, 1 = y)))

  //temp *******could break code if nothing is read in
  u_int8_t hardness[ROWS][COLS];

  if(argc==2&&strcmp(argv[1],"--load")==0)
  {

  //Checking if has min bytes for needed for data to be read in
  fseek(saveFile, 0, SEEK_END);
  //if(ftell(saveFile)  > 1708) {
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
    roomCords = (int8_t*)malloc(sizeof(int8_t*) * numberOfRooms * 4);
    fread(roomCords, sizeof(int8_t), numberOfRooms * 4, saveFile);
    fread(&numberOfUpStairs, sizeof(int16_t), 1, saveFile);
    numberOfUpStairs = be16toh(numberOfUpStairs);
    upStairCords = (int8_t*)malloc(sizeof(int8_t*) * numberOfUpStairs * 2);
    fread(upStairCords, sizeof(int8_t), numberOfUpStairs * 2, saveFile);
    fread(&numberOfDownStairs, sizeof(int16_t), 1, saveFile);
    numberOfDownStairs = be16toh(numberOfDownStairs);
    downStairCords = (int8_t*)malloc(sizeof(int8_t*) * numberOfDownStairs * 2);
    fread(downStairCords, sizeof(int8_t), numberOfDownStairs * 2, saveFile);
    
    //debug
    //for(int i = 0; i<ROWS; i++) {
    //for(int j = 0; j<COLS; j++) {
    //printf("Loaded hardness: %d\n", hardness[i][j]);
	//}
    //}
    //printf("Loaded total dungeon rooms: %i\n", numberOfRooms);
    //for(int i = 0; i<numberOfRooms; i++) {
    //  printf("Loaded room index: %i, X: %i, Y: %i, Rows: %i, Cols: %i\n", i, *(roomCords + i*4 + 0), *(roomCords + i*4 + 1), *(roomCords + i*4 + 2), *(roomCords + i*4 + 3));
    //}
    //printf("Loaded total number of stairs: %i, upStairs: %i, downStairs: %i\n", numberOfUpStairs + numberOfDownStairs, numberOfUpStairs, numberOfDownStairs);
    //for(int i = 0; i<numberOfUpStairs; i++) {
    //  printf("Loaded upStair index: %i, X: %i, Y: %i\n", i, *(upStairCords + i*2 + 0), *(upStairCords + i*2 + 1));
    //}
    //for(int i = 0; i<numberOfDownStairs; i++) {
    //  printf("Loaded downStair index: %i, X: %i, Y: %i\n", i, *(downStairCords + i*2 + 0), *(downStairCords + i*2 + 1));
    //}
    //debug
    
  fclose(saveFile);
  
  // *(roomCords + (roomindex)*4 + (0 = x, 1 = y, 2 = rows, 3 = cols)))
  //adding hardness, stairs, and rooms for loaded dungeons
  for(int i=0; i<numberOfRooms; i++)
    {
      for(int j=0; j<*(roomCords + (i)*4 +3); j++)
	{
	  for(int k=0; k<*(roomCords + (i)*4 +2); k++)
	    {
	      dungeonChar[*(roomCords + (i)*4+1)+j][*(roomCords + (i)*4)+k]='.';
	    }
	}
     
    }
  for(int i=0; i<ROWS; i++)
    {
      for(int j=0; j<COLS; j++)
	{
	  Dungeon.dungeonGrid[i][j]=hardness[i][j];
	}
    }

  // *(roomCords + (roomindex)*4 + (0 = x, 1 = y)))
  for(int i = 0; i < numberOfUpStairs; i++){
    dungeonChar[*(upStairCords + (i)*2+1)][*(upStairCords + (i)*2)] = '<';
  }
  for(int i = 0; i < numberOfDownStairs; i++){
    dungeonChar[*(downStairCords + (i)*2+1)][*(downStairCords + (i)*2)] = '>';
  }
  
  }
  //end of load
  else
    {



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
    }//
 //declaring char dungeon
for(int i = 0; i < ROWS; i++){
    for(int j = 0; j < COLS; j++){
      if(Dungeon.dungeonGrid[i][j] == FLOOR_HARDNESS&&dungeonChar[i][j]!='.'&&dungeonChar[i][j]!='<'&&dungeonChar[i][j]!='>'){
      dungeonChar[i][j] = '#';
      }
      else if(Dungeon.dungeonGrid[i][j] == BORDER_HARDNESS){
    		if(i == 0 || i == ROWS-1)
    		  dungeonChar[i][j] = '-';
    		else
    		  dungeonChar[i][j] = '|';
    	}
    	else if(dungeonChar[i][j]!='.'&&dungeonChar[i][j]!='<'&&dungeonChar[i][j]!='>'){
    		dungeonChar[i][j] = ' ';
    	}
    }
  }
 if(argc==2&&strcmp(argv[1],"--save")==0)
    {
  //start of save
  if((saveFile = fopen(path, "w")) == NULL) {
    printf("Error opening save file");
    exit(1);
  }

  //formating for write vars. new vars likly not needed. the function calls "be#toh()" need to be called on vars befor writing. watch out as lots of int8_t = int

  //hardness
    for(int i = 0; i<ROWS; i++) {
      for(int j = 0; j<COLS; j++) {
        hardness[i][j] = Dungeon.dungeonGrid[i][j];
        
        //debug
	//printf("Dungeon hardness:%d, Saved hardness: %d\n",Dungeon.dungeonGrid[i][j], hardness[i][j]);// think this converts from int32_t to int8_t
	//debug
    }
  }

  //rooms
  numberOfRooms = sizeof(Dungeon.Rooms) / sizeof(room); //hopefully gets length of dungeon rooms array
  
  //debug
  //printf("Saved total dungeon rooms: %i\n", numberOfRooms);
  //debug
  
  free(roomCords);
  roomCords = (int8_t*)malloc(sizeof(int8_t*) * numberOfRooms * 4);
  for(int i = 0; i<numberOfRooms; i++) {
    *(roomCords + i*4 + 1) = Dungeon.Rooms[i].gridRow;
    *(roomCords + i*4 + 0) = Dungeon.Rooms[i].gridCol;
    *(roomCords + i*4 + 3) = Dungeon.Rooms[i].rows;
    *(roomCords + i*4 + 2) = Dungeon.Rooms[i].cols;
    
    //debug
    //printf("Dungeon room index: %i, X: %i, Y: %i, Rows: %i, Cols: %i\n", i, Dungeon.Rooms[i].gridRow, Dungeon.Rooms[i].gridCol, Dungeon.Rooms[i].rows, Dungeon.Rooms[i].cols);
    //printf("Saved room index  : %i, X: %i, Y: %i, Rows: %i, Cols: %i\n", i, *(roomCords + i*4 + 0), *(roomCords + i*4 + 1), *(roomCords + i*4 + 2), *(roomCords + i*4 + 3));
    //debug
    
  }

  //stairs
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
  
  //debug
  //printf("Dungeon total number of stairs: %i\n", sizeof(Dungeon.Stairs) / sizeof(stairs));
  //printf("save total number of stairs   : %i, upStairs: %i, downStairs: %i\n", numberOfUpStairs + numberOfDownStairs, numberOfUpStairs, numberOfDownStairs);
  //debug
  
  free(upStairCords);
  free(downStairCords);
  upStairCords = (int8_t*)malloc(sizeof(int8_t*) * numberOfUpStairs * 2);
  downStairCords = (int8_t*)malloc(sizeof(int8_t*) * numberOfDownStairs * 2);
  int a=0;
  int b=0;
  for(int i = 0; i<sizeof(Dungeon.Stairs) / sizeof(stairs); i++) {
    if(Dungeon.Stairs[i].direction != '\0') {
      if(Dungeon.Stairs[i].direction == '<') {
        *(upStairCords + a*2 + 1) = Dungeon.Stairs[i].gridRow;
        *(upStairCords + a*2 + 0) = Dungeon.Stairs[i].gridCol;
        
	//debug
        //printf("Dungeon upStair index: %i, X: %i, Y: %i\n", i, Dungeon.Stairs[i].gridRow, Dungeon.Stairs[i].gridCol);
        //printf("Saved upStair index  : %i, X: %i, Y: %i\n", i, *(upStairCords + a*2 + 0), *(upStairCords + a*2 + 1));
        //debug
        
        a++;
      } else if(Dungeon.Stairs[i].direction == '>') {
        *(downStairCords + b*2 + 1) = Dungeon.Stairs[i].gridRow;
        *(downStairCords + b*2 + 0) = Dungeon.Stairs[i].gridCol;
        
	//debug
        //printf("Dungeon downStair index: %i, X: %i, Y: %i\n", i, Dungeon.Stairs[i].gridRow, Dungeon.Stairs[i].gridCol);
        //printf("Saved downStair index  : %i, X: %i, Y: %i\n", i, *(downStairCords + b*2 + 0), *(downStairCords + b*2 + 1));
        //debug
        
        b++;
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
    }
  //debug
  //printf("calculated save size: %i\n", 12 + 4 + 4 + 2 + 1680 + 2 + numberOfRooms * 4 + 2 + numberOfUpStairs * 2 + 2 + numberOfDownStairs * 2);

  printBoard(dungeonChar);

  return 0;
}
