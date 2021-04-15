#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <endian.h>
#include <sys/time.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <errno.h>
#include <ncurses.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <string>
#include <cmath>
#include <numeric>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>

#include "heap.h"
#include "dice.h"
#include "object_descriptions.h"
#include "dungeon.h"


//comparator for dijkstras
static int32_t cell_cmp(const void *key, const void *with) {
  return ((cell *) key)->priority - ((cell *) with)->priority;
}
//comparator for gameloop
static int32_t character_cmp(const void *key, const void *with){
	if(((Character *) key)->nextTurn - ((Character *) with)->nextTurn == 0)
		return ((Character *) key)->sequenceNum - ((Character *) with)->sequenceNum;
	else
		return ((Character *) key)->nextTurn - ((Character *) with)->nextTurn;
}

void object_factory(dungeon *d){
	int objectsPlaced = 0;
  //better style to produce random number
  std::random_device rd;
  std::mt19937 mt(rd());
  //get ranges set (inclusive)
   std::cout << "wasdadadad" << std::endl;
  std::uniform_int_distribution<int> obj(0, d->object_descriptions.size()-1);
  std::uniform_int_distribution<int> row(1, 13);
  std::uniform_int_distribution<int> col(1, 70);
  std::uniform_int_distribution<int> rrty(0, 99);
	while(objectsPlaced < MAX_OBJECTS){
    int randObj = 0;
    Object newObj;
    while(1){
      randObj = obj(mt);
      if(!d->object_descriptions[randObj].placed || d->object_descriptions[randObj].get_artifact() == false){
        int randRarity = rrty(mt);
        if(randRarity < (int)d->object_descriptions[randObj].get_rarity()){
          break;
        }
      }
    }
    newObj.name = d->object_descriptions[randObj].get_name();
    newObj.color = d->object_descriptions[randObj].get_color();
    newObj.Damage = d->object_descriptions[randObj].get_damage();
    newObj.type = d->object_descriptions[randObj].get_type();
    newObj.hit = d->object_descriptions[randObj].get_hit().roll();
    newObj.dodge = d->object_descriptions[randObj].get_dodge().roll();
    newObj.defense = d->object_descriptions[randObj].get_defence().roll();
    newObj.weight = d->object_descriptions[randObj].get_weight().roll();
    newObj.speed = d->object_descriptions[randObj].get_speed().roll();
    newObj.attribute = d->object_descriptions[randObj].get_attribute().roll();
    newObj.value = d->object_descriptions[randObj].get_value().roll();
    newObj.rarity = d->object_descriptions[randObj].get_rarity();
    newObj.artifact = d->object_descriptions[randObj].get_artifact();
    newObj.symbol = object_symbol[newObj.type];
    while(1){
      newObj.gridRow = row(mt);
    	newObj.gridCol = col(mt);
      if(d->dungeon[newObj.gridRow][newObj.gridCol].character == '.' || d->dungeon[newObj.gridRow][newObj.gridCol].character == '#'){
        objectsPlaced++;
        d->objects.push_back(newObj);
        d->object_descriptions[randObj].placed = 1;
        break;
      }
    }
	}
}

void monster_factory(dungeon *d, int index){
  std::random_device rd;
  std::mt19937 mt(rd());
  //get ranges set (inclusive)
  std::uniform_int_distribution<int> mon(0, d->monsDes.size()-1);
  std::uniform_int_distribution<int> rrty(0, 99);
  int randMon;
  while(1) {
    randMon = mon(mt);
    if(!d->monsDes[randMon].placed || !(d->monsDes[randMon].abilBits && (1 << 7))){
      int rarity = rrty(mt);
      if(rarity < d->monsDes[randMon].rarity)
        break;
    }
  }
  //monsters won't have names
  (d->characters + index)->dead = 0;
  (d->characters + index)->colorBits.push_back(d->monsDes[randMon].colorBits[0]);
  (d->characters + index)->speed = d->monsDes[randMon].speedtest.roll();
  (d->characters + index)->type = d->monsDes[randMon].abilBits;
  (d->characters + index)->hitpoints = d->monsDes[randMon].hitpoints.roll();
  (d->characters + index)->rarity = d->monsDes[randMon].rarity;
  (d->characters + index)->Damage.set_base(d->monsDes[randMon].Damage.get_base());
  (d->characters + index)->Damage.set_number(d->monsDes[randMon].Damage.get_number());
  (d->characters + index)->Damage.set_sides(d->monsDes[randMon].Damage.get_sides());
  (d->characters + index)->typeofChar = d->monsDes[randMon].symbol[0];
}
//initializing board
void init_dungeon(dungeon *d){
	d->fullDungeon = true;
 for(int i = 0; i < ROWS; i++){
    for(int j = 0; j < COLS; j++){
    	d->dungeon[i][j].character = ' ';
    }
 }
 srand(time(0));
 //random dimensions for each room in the array
 for(int i = 0; i < MAX_ROOMS; i++){
	d->Rooms[i].rows = (rand() % 4) + 4;
	d->Rooms[i].cols = (rand() % 4) + 5;
 }
  //initializing the board to white spaces
  for(int i = 0; i < ROWS; i++){
    for(int j = 0; j < COLS; j++){
    	//dungeonGrid[i][j] = ROCK_HARDNESS;
    	//for cell dungeon
    	d->dungeon[i][j].hardness = (rand() % 254) + 1;

		d->revelaedBoard[i][j] = ' ';
    }
  }

  //adding horizontal borders
  for(int i=0; i<COLS; i++){
    //dungeonGrid[0][i] = BORDER_HARDNESS;
    //dungeonGrid[ROWS - 1][i] = BORDER_HARDNESS;
    //for cell dungeon
    d->dungeon[0][i].hardness = BORDER_HARDNESS;
    d->dungeon[0][i].character = '-';
    d->dungeon[ROWS - 1][i].hardness = BORDER_HARDNESS;
    d->dungeon[ROWS - 1][i].character = '-';

	d->revelaedBoard[ROWS - 1][i] = '-';
	d->revelaedBoard[0][i] = '-';
  }
  //adding vertical borders
  for(int i=1; i<ROWS-1; i++){
    //dungeonGrid[i][0] = BORDER_HARDNESS;
    //dungeonGrid[i][COLS - 1] = BORDER_HARDNESS;
    //for cell dungeon
    d->dungeon[i][0].hardness = BORDER_HARDNESS;
    d->dungeon[i][0].character = '|';
    d->dungeon[i][COLS - 1].hardness = BORDER_HARDNESS;
    d->dungeon[i][COLS - 1].character = '|';

	d->revelaedBoard[i][COLS - 1] = '|';
	d->revelaedBoard[i][0] = '|';
  }
  int roomsPlaced = 0;
  //this loop should continue until all rooms are placed in free spaces
  while(roomsPlaced < MAX_ROOMS){
  	int freeSpace = 1;
	int randRow = (rand() % 13) + 1;
	int randCol = (rand() % 70) + 1;
	//checking if room + border will fit
	for(int i=-1; i<d->Rooms[roomsPlaced].rows+1; i++){
	  for(int j=-1; j<d->Rooms[roomsPlaced].cols+1; j++){
	  	if(d->dungeon[randRow + i][randCol + j].hardness == BORDER_HARDNESS || d->dungeon[randRow + i][randCol + j].hardness == FLOOR_HARDNESS){
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
	  d->Rooms[roomsPlaced].gridRow = randRow;
	  d->Rooms[roomsPlaced].gridCol = randCol;
	  for(int i=0; i<d->Rooms[roomsPlaced].rows; i++){
	    for(int j=0; j<d->Rooms[roomsPlaced].cols;j++){
	    	//placing player in first room top left
	    	if(roomsPlaced == 0){
	    	  d->pc.gridRow = randRow;
	    	  d->pc.gridCol = randCol;
	    	  (d->characters)->gridRow = randRow;
	    	  (d->characters)->gridCol = randCol;
	    	  (d->characters)->typeofChar = '@';
	    	  (d->characters)->speed = PC_SPEED;
	    	  (d->characters)->sequenceNum = 0;
			  (d->characters)->colorBits.push_back(PC_COLOR);
	    	}
	     	//dungeonGrid[randRow + i][randCol + j] = 0;
	     	d->dungeon[randRow + i][randCol + j].hardness = FLOOR_HARDNESS;
	     	d->dungeon[randRow + i][randCol + j].character = '.';
	    }
	  }
	  roomsPlaced++;
	}
}
//this loops through each room and its neigbor in the array to connect them together with corridors
for(int i = 0; i < MAX_ROOMS-1;i++){
        //finds the direction in which a room finds its neighboor room
	int directionY = (d->Rooms[i+1].gridRow - d->Rooms[i].gridRow);
	int directionX = (d->Rooms[i+1].gridCol - d->Rooms[i].gridCol);
	//horizontal direction
	for(int j = d->Rooms[i].gridRow; j != d->Rooms[i+1].gridRow; j += (directionY>0) ? 1 : -1){
		if(d->dungeon[j][d->Rooms[i].gridCol].hardness != BORDER_HARDNESS && d->dungeon[j][d->Rooms[i].gridCol].hardness != FLOOR_HARDNESS){
			//dungeonGrid[j][Dungeon.Rooms[i].gridCol] = 0;
			d->dungeon[j][d->Rooms[i].gridCol].character = '#';
			d->dungeon[j][d->Rooms[i].gridCol].hardness = FLOOR_HARDNESS;
			}
	}
	//vertical direction
	for(int k = d->Rooms[i].gridCol; k != d->Rooms[i+1].gridCol; k += (directionX>0) ? 1 : -1){
		if(d->dungeon[d->Rooms[i].gridRow+directionY][k].hardness != BORDER_HARDNESS && d->dungeon[d->Rooms[i].gridRow+directionY][k].hardness != FLOOR_HARDNESS){
			//dungeonGrid[Dungeon.Rooms[i].gridRow+directionY][k] = 0;
			d->dungeon[d->Rooms[i].gridRow+directionY][k].character = '#';
			d->dungeon[d->Rooms[i].gridRow+directionY][k].hardness = FLOOR_HARDNESS;
			}
	}
}
//this loop will place stairs randomly on the map but making sure it is on a floor
    int stairsPlaced = 0;
      while(stairsPlaced < MAX_STAIRS){
	  int randRow = (rand() % 13) + 1;
	  int randCol = (rand() % 70) + 1;
	  if(d->dungeon[randRow][randCol].hardness == FLOOR_HARDNESS){
		d->Stairs[stairsPlaced].gridRow = randRow;
		d->Stairs[stairsPlaced].gridCol = randCol;
		//will switch between upstairs and downstairs
		if(stairsPlaced % 2 == 0){
			d->Stairs[stairsPlaced++].direction = '<';
			d->dungeon[randRow][randCol].hardness = 0;
			d->dungeon[randRow][randCol].character = '<';
		}
		else{
			d->Stairs[stairsPlaced++].direction = '>';
			d->dungeon[randRow][randCol].hardness = 0;
			d->dungeon[randRow][randCol].character = '>';
		}

	   }

       }
       object_factory(d);
       //this loop places monsters on floors
    int monstersPlaced = 0;
    d->monstersAlive = d->numOfCharacters-1;
    std::random_device rd;
    std::mt19937 mt(rd());
    //get ranges set (inclusive)
    std::uniform_int_distribution<int> row(1, 13);
    std::uniform_int_distribution<int> col(1, 70);
    while(monstersPlaced < d->numOfCharacters){ //gets stuck in this loop
    int randRow = row(mt);
	  int randCol = col(mt);
    if(d->dungeon[randRow][randCol].hardness == FLOOR_HARDNESS && randRow != d->pc.gridRow && randCol != d->pc.gridCol){
       	 	//finds a random type and speed for monster
       	 	//declaring the monster inside of the character array starting at index 1
         monster_factory(d, monstersPlaced + 1);
		 (d->characters + monstersPlaced + 1)->gridRow = randRow;
		 (d->characters + monstersPlaced + 1)->gridCol = randCol;
       	 	//prints to a hexa decimal, hard-coded
       	 	monstersPlaced++;
       	 }
    }
}
//prints board for nontunnelers
void printNonTun(dungeon *d){
	for(int i = 0; i < ROWS;i++){
	  for(int j = 0; j < COLS; j++){
	  	if(d->dungeon[i][j].hardness != 0)
	  		printf("%c", d->dungeon[i][j].character);
	  	else if(d->pc.gridRow==i && d->pc.gridCol==j)
	  		printf("%c", d->dungeon[i][j].character);
	  	else
	  		printf("%d", d->dungeon[i][j].priority%10);
	  }
	  printf("\n");
	}
}

//prints board for tunnelers
void printTun(dungeon *d){
        for(int i = 0; i < ROWS;i++){
	  for(int j = 0; j < COLS; j++){
	  	if(d->dungeon[i][j].hardness == 255)
	  		printf("%c", d->dungeon[i][j].character);
	  	else if(d->pc.gridRow==i && d->pc.gridCol==j)
	  		printf("%c", d->dungeon[i][j].character);
	  	else
	  		printf("%d", d->dungeon[i][j].priority%10);
	  }
	  printf("\n");
	}
}

void rotateVec(double angle, double *v, double* arr) { //rotates a vectore ccw
	double cs = cos(angle);
	double sn = sin(angle);
	*arr = (v[0]*cs) + (v[1]*sn);
	*(arr + 1) = (v[0]*sn*(-1)) + (v[1]*cs);
}

//prints regular board
void printBoard(dungeon *d) {
  BoldBoard board;
  for(int i = 0; i<ROWS; i++) { //init a temp board, needed for perserving characters
	for(int j = 0; j < COLS; j++) {
		board.board[i][j] = '\0';
		board.bold[i][j] = false;
		board.color[i][j] = 7;
	}
  }

  if(!(d->fullDungeon)) {
	  for(int k = -1; k<2; k+=2) { //manually going through direct N,S,E,W
		  for(int l = 0; l<=1; l++) {
			for(int i = 0; i<=VEIW_RADIUS; i++) {
				int row = d->pc.gridRow + ((i*k)*(l));
				int col = d->pc.gridCol + (((i+1)*k)*(1-l));
				if(row < ROWS && col < COLS && row >= 0 && col >= 0) {
					char seen = d->dungeon[row][col].character;
					if(seen == ' ') {
						break;
					} else {
						d->revelaedBoard[row][col] = seen;
						//board.board[row][col] = seen; //no remembering
						board.bold[row][col] = true;
    					for(std::vector<Object>::iterator it = d->objects.begin(); it != d->objects.end(); ++it) { //adding items if they are in seen location
							if(it->gridRow == row && it->gridCol == col) {
								board.board[it->gridRow][it->gridCol] = it->symbol;
								board.color[it->gridRow][it->gridCol] = it->color;
								board.bold[row][col] = false;
								break;
							}
    					}
						for(int k = 0; k < d->numOfCharacters; k++) { //adding monsters if they are in seen location
							if((d->characters+k)->dead!=1 && (d->characters+k)->gridRow == row && (d->characters+k)->gridCol == col) {
								board.board[row][col] = (d->characters+k)->typeofChar;
								board.color[row][col] = (d->characters+k)->colorBits[0];
								board.bold[row][col] = false;
								break;
							}
						}
					}
				}
			}
		  }
	  }

	  double theta = (6.28) / ((VEIW_RADIUS * 10) + 50);
	  double sightVec[] = {VEIW_RADIUS, 0};

	  for(double a = theta; a < 6.5; a+=theta) {
		  double checkSightVec[2] = {VEIW_RADIUS, 0};
		  rotateVec(a, sightVec, checkSightVec);
		  int den = std::__detail::__gcd((unsigned int) round(abs(checkSightVec[0])), (unsigned int) round(abs(checkSightVec[1]))); //to find smallest steps
		  int i = 0;
		  int j = 0;
		  int prevRow = 0;
		  int prevCol = 0;
		  bool noWall = true;
		  for(int l = 0; l<den && noWall; l++) { //if vect broken up repeat
			for(i = 0; abs(i) < (abs(checkSightVec[0]))/(den-l) && noWall; checkSightVec[0] > 0 ? i++ : i--) {
				for(j = 0; abs(j) < (abs(checkSightVec[1]))/(den-l) && noWall; checkSightVec[1] > 0 ? j++ : j--) { //loops through vecter and breaks if touches a wall, has a bais
					int row = d->pc.gridRow + i + prevRow;
					int col = d->pc.gridCol + j + prevCol;
					if(row < ROWS && col < COLS && row >= 0 && col >= 0) {
						char seen = d->dungeon[row][col].character;
						if(seen == ' ') {
							noWall = false;
							break;
						} else {
							d->revelaedBoard[row][col] = seen;
							//board.board[row][col] = seen; //no remembering
							board.bold[row][col] = true;
    						for(std::vector<Object>::iterator it = d->objects.begin(); it != d->objects.end(); ++it) { //adding items if they are in seen location
								if(it->gridRow == row && it->gridCol == col) {
									board.board[it->gridRow][it->gridCol] = it->symbol;
									board.color[it->gridRow][it->gridCol] = it->color;
									board.bold[row][col] = false;
									break;
								}
    						}
							for(int k = 0; k < d->numOfCharacters; k++) { //adding monsters if they are in seen location
								if((d->characters+k)->dead!=1 && (d->characters+k)->gridRow == row && (d->characters+k)->gridCol == col) {
									board.board[row][col] = (d->characters+k)->typeofChar;
									board.color[row][col] = (d->characters+k)->colorBits[0];
									board.bold[row][col] = false;
									break;
								}
							}
						}
					}
				}
				prevCol = j;
			}
			prevRow = i;
		  }
	  }

	  for(int i = 0; i<ROWS; i++) { //copy revelaedBoard into a temp board
		  for(int j = 0; j < COLS; j++) {
			  if(board.board[i][j] == '\0') {
				  board.board[i][j] = d->revelaedBoard[i][j];
			  }
		  }
	  }

  } else {
		for(int i = 0; i < ROWS; i++){ //copy terrain
			for(int j = 0; j < COLS; j++){
				board.board[i][j] = d->dungeon[i][j].character;
			}
		}
    	for(std::vector<Object>::iterator it = d->objects.begin(); it != d->objects.end(); ++it){
      		board.board[it->gridRow][it->gridCol] = it->symbol;
			board.color[it->gridRow][it->gridCol] = it->color;
    	}
		for(int i = 0; i < d->numOfCharacters; i++) { // replacing terrain with characters
			if((d->characters+i)->dead!=1) {
				board.board[(d->characters+i)->gridRow][(d->characters+i)->gridCol] = (d->characters+i)->typeofChar;
				board.color[(d->characters+i)->gridRow][(d->characters+i)->gridCol] = (d->characters+i)->colorBits[0];
			}
		}
  }

  for(int i = 0; i < ROWS;i++){ //printing board
	  for(int j = 0; j < COLS; j++){
		  board.bold[i][j] ? attron(A_BOLD) : attroff(A_BOLD);
      /*for(std::vector<Object>::iterator it = d->objects.begin(); it != d->objects.end(); ++it){
        if(i == it->gridRow && j == it->gridCol){
          attron(COLOR_PAIR(it->color));
          mvaddch(i+1, j, board.board[i][j]);
          attroff(COLOR_PAIR(it->color));
          break;
        }
      }*/ //this is not working
	  	attron(COLOR_PAIR(board.color[i][j]));
      	mvaddch(i+1, j, board.board[i][j]);
	  }
  }
}

void printBoardWcursor(dungeon *d, int cursorRow, int cursorCol){
	mvprintw(0, 1, "teleport mode");
	char board[ROWS][COLS];
	for(int i = 0; i < ROWS;i++){ //copy terrain
	  for(int j = 0; j < COLS; j++){
	    board[i][j] = d->dungeon[i][j].character;
	  }
	}
	for(int i = 0; i < d->numOfCharacters; i++) { // replacing terrain with characters
	if((d->characters+i)->dead!=1)
	{
	  board[(d->characters+i)->gridRow][(d->characters+i)->gridCol] = (d->characters+i)->typeofChar;
	}
	}
	board[cursorRow][cursorCol] = '*';

	for(int i = 0; i < ROWS;i++){
	  for(int j = 0; j < COLS; j++){
		mvaddch(i+1, j, board[i][j]);
	  }
	}
}
//testing if characters placed correctly

heap_t movePrioQueueInit(dungeon *d) { //init prioQueue
	heap_t h;
	heap_init(&h, character_cmp, NULL);
	for(int i = 0; i < d->numOfCharacters; i++) { //setting speed
		(d->characters+i)->nextTurn = 1000/(d->characters+i)->speed;
		heap_insert(&h, (d->characters+i));
	}
	return h;
}

//sees if the pc is visible to the monster in cardinal directions and can see past other monsters
int pc_visible(dungeon *d, Character *p){
	int direction = 0;
	if(p->gridRow == d->pc.gridRow){
		direction = (p->gridCol < d->pc.gridCol) ? 1 : -1;
		int i = direction;
		while(d->dungeon[p->gridRow][p->gridCol+i].hardness == FLOOR_HARDNESS){
			if((p->gridCol+i) == d->pc.gridCol){
				p->lastSeenRow = d->pc.gridRow;
				p->lastSeenCol = p->gridCol+i;
				return 1;
			}
			i += direction;
		}
		return -1;
	}
	else if(p->gridCol == d->pc.gridCol){
		direction = (p->gridRow < d->pc.gridRow) ? 1 : -1;
		int i = direction;
		while(d->dungeon[p->gridRow+i][p->gridCol].hardness == FLOOR_HARDNESS){
			if((p->gridRow+i) == d->pc.gridRow){
				p->lastSeenRow = p->gridRow+i;
				p->lastSeenCol = d->pc.gridRow;
				return 1;
			}
			i += direction;
		}
		return -1;
	}
	return -1;

}
//displaying monsters on new 9dow
void display_monsters(dungeon *d, WINDOW * win){
	keypad(win, TRUE);
	int start = 1;
	int end = (d->monstersAlive <= 24) ? d->numOfCharacters : 24;
	for(int i = start;i < end; i++){
	  if((d->characters+i)->dead != 1){
		wprintw(win, "%c, ", (d->characters+i)->typeofChar);
		if((d->characters+i)->gridRow - (d->characters)->gridRow != 0){
			wprintw(win, "%d %s ", abs((d->characters+i)->gridRow - (d->characters)->gridRow), ((d->characters+i)->gridRow < (d->characters)->gridRow) ? "north" : "south");
		}
		if((d->characters+i)->gridRow - (d->characters)->gridRow != 0 && (d->characters+i)->gridCol - (d->characters)->gridCol != 0)
			wprintw(win,"and ");

		if((d->characters+i)->gridCol - (d->characters)->gridCol != 0){
			wprintw(win, "%d %s", abs((d->characters+i)->gridCol - (d->characters)->gridCol), ((d->characters+i)->gridCol < (d->characters)->gridCol) ? "west" : "east");

		}
		wprintw(win,"\n");
	  }

	}
	mvwprintw(win, 5, 50, "press esc to go back to map");
	while(1){
	  int option = wgetch(win);
	  if(option == KEY_DOWN){
		 werase(win);
		 if(end != d->numOfCharacters){
		 	start++;
		 	end++;
		 }
	  }
	  else if(option == KEY_UP){
	  	werase(win);
	  	if(start != 1){
	  	  start--;
	  	  end--;
	  	}
	  }
	  else if(option == 27){
	  	break;
	  }
	  for(int i = start;i < end; i++){

	    if((d->characters+i)->dead != 1){
		 wprintw(win, "%c, ", (d->characters+i)->typeofChar);
		 if((d->characters+i)->gridRow - (d->characters)->gridRow != 0){
			wprintw(win, "%d %s ", abs((d->characters+i)->gridRow - (d->characters)->gridRow), ((d->characters+i)->gridRow < (d->characters)->gridRow) ? "north" : "south");
		 }
		 if((d->characters+i)->gridRow - (d->characters)->gridRow != 0 && (d->characters+i)->gridCol - (d->characters)->gridCol != 0)
			wprintw(win,"and ");

		 if((d->characters+i)->gridCol - (d->characters)->gridCol != 0){
			wprintw(win, "%d %s", abs((d->characters+i)->gridCol - (d->characters)->gridCol), ((d->characters+i)->gridCol < (d->characters)->gridCol) ? "west" : "east");

		 }
		wprintw(win,"\n");
	    }
	  }
	  mvwprintw(win, 5, 50, "press esc to go back to map");

	}
	delwin(win);

}

void teleport(dungeon *d){
	int cursorRow = d->pc.gridRow;
	int cursorCol = d->pc.gridCol;
	while(1){
		printBoardWcursor(d, cursorRow, cursorCol);
		int direction = getch();
		if(direction == KEY_UP){
		if(cursorRow<=1){
		return;}
			cursorRow--;}
		else if(direction == KEY_DOWN){
		if(cursorRow>=19){
		return;}
			cursorRow++;}
		else if(direction == KEY_LEFT){
		if(cursorCol<=1){
		return;}
			cursorCol--;}
		else if(direction == KEY_RIGHT){
		if(cursorCol>=78){
		return;}
			cursorCol++;}
		else if(direction == 'r'){
			int randRow = (rand() % 19) + 1;
			int randCol = (rand() % 78) + 1;
			d->pc.gridRow = randRow;
			d->pc.gridCol = randCol;
			(d->characters)->gridRow = randRow;
			(d->characters)->gridCol = randCol;
			if(d->dungeon[randRow][randCol].hardness != 0){
				d->dungeon[randRow][randCol].hardness = 0;
				d->dungeon[randRow][randCol].character = '#';
			}
			for(int i = 1; i < d->numOfCharacters; i++){
			  if(d->pc.gridRow == (d->characters+i)->gridRow && d->pc.gridCol == (d->characters+i)->gridCol){
				  	(d->characters+i)->dead=1;
				  	d->monstersAlive--;
				  	}
			}
			break;
		}
		else if(direction == 'g'){
			d->pc.gridRow = cursorRow;
			d->pc.gridCol = cursorCol;
			(d->characters)->gridRow = cursorRow;
			(d->characters)->gridCol = cursorCol;
			if(d->dungeon[cursorRow][cursorCol].hardness != 0){
				d->dungeon[cursorRow][cursorCol].hardness = 0;
				d->dungeon[cursorRow][cursorCol].character = '#';
			}
			break;
		}


	}
}

int pc_move(dungeon *d){
	printBoard(d);
	int move = getch();
	int yDirection = 0;
	int xDirection = 0;

	if(move == '7' || move == 'y'){
		yDirection--;
		xDirection--;
		for(int i=1; i<d->monstersAlive; i++)
		{
		//if((d->pc).gridRow+yDirection==(d->characters+i)->gridRow&&(d->pc).gridCol+xDirection==(d->characters+i)->gridCol)
		{
			//(d->characters+i)->hitpoints-=(d->pc).damage.roll();
		}
		}
	}
	else if(move == '8' || move == 'k')
		yDirection--;
	else if(move == '9' || move == 'u'){
		yDirection--;
		xDirection++;
	}
	else if(move == '6' || move == 'l')
		xDirection++;
	else if(move == '3' || move == 'n'){
		yDirection++;
		xDirection++;
	}
	else if(move == '2' || move == 'j')
		yDirection++;
	else if(move == '1' || move == 'b'){
		yDirection++;
		xDirection--;
	}
	else if(move == '4' || move == 'h')
		xDirection--;

	else if(move == '5' || move == '.' || move == ' '){}

	else if(move == 'g'){
		erase();
		teleport(d);
		erase();
	}

	else if(move == 'Q'){
		endwin();
		exit(0);
		}

	else if(move == 'm'){
		WINDOW * win = newwin(24, 80, 0, 0);
		display_monsters(d, win);
		erase();
		return 0;
		}
	else if(move == '<'){
		if(d->dungeon[d->pc.gridRow][d->pc.gridCol].character == '>'){
		  mvprintw(0, 1, "wrong stair-case");
		  return 0;
		}
		else if(d->dungeon[d->pc.gridRow][d->pc.gridCol].character != '<'){
		  mvprintw(0, 1, "this is not a stair-case");
		  return 0;
		}
		init_dungeon(d);
	}
	else if(move == '>'){
		if(d->dungeon[d->pc.gridRow][d->pc.gridCol].character == '<'){
		  mvprintw(0, 1, "wrong stair-case");
		  return 0;
		}
		else if(d->dungeon[d->pc.gridRow][d->pc.gridCol].character != '>'){
		  mvprintw(0, 1, "this is not a stair-case");
		  return 0;
		}

		init_dungeon(d);
	}
	else if(move == 'f')
	{
		d->fullDungeon=!(d->fullDungeon);
		return 0;
	}


	else{
		mvprintw(0, 1, "not an input, try again");
		return 0;
	}

	if(d->dungeon[d->pc.gridRow+yDirection][d->pc.gridCol+xDirection].hardness != 0){
		mvprintw(0, 1, "There is a wall dummy!!");
		return 0;
	}
	d->pc.gridRow+=yDirection;
	d->pc.gridCol+=xDirection;
	(d->characters)->gridRow+=yDirection;
	(d->characters)->gridCol+=xDirection;
	for(int i = 1; i < d->numOfCharacters; i++){
	  if(d->pc.gridRow == (d->characters+i)->gridRow && d->pc.gridCol == (d->characters+i)->gridCol){
		  	(d->characters+i)->dead=1;
		  	d->monstersAlive--;
		  	}
	}

	return 1;
}

//algorithm for nontunnelers
void Dijkstras_nontun(dungeon *d, int row, int col){
	//points to the current cell that was popped from heap
	static cell *p;
	heap_t h;
	//declaring all cells to be distance infinity
	for(int i = 0; i < ROWS; i++){
	  for(int j = 0; j < COLS; j++){
	    d->dungeon[i][j].priority = INT_MAX;
	  }
	}
	//making the player character the root of heap and distance 0 since that where we want to start
	d->dungeon[row][col].priority = 0;

	heap_init(&h, cell_cmp,  NULL);
	//inserting all floor cells inside heap
	for(int i = 0; i < ROWS; i++){
	  for(int j = 0; j < COLS; j++){
	    if(d->dungeon[i][j].hardness == 0)
	      d->dungeon[i][j].hn = heap_insert(&h, &d->dungeon[i][j]);
	    else
	      d->dungeon[i][j].hn = NULL;
	  }
	}
	//this loop pulls the root of the heap and goes through its neighbors. It will go until heap is empty
	while((p = (cell*)heap_remove_min(&h))){
	  p->hn = NULL;

	  if((d->dungeon[p->gridRow + 1][p->gridCol + 1].hn) && (d->dungeon[p->gridRow + 1][p->gridCol + 1].priority > (p->priority + 1))){
	  	d->dungeon[p->gridRow + 1][p->gridCol + 1].priority = p->priority + 1;
	  	heap_decrease_key_no_replace(&h, d->dungeon[p->gridRow + 1][p->gridCol + 1].hn);
	  }

	 if((d->dungeon[p->gridRow + 1][p->gridCol - 1].hn) && (d->dungeon[p->gridRow + 1][p->gridCol - 1].priority > (p->priority + 1))){
	  	d->dungeon[p->gridRow + 1][p->gridCol - 1].priority = p->priority + 1;
	  	heap_decrease_key_no_replace(&h, d->dungeon[p->gridRow + 1][p->gridCol - 1].hn);
	  }

	  if((d->dungeon[p->gridRow - 1][p->gridCol + 1].hn) && (d->dungeon[p->gridRow - 1][p->gridCol + 1].priority > (p->priority + 1))){
	  	d->dungeon[p->gridRow - 1][p->gridCol + 1].priority = p->priority + 1;
	  	heap_decrease_key_no_replace(&h, d->dungeon[p->gridRow - 1][p->gridCol + 1].hn);
	  }

	  if((d->dungeon[p->gridRow - 1][p->gridCol - 1].hn) && (d->dungeon[p->gridRow - 1][p->gridCol - 1].priority > (p->priority + 1))){
	  	d->dungeon[p->gridRow - 1][p->gridCol - 1].priority = p->priority + 1;
	  	heap_decrease_key_no_replace(&h, d->dungeon[p->gridRow - 1][p->gridCol - 1].hn);
	  }

	  if((d->dungeon[p->gridRow + 1][p->gridCol].hn) && (d->dungeon[p->gridRow + 1][p->gridCol].priority > (p->priority + 1))){
	  	d->dungeon[p->gridRow + 1][p->gridCol].priority = p->priority + 1;
	  	heap_decrease_key_no_replace(&h, d->dungeon[p->gridRow + 1][p->gridCol].hn);
	  }

	   if((d->dungeon[p->gridRow - 1][p->gridCol].hn) && (d->dungeon[p->gridRow - 1][p->gridCol].priority > (p->priority + 1))){
	  	d->dungeon[p->gridRow - 1][p->gridCol].priority = p->priority + 1;
	  	heap_decrease_key_no_replace(&h, d->dungeon[p->gridRow - 1][p->gridCol].hn);
	  }

	  if((d->dungeon[p->gridRow][p->gridCol + 1].hn) && (d->dungeon[p->gridRow][p->gridCol + 1].priority > (p->priority + 1))){
	  	d->dungeon[p->gridRow][p->gridCol + 1].priority = p->priority + 1;
	  	heap_decrease_key_no_replace(&h, d->dungeon[p->gridRow][p->gridCol + 1].hn);
	  }

	  if((d->dungeon[p->gridRow][p->gridCol - 1].hn) && (d->dungeon[p->gridRow][p->gridCol - 1].priority > (p->priority + 1))){
	  	d->dungeon[p->gridRow][p->gridCol - 1].priority = p->priority + 1;
	  	heap_decrease_key_no_replace(&h, d->dungeon[p->gridRow][p->gridCol - 1].hn);
	  }


	}


}

//algorithm for tunnelers
void Dijkstras_tun(dungeon *d, int row, int col) {
	//points to the current cell that was popped from heap
	static cell *p;
	heap_t h;
	//declaring all cells to be distance infinity
	for(int i = 0; i < ROWS; i++){
	  for(int j = 0; j < COLS; j++){
	    d->dungeon[i][j].priority = INT_MAX;
	  }
	}
	//making the player character the root of heap and distance 0 since that where we want to start
	d->dungeon[row][col].priority = 0;

	heap_init(&h, cell_cmp,  NULL);
	//add all cells that not border cells, into heap
	for(int i = 0; i < ROWS; i++){
	  for(int j = 0; j < COLS; j++){
	    if(d->dungeon[i][j].hardness != BORDER_HARDNESS)
	      d->dungeon[i][j].hn = heap_insert(&h, &d->dungeon[i][j]);
	    else
	      d->dungeon[i][j].hn = NULL;
	  }
	}
	//this loop pulls the root of the heap and goes through its neighbors. It will go until heap is empty
	while((p = (cell*)heap_remove_min(&h))){
	  p->hn = NULL;

	  if(d->dungeon[p->gridRow + 1][p->gridCol + 1].hn){
	  	if(d->dungeon[p->gridRow + 1][p->gridCol + 1].priority > (p->priority + 1) && d->dungeon[p->gridRow + 1][p->gridCol + 1].hardness ==0){
	  	  d->dungeon[p->gridRow + 1][p->gridCol + 1].priority = p->priority + 1;
	  	  heap_decrease_key_no_replace(&h, d->dungeon[p->gridRow + 1][p->gridCol + 1].hn);
	  	}
	  	else if(d->dungeon[p->gridRow + 1][p->gridCol + 1].priority > (p->priority + (1 + (p->hardness/85))) && d->dungeon[p->gridRow + 1][p->gridCol + 1].hardness != 0){
	  	  d->dungeon[p->gridRow + 1][p->gridCol + 1].priority = p->priority + (1 + (p->hardness/85));
	  	  heap_decrease_key_no_replace(&h, d->dungeon[p->gridRow + 1][p->gridCol + 1].hn);
	  	}
	  }

	 if(d->dungeon[p->gridRow + 1][p->gridCol - 1].hn){
	  	if(d->dungeon[p->gridRow + 1][p->gridCol - 1].priority > (p->priority + 1) && d->dungeon[p->gridRow + 1][p->gridCol - 1].hardness ==0){
	  	  d->dungeon[p->gridRow + 1][p->gridCol - 1].priority = p->priority + 1;
	  	  heap_decrease_key_no_replace(&h, d->dungeon[p->gridRow + 1][p->gridCol - 1].hn);
	  	}
	  	else if(d->dungeon[p->gridRow + 1][p->gridCol - 1].priority > (p->priority + (1 + (p->hardness/85))) && d->dungeon[p->gridRow + 1][p->gridCol - 1].hardness != 0){
	  	  d->dungeon[p->gridRow + 1][p->gridCol - 1].priority = p->priority + (1 + (p->hardness/85));
	  	  heap_decrease_key_no_replace(&h, d->dungeon[p->gridRow + 1][p->gridCol - 1].hn);
	  	}
	  }

	  if(d->dungeon[p->gridRow - 1][p->gridCol + 1].hn){
	  	if(d->dungeon[p->gridRow - 1][p->gridCol + 1].priority > (p->priority + 1) && d->dungeon[p->gridRow - 1][p->gridCol + 1].hardness ==0){
	  	  d->dungeon[p->gridRow - 1][p->gridCol + 1].priority = p->priority + 1;
	  	  heap_decrease_key_no_replace(&h, d->dungeon[p->gridRow - 1][p->gridCol + 1].hn);
	  	}
	  	else if(d->dungeon[p->gridRow - 1][p->gridCol + 1].priority > (p->priority + (1 + (p->hardness/85))) && d->dungeon[p->gridRow - 1][p->gridCol + 1].hardness != 0){
	  	  d->dungeon[p->gridRow - 1][p->gridCol + 1].priority = p->priority + (1 + (p->hardness/85));
	  	  heap_decrease_key_no_replace(&h, d->dungeon[p->gridRow - 1][p->gridCol + 1].hn);
	  	}
	  }

	  if(d->dungeon[p->gridRow - 1][p->gridCol - 1].hn){
	  	if(d->dungeon[p->gridRow - 1][p->gridCol - 1].priority > (p->priority + 1) && d->dungeon[p->gridRow - 1][p->gridCol - 1].hardness ==0){
	  	  d->dungeon[p->gridRow - 1][p->gridCol - 1].priority = p->priority + 1;
	  	  heap_decrease_key_no_replace(&h, d->dungeon[p->gridRow - 1][p->gridCol - 1].hn);
	  	}
	  	else if(d->dungeon[p->gridRow - 1][p->gridCol - 1].priority > (p->priority + (1 + (p->hardness/85))) && d->dungeon[p->gridRow - 1][p->gridCol - 1].hardness != 0){
	  	  d->dungeon[p->gridRow - 1][p->gridCol - 1].priority = p->priority + (1 + (p->hardness/85));
	  	  heap_decrease_key_no_replace(&h, d->dungeon[p->gridRow - 1][p->gridCol - 1].hn);
	  	}
	  }

	  if(d->dungeon[p->gridRow + 1][p->gridCol].hn){
	  	if(d->dungeon[p->gridRow + 1][p->gridCol].priority > (p->priority + 1) && d->dungeon[p->gridRow + 1][p->gridCol].hardness ==0){
	  	  d->dungeon[p->gridRow + 1][p->gridCol].priority = p->priority + 1;
	  	  heap_decrease_key_no_replace(&h, d->dungeon[p->gridRow + 1][p->gridCol].hn);
	  	}
	  	else if(d->dungeon[p->gridRow + 1][p->gridCol].priority > (p->priority + (1 + (p->hardness/85))) && d->dungeon[p->gridRow + 1][p->gridCol].hardness != 0){
	  	  d->dungeon[p->gridRow + 1][p->gridCol].priority = p->priority + (1 + (p->hardness/85));
	  	  heap_decrease_key_no_replace(&h, d->dungeon[p->gridRow + 1][p->gridCol].hn);
	  	}
	  }

	  if(d->dungeon[p->gridRow - 1][p->gridCol].hn){
	  	if(d->dungeon[p->gridRow - 1][p->gridCol].priority > (p->priority + 1) && d->dungeon[p->gridRow - 1][p->gridCol].hardness ==0){
	  	  d->dungeon[p->gridRow - 1][p->gridCol].priority = p->priority + 1;
	  	  heap_decrease_key_no_replace(&h, d->dungeon[p->gridRow - 1][p->gridCol].hn);
	  	}
	  	else if(d->dungeon[p->gridRow - 1][p->gridCol].priority > (p->priority + (1 + (p->hardness/85))) && d->dungeon[p->gridRow - 1][p->gridCol].hardness != 0){
	  	  d->dungeon[p->gridRow - 1][p->gridCol].priority = p->priority + (1 + (p->hardness/85));
	  	  heap_decrease_key_no_replace(&h, d->dungeon[p->gridRow - 1][p->gridCol].hn);
	  	}
	  }

	  if(d->dungeon[p->gridRow][p->gridCol + 1].hn){
	  	if(d->dungeon[p->gridRow][p->gridCol + 1].priority > (p->priority + 1) && d->dungeon[p->gridRow][p->gridCol + 1].hardness ==0){
	  	  d->dungeon[p->gridRow][p->gridCol + 1].priority = p->priority + 1;
	  	  heap_decrease_key_no_replace(&h, d->dungeon[p->gridRow][p->gridCol + 1].hn);
	  	}
	  	else if(d->dungeon[p->gridRow][p->gridCol + 1].priority > (p->priority + (1 + (p->hardness/85))) && d->dungeon[p->gridRow][p->gridCol + 1].hardness != 0){
	  	  d->dungeon[p->gridRow][p->gridCol + 1].priority = p->priority + (1 + (p->hardness/85));
	  	  heap_decrease_key_no_replace(&h, d->dungeon[p->gridRow][p->gridCol + 1].hn);
	  	}
	  }

	  if(d->dungeon[p->gridRow][p->gridCol - 1].hn){
	  	if(d->dungeon[p->gridRow][p->gridCol - 1].priority > (p->priority + 1) && d->dungeon[p->gridRow][p->gridCol - 1].hardness ==0){
	  	  d->dungeon[p->gridRow][p->gridCol - 1].priority = p->priority + 1;
	  	  heap_decrease_key_no_replace(&h, d->dungeon[p->gridRow][p->gridCol - 1].hn);
	  	}
	  	else if(d->dungeon[p->gridRow][p->gridCol - 1].priority > (p->priority + (1 + (p->hardness/85))) && d->dungeon[p->gridRow][p->gridCol - 1].hardness != 0){
	  	  d->dungeon[p->gridRow][p->gridCol - 1].priority = p->priority + (1 + (p->hardness/85));
	  	  heap_decrease_key_no_replace(&h, d->dungeon[p->gridRow][p->gridCol - 1].hn);
	  	}
	  }


	}


}

long long getCurrentTime(){
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return (((long long)tv.tv_sec)*1000)+(tv.tv_usec/1000);
}

void print_dead(){
	WINDOW * win = newwin(24, 80, 0, 0);
	mvwprintw(win, 10, 30, "YOU SUCK HAHAHAHA\n");
	mvwprintw(win, 0, 0, "press any key to quit\n");
	wrefresh(win);
	int cd = wgetch(win);
	(void)cd;
}

void print_win(){
	WINDOW * win = newwin(24, 80, 0, 0);
	mvwprintw(win, 10, 30, "you won but you aint that good lol.\n");
	mvwprintw(win, 0, 0, "press any key to quit\n");
	wrefresh(win);
	int cd = wgetch(win);
	(void)cd;

}

void monster_move(Character *m, dungeon *dungeon){
	//intelligent
	if(m->type & (1<<0))
	{

		//This needs to use the shortest path thing that you made last time idk how to use it tbh
		if(m->type & (1<<2))
		{
			if(m->type & (1<<1))
			{
			  Dijkstras_tun(dungeon, dungeon->pc.gridRow, dungeon->pc.gridCol);
				for(int i=-1; i<=1; i++)
				{
					for(int j=-1; j<=1; j++)
					{
						if(dungeon->dungeon[m->gridRow+i][m->gridCol+j].priority<dungeon->dungeon[m->gridRow][m->gridCol].priority&&dungeon->dungeon[m->gridRow+i][m->gridCol+j].hardness!=BORDER_HARDNESS)
						{
							if(dungeon->dungeon[m->gridRow+i][m->gridCol+j].hardness<=85)
							{
								if(m->gridRow+i==(dungeon->pc).gridRow&&m->gridCol+j==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
								for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow+i==(dungeon->characters+k)->gridRow&&m->gridCol+j==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
								dungeon->dungeon[m->gridRow+i][m->gridCol+j].hardness=0;
								m->gridCol+=j;
								m->gridRow+=i;
								dungeon->dungeon[m->gridRow][m->gridCol].character='#';
							}
							else
							{
								dungeon->dungeon[m->gridRow+i][m->gridCol+j].hardness-=85;
							}
							i=j=2;
						}
					}
				}

			}
			else
			{
			  if(m->lastSeenRow!=0)
			  {
			  Dijkstras_tun(dungeon, m->lastSeenRow, m->lastSeenCol);
				for(int i=-1; i<=1; i++)
				{
					for(int j=-1; j<=1; j++)
					{
						if(dungeon->dungeon[m->gridRow+i][m->gridCol+j].priority<dungeon->dungeon[m->gridRow][m->gridCol].priority&&dungeon->dungeon[m->gridRow+i][m->gridCol+j].hardness!=BORDER_HARDNESS)
						{
							if(dungeon->dungeon[m->gridRow+i][m->gridCol+j].hardness<=85)
							{
							if(m->gridRow+i==(dungeon->pc).gridRow&&m->gridCol+j==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
							for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow+i==(dungeon->characters+k)->gridRow&&m->gridCol+j==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
								dungeon->dungeon[m->gridRow+i][m->gridCol+j].hardness=0;
								m->gridCol+=j;
								m->gridRow+=i;
								dungeon->dungeon[m->gridRow][m->gridCol].character='#';
							}
							else
							{
								dungeon->dungeon[m->gridRow+i][m->gridCol+j].hardness-=85;
							}
							i=j=2;
						}
					}
				}
			}
			}
		}
		else
		{
			if(m->type & (1<<1))
			{
			  Dijkstras_nontun(dungeon, dungeon->pc.gridRow, dungeon->pc.gridCol);
				for(int i=-1; i<=1; i++)
				{
					for(int j=-1; j<=1; j++)
					{
						if(dungeon->dungeon[m->gridRow+i][m->gridCol+j].priority<dungeon->dungeon[m->gridRow][m->gridCol].priority)
						{
						if(m->gridRow+i==(dungeon->pc).gridRow&&m->gridCol+j==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
						for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow+i==(dungeon->characters+k)->gridRow&&m->gridCol+j==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
							m->gridCol+=j;
							m->gridRow+=i;
							i=j=2;
						}
					}
				}
			}
			else
			{
			  if(m->lastSeenRow!=0)
			  {
			  Dijkstras_nontun(dungeon, m->lastSeenRow, m->lastSeenCol);
				for(int i=-1; i<=1; i++)
				{
					for(int j=-1; j<=1; j++)

					{
						if(dungeon->dungeon[m->gridRow+i][m->gridCol+j].priority<dungeon->dungeon[m->gridRow][m->gridCol].priority)
						{
						if(m->gridRow+i==(dungeon->pc).gridRow&&m->gridCol+j==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
						for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow+i==(dungeon->characters+k)->gridRow&&m->gridCol+j==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
							m->gridCol+=j;
							m->gridRow+=i;
							i=j=2;
						}
					}
				}
			}
			}
		}
	}
	//telepathic
	if(m->type & (1<<1) && !(m->type & (1<<0)) )
	{
		int pcRow=dungeon->pc.gridRow;
		int pcCol=dungeon->pc.gridCol;
		int moved=0;
		if(pcRow<m->gridRow&&pcCol<m->gridCol&&!moved)
		{
			if(dungeon->dungeon[m->gridRow-1][m->gridCol-1].hardness==0)
			{
			if(m->gridRow-1==(dungeon->pc).gridRow&&m->gridCol-1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow-1==(dungeon->characters+k)->gridRow&&m->gridCol-1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				m->gridCol--;
				m->gridRow--;
				moved=1;
			}
			else if(m->type & (1<<2) && dungeon->dungeon[m->gridRow-1][m->gridCol-1].hardness<=85)
			{
			if(m->gridRow-1==(dungeon->pc).gridRow&&m->gridCol-1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow-1==(dungeon->characters+k)->gridRow&&m->gridCol-1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				dungeon->dungeon[m->gridRow-1][m->gridCol-1].hardness<=0;
				m->gridCol--;
				m->gridRow--;
				moved=1;
			}
			else if(m->type & (1<<2))
			{
				dungeon->dungeon[m->gridRow-1][m->gridCol-1].hardness-=85;
				moved=1;
			}
		}
		if(pcRow>m->gridRow&&pcCol>m->gridCol&&!moved)
		{
			if(dungeon->dungeon[m->gridRow+1][m->gridCol+1].hardness==0)
			{
			if(m->gridRow+1==(dungeon->pc).gridRow&&m->gridCol+1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow+1==(dungeon->characters+k)->gridRow&&m->gridCol+1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				m->gridCol++;
				m->gridRow++;
				moved=1;
			}
			else if(m->type & (1<<2) && dungeon->dungeon[m->gridRow+1][m->gridCol+1].hardness<=85)
			{
			if(m->gridRow+1==(dungeon->pc).gridRow&&m->gridCol+1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow+1==(dungeon->characters+k)->gridRow&&m->gridCol+1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				dungeon->dungeon[m->gridRow+1][m->gridCol+1].hardness=0;
				m->gridCol++;
				m->gridRow++;
				dungeon->dungeon[m->gridRow][m->gridCol].character='#';
				moved=1;
			}
			else if(m->type & (1<<2))
			{
				dungeon->dungeon[m->gridRow+1][m->gridCol+1].hardness-=85;
				moved=1;
			}
		}
		if(pcRow<m->gridRow&&pcCol>m->gridCol&&!moved)
		{
			if(dungeon->dungeon[m->gridRow-1][m->gridCol+1].hardness==0)
			{
			if(m->gridRow-1==(dungeon->pc).gridRow&&m->gridCol+1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow-1==(dungeon->characters+k)->gridRow&&m->gridCol+1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				m->gridCol++;
				m->gridRow--;
				moved=1;
			}
			else if(m->type & (1<<2) && dungeon->dungeon[m->gridRow-1][m->gridCol+1].hardness<=85)
			{
			if(m->gridRow-1==(dungeon->pc).gridRow&&m->gridCol+1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow-1==(dungeon->characters+k)->gridRow&&m->gridCol+1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				dungeon->dungeon[m->gridRow-1][m->gridCol+1].hardness=0;
				m->gridCol++;
				m->gridRow--;
				dungeon->dungeon[m->gridRow][m->gridCol].character='#';
				moved=1;
			}
			else if(m->type & (1<<2))
			{
				dungeon->dungeon[m->gridRow-1][m->gridCol+1].hardness-=85;
				moved=1;
			}
		}
		if(pcRow>m->gridRow&&pcCol<m->gridCol&&!moved)
		{
			if(dungeon->dungeon[m->gridRow+1][m->gridCol-1].hardness==0)
			{
			if(m->gridRow+1==(dungeon->pc).gridRow&&m->gridCol-1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow+1==(dungeon->characters+k)->gridRow&&m->gridCol-1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				m->gridCol--;
				m->gridRow++;
				moved=1;
			}
			else if(m->type & (1<<2) && dungeon->dungeon[m->gridRow+1][m->gridCol-1].hardness<=85)
			{
				dungeon->dungeon[m->gridRow+1][m->gridCol-1].hardness=0;
				m->gridCol--;
				m->gridRow++;
				dungeon->dungeon[m->gridRow][m->gridCol].character='#';
				moved=1;
			}
			else if(m->type & (1<<2))
			{
				dungeon->dungeon[m->gridRow+1][m->gridCol-1].hardness-=85;
				moved=1;
			}
		}
		if(pcCol<m->gridCol&&!moved)
		{
			if(dungeon->dungeon[m->gridRow][m->gridCol-1].hardness==0)
			{
			if(m->gridRow==(dungeon->pc).gridRow&&m->gridCol-1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow==(dungeon->characters+k)->gridRow&&m->gridCol-1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				m->gridCol--;
				moved=1;
			}
			else if(m->type & (1<<2) && dungeon->dungeon[m->gridRow-1][m->gridCol].hardness<=85)
			{
			if(m->gridRow==(dungeon->pc).gridRow&&m->gridCol-1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow==(dungeon->characters+k)->gridRow&&m->gridCol-1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				dungeon->dungeon[m->gridRow][m->gridCol-1].hardness=0;
				m->gridCol--;
				dungeon->dungeon[m->gridRow][m->gridCol].character='#';
				moved=1;
			}
			else if(m->type & (1<<2))
			{
				dungeon->dungeon[m->gridRow][m->gridCol-1].hardness-=85;
				moved=1;
			}
		}
		if(pcRow<m->gridRow&&!moved)
		{
			if(dungeon->dungeon[m->gridRow-1][m->gridCol].hardness==0)
			{
			if(m->gridRow-1==(dungeon->pc).gridRow&&m->gridCol==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow-1==(dungeon->characters+k)->gridRow&&m->gridCol==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				m->gridRow--;
				moved=1;
			}
			else if(m->type & (1<<2) && dungeon->dungeon[m->gridRow-1][m->gridCol].hardness<=85)
			{
			if(m->gridRow-1==(dungeon->pc).gridRow&&m->gridCol==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow-1==(dungeon->characters+k)->gridRow&&m->gridCol==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				dungeon->dungeon[m->gridRow-1][m->gridCol].hardness=0;
				m->gridRow--;
				dungeon->dungeon[m->gridRow][m->gridCol].character='#';
				moved=1;
			}
			else if(m->type & (1<<2))
			{
				dungeon->dungeon[m->gridRow-1][m->gridCol].hardness-=85;
				moved=1;
			}
		}
		if(pcCol>m->gridCol&&!moved)
		{
			if(dungeon->dungeon[m->gridRow][m->gridCol+1].hardness==0)
			{
			if(m->gridRow==(dungeon->pc).gridRow&&m->gridCol+1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow==(dungeon->characters+k)->gridRow&&m->gridCol+1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				m->gridCol++;
				moved=1;
			}
			else if(m->type & (1<<2) && dungeon->dungeon[m->gridRow][m->gridCol+1].hardness<=85)
			{
			if(m->gridRow==(dungeon->pc).gridRow&&m->gridCol+1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow==(dungeon->characters+k)->gridRow&&m->gridCol+1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				dungeon->dungeon[m->gridRow][m->gridCol+1].hardness=0;
				m->gridCol++;
				dungeon->dungeon[m->gridRow][m->gridCol].character='#';
				moved=1;
			}
			else if(m->type & (1<<2))
			{
				dungeon->dungeon[m->gridRow][m->gridCol+1].hardness-=85;
				moved=1;
			}
		}
		if(pcRow>m->gridRow&&!moved)
		{
			if(dungeon->dungeon[m->gridRow+1][m->gridCol].hardness==0)
			{
			if(m->gridRow+1==(dungeon->pc).gridRow&&m->gridCol==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow+1==(dungeon->characters+k)->gridRow&&m->gridCol==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				m->gridRow++;
				moved=1;
			}
			else if(m->type & (1<<2) && dungeon->dungeon[m->gridRow+1][m->gridCol].hardness<=85)
			{
			if(m->gridRow+1==(dungeon->pc).gridRow&&m->gridCol==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow+1==(dungeon->characters+k)->gridRow&&m->gridCol==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				dungeon->dungeon[m->gridRow+1][m->gridCol].hardness=0;
				m->gridRow++;
				dungeon->dungeon[m->gridRow][m->gridCol].character='#';
				moved=1;
			}
			else if(m->type & (1<<2))
			{
				dungeon->dungeon[m->gridRow+1][m->gridCol].hardness-=85;
				moved=1;
			}
		}
	}
	//tunneling
	if(m->type & (1<<2) && !(m->type & (1<<0)))
	{
		int moved=0;
		if(pc_visible(dungeon, m) == -1)
		{
		while(!moved)
		{
		int x=0;
		int y=0;
		while(x==0&&y==0)
		{
			x=rand()%3 -1;
			y=rand()%3 -1;
		}
		if(dungeon->dungeon[m->gridRow+y][m->gridCol+x].hardness==0||(m->type & (1<<2)&&dungeon->dungeon[m->gridRow+y][m->gridCol+x].hardness!=BORDER_HARDNESS))
		{
			if(dungeon->dungeon[m->gridRow+y][m->gridCol+x].hardness==0)
				{
				if(m->gridRow+y==(dungeon->pc).gridRow&&m->gridCol+x==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
				for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow+y==(dungeon->characters+k)->gridRow&&m->gridCol+x==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
					m->gridRow+=y;
					m->gridCol+=x;
					moved=1;
				}
			else if(m->type & (1<<2))
			{
				if(dungeon->dungeon[m->gridRow+y][m->gridCol+x].hardness<=85)
				{
				if(m->gridRow+y==(dungeon->pc).gridRow&&m->gridCol+x==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
				for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow+y==(dungeon->characters+k)->gridRow&&m->gridCol+x==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
					dungeon->dungeon[m->gridRow+y][m->gridCol+x].hardness=0;
					m->gridRow+=y;
					m->gridCol+=x;
					dungeon->dungeon[m->gridRow][m->gridCol].character='#';
					moved=1;
				}
				else
				{
					dungeon->dungeon[m->gridRow+y][m->gridCol+x].hardness-=85;
					moved=1;
				}
			}
		}
		}
		}
		else
		{
		int pcRow=m->lastSeenRow;
		int pcCol=m->lastSeenCol;
		int moved=0;
		if(pcRow<m->gridRow&&pcCol<m->gridCol&&!moved)
		{
			if(dungeon->dungeon[m->gridRow-1][m->gridCol-1].hardness==0)
			{
			if(m->gridRow-1==(dungeon->pc).gridRow&&m->gridCol-1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow-1==(dungeon->characters+k)->gridRow&&m->gridCol-1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				m->gridCol--;
				m->gridRow--;
				moved=1;
			}
			else if(m->type & (1<<2) && dungeon->dungeon[m->gridRow-1][m->gridCol-1].hardness<=85)
			{
			if(m->gridRow-1==(dungeon->pc).gridRow&&m->gridCol-1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow-1==(dungeon->characters+k)->gridRow&&m->gridCol-1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				dungeon->dungeon[m->gridRow-1][m->gridCol-1].hardness=0;
				m->gridCol--;
				m->gridRow--;
				dungeon->dungeon[m->gridRow][m->gridCol].character='#';
				moved=1;
			}
			else if(m->type & (1<<2))
			{
				dungeon->dungeon[m->gridRow-1][m->gridCol-1].hardness-=85;
				moved=1;
			}
		}
		if(pcRow>m->gridRow&&pcCol>m->gridCol&&!moved)
		{
			if(dungeon->dungeon[m->gridRow+1][m->gridCol+1].hardness==0)
			{
			if(m->gridRow+1==(dungeon->pc).gridRow&&m->gridCol-1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow+1==(dungeon->characters+k)->gridRow&&m->gridCol+1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				m->gridCol++;
				m->gridRow++;
				moved=1;
			}
			else if(m->type & (1<<2) && dungeon->dungeon[m->gridRow+1][m->gridCol+1].hardness<=85)
			{
			if(m->gridRow+1==(dungeon->pc).gridRow&&m->gridCol+1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow+1==(dungeon->characters+k)->gridRow&&m->gridCol+1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				dungeon->dungeon[m->gridRow+1][m->gridCol+1].hardness=0;
				m->gridCol++;
				m->gridRow++;
				dungeon->dungeon[m->gridRow][m->gridCol].character='#';
				moved=1;
			}
			else if(m->type & (1<<2))
			{
				dungeon->dungeon[m->gridRow+1][m->gridCol+1].hardness-=85;
				moved=1;
			}
		}
		if(pcRow==m->gridRow&&pcCol<m->gridCol&&!moved)
		{
			if(dungeon->dungeon[m->gridRow][m->gridCol-1].hardness==0)
			{
			if(m->gridRow==(dungeon->pc).gridRow&&m->gridCol-1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow==(dungeon->characters+k)->gridRow&&m->gridCol-1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				m->gridCol--;
				moved=1;
			}
			else if(m->type & (1<<2) && dungeon->dungeon[m->gridRow][m->gridCol-1].hardness<=85)
			{
			if(m->gridRow==(dungeon->pc).gridRow&&m->gridCol-1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow==(dungeon->characters+k)->gridRow&&m->gridCol-1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				dungeon->dungeon[m->gridRow-1][m->gridCol-1].hardness=0;
				m->gridCol--;
				dungeon->dungeon[m->gridRow][m->gridCol].character='#';
				moved=1;
			}
			else if(m->type & (1<<2))
			{
				dungeon->dungeon[m->gridRow][m->gridCol-1].hardness-=85;
				moved=1;
			}
		}
		if(pcRow<m->gridRow&&pcCol==m->gridCol&&!moved)
		{
			if(dungeon->dungeon[m->gridRow-1][m->gridCol].hardness==0)
			{
			if(m->gridRow-1==(dungeon->pc).gridRow&&m->gridCol==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow-1==(dungeon->characters+k)->gridRow&&m->gridCol==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				m->gridRow--;
				moved=1;
			}
			else if(m->type & (1<<2) && dungeon->dungeon[m->gridRow-1][m->gridCol].hardness<=85)
			{
			if(m->gridRow-1==(dungeon->pc).gridRow&&m->gridCol==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow-1==(dungeon->characters+k)->gridRow&&m->gridCol==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				dungeon->dungeon[m->gridRow-1][m->gridCol].hardness=0;
				m->gridRow--;
				dungeon->dungeon[m->gridRow][m->gridCol].character='#';
				moved=1;
			}
			else if(m->type & (1<<2))
			{
				dungeon->dungeon[m->gridRow-1][m->gridCol].hardness-=85;
				moved=1;
			}
		}
		if(pcRow<m->gridRow&&pcCol>m->gridCol&&!moved)
		{
			if(dungeon->dungeon[m->gridRow-1][m->gridCol+1].hardness==0)
			{
			if(m->gridRow-1==(dungeon->pc).gridRow&&m->gridCol+1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow-1==(dungeon->characters+k)->gridRow&&m->gridCol+1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				m->gridCol++;
				m->gridRow--;
				moved=1;
			}
			else if(m->type & (1<<2) && dungeon->dungeon[m->gridRow-1][m->gridCol-1].hardness<=85)
			{
			if(m->gridRow-1==(dungeon->pc).gridRow&&m->gridCol+1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow-1==(dungeon->characters+k)->gridRow&&m->gridCol+1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				dungeon->dungeon[m->gridRow-1][m->gridCol+1].hardness=0;
				m->gridCol++;
				m->gridRow--;
				dungeon->dungeon[m->gridRow][m->gridCol].character='#';
				moved=1;
			}
			else if(m->type & (1<<2))
			{
				dungeon->dungeon[m->gridRow-1][m->gridCol+1].hardness-=85;
				moved=1;
			}
		}
		if(pcRow>m->gridRow&&pcCol<m->gridCol&&!moved)
		{
			if(dungeon->dungeon[m->gridRow+1][m->gridCol-1].hardness==0)
			{
			if(m->gridRow+1==(dungeon->pc).gridRow&&m->gridCol-1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow+1==(dungeon->characters+k)->gridRow&&m->gridCol-1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				m->gridCol--;
				m->gridRow++;
				moved=1;
			}
			else if(m->type & (1<<2) && dungeon->dungeon[m->gridRow+1][m->gridCol-1].hardness<=85)
			{
			if(m->gridRow+1==(dungeon->pc).gridRow&&m->gridCol-1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow+1==(dungeon->characters+k)->gridRow&&m->gridCol-1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				dungeon->dungeon[m->gridRow+1][m->gridCol-1].hardness=0;
				m->gridCol--;
				m->gridRow++;
				dungeon->dungeon[m->gridRow][m->gridCol].character='#';
				moved=1;
			}
			else if(m->type & (1<<2))
			{
				dungeon->dungeon[m->gridRow+1][m->gridCol-1].hardness-=85;
				moved=1;
			}
		}
		if(pcRow==m->gridRow&&pcCol>m->gridCol&&!moved)
		{
			if(dungeon->dungeon[m->gridRow][m->gridCol+1].hardness==0)
			{
			if(m->gridRow==(dungeon->pc).gridRow&&m->gridCol+1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow==(dungeon->characters+k)->gridRow&&m->gridCol+1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				m->gridCol++;
				moved=1;
			}
			else if(m->type & (1<<2) && dungeon->dungeon[m->gridRow-1][m->gridCol-1].hardness<=85)
			{
			if(m->gridRow==(dungeon->pc).gridRow&&m->gridCol+1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow==(dungeon->characters+k)->gridRow&&m->gridCol+1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				dungeon->dungeon[m->gridRow][m->gridCol+1].hardness=0;
				m->gridCol++;
				dungeon->dungeon[m->gridRow][m->gridCol].character='#';
				moved=1;
			}
			else if(m->type & (1<<2))
			{
				dungeon->dungeon[m->gridRow][m->gridCol+1].hardness-=85;
				moved=1;
			}
		}
		if(pcRow>m->gridRow&&pcCol==m->gridCol&&!moved)
		{
			if(dungeon->dungeon[m->gridRow+1][m->gridCol].hardness==0)
			{
			if(m->gridRow+1==(dungeon->pc).gridRow&&m->gridCol==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow+1==(dungeon->characters+k)->gridRow&&m->gridCol==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				m->gridRow++;
				moved=1;
			}
			else if(m->type & (1<<2) && dungeon->dungeon[m->gridRow-1][m->gridCol-1].hardness<=85)
			{
			if(m->gridRow+1==(dungeon->pc).gridRow&&m->gridCol==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow+1==(dungeon->characters+k)->gridRow&&m->gridCol==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				dungeon->dungeon[m->gridRow+1][m->gridCol].hardness=0;
				m->gridRow++;
				dungeon->dungeon[m->gridRow][m->gridCol].character='#';
				moved=1;
			}
			else if(m->type & (1<<2))
			{
				dungeon->dungeon[m->gridRow+1][m->gridCol].hardness-=85;
				moved=1;
			}
		}
		}
	}
	//erratic
	if(m->type & (1<<3))
	{
		if(rand()%1)
		{
		int moved=0;
		while(!moved)
		{
		int x=0;
		int y=0;
		while(x==0&&y==0)
		{
			x=rand()%3 -1;
			y=rand()%3 -1;
		}
		if(dungeon->dungeon[m->gridRow+y][m->gridCol+x].hardness==0||(m->type & (1<<2)&&dungeon->dungeon[m->gridRow+y][m->gridCol+x].hardness!=BORDER_HARDNESS))
		{
			if(dungeon->dungeon[m->gridRow+y][m->gridCol+x].hardness==0)
				{
				if(m->gridRow+y==(dungeon->pc).gridRow&&m->gridCol+x==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
				for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow+y==(dungeon->characters+k)->gridRow&&m->gridCol+x==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
					m->gridRow+=y;
					m->gridCol+=x;
				}
			else if(m->type & (1<<2))
			{
				if(dungeon->dungeon[m->gridRow+y][m->gridCol+x].hardness<=85)
				{
				if(m->gridRow+y==(dungeon->pc).gridRow&&m->gridCol+x==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
				for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow+y==(dungeon->characters+k)->gridRow&&m->gridCol+x==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
					dungeon->dungeon[m->gridRow+y][m->gridCol+x].hardness=0;
					m->gridRow+=y;
					m->gridCol+=x;
					dungeon->dungeon[m->gridRow][m->gridCol].character='#';
					moved=1;
				}
				else
				{
					dungeon->dungeon[m->gridRow+y][m->gridCol+x].hardness-=85;
					moved=1;
				}
			}
		}
		}
		}
	}
	if(m->type==0 ||(m->type & (1<<0)&&m->lastSeenRow==0)||(!(m->type & (1<<0))&&!(m->type & (1<<1))&&!(m->type & (1<<2))&&(m->type & (1<<3))))
	{
		int moved=0;
		//do this however it works with what you are doing
		if(pc_visible(dungeon, m) == -1)
		{
		while(!moved)
		{
		int x=0;
		int y=0;
		while(x==0&&y==0)
		{
			x=rand()%3 -1;
			y=rand()%3 -1;
		}
		if(dungeon->dungeon[m->gridRow+y][m->gridCol+x].hardness==0||(m->type & (1<<2)&&dungeon->dungeon[m->gridRow+y][m->gridCol+x].hardness!=BORDER_HARDNESS))
		{
			if(dungeon->dungeon[m->gridRow+y][m->gridCol+x].hardness==0)
				{
				if(m->gridRow+y==(dungeon->pc).gridRow&&m->gridCol+x==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
				for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow+y==(dungeon->characters+k)->gridRow&&m->gridCol+x==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
					m->gridRow+=y;
					m->gridCol+=x;
					moved=1;
				}
			else if(m->type & (1<<2))
			{
				if(dungeon->dungeon[m->gridRow+y][m->gridCol+x].hardness<=85)
				{
				if(m->gridRow+y==(dungeon->pc).gridRow&&m->gridCol+x==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
				for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow+y==(dungeon->characters+k)->gridRow&&m->gridCol+x==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
					dungeon->dungeon[m->gridRow+y][m->gridCol+x].hardness=0;
					m->gridRow+=y;
					m->gridCol+=x;
					dungeon->dungeon[m->gridRow][m->gridCol].character='#';
					moved=1;
				}
				else
				{
					dungeon->dungeon[m->gridRow+y][m->gridCol+x].hardness-=85;
					moved=1;
				}
			}
		}
		}
		}
		else
		{
		int pcRow=m->lastSeenRow;
		int pcCol=m->lastSeenCol;
		if(pcRow<m->gridRow&&pcCol<m->gridCol&&!moved)
		{
			if(dungeon->dungeon[m->gridRow-1][m->gridCol-1].hardness==0)
			{
			if(m->gridRow-1==(dungeon->pc).gridRow&&m->gridCol-1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow-1==(dungeon->characters+k)->gridRow&&m->gridCol-1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				m->gridCol--;
				m->gridRow--;
				moved=1;
			}
		}
		if(pcRow>m->gridRow&&pcCol>m->gridCol&&!moved)
		{
			if(dungeon->dungeon[m->gridRow+1][m->gridCol+1].hardness==0)
			{
			if(m->gridRow+1==(dungeon->pc).gridRow&&m->gridCol+1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow+1==(dungeon->characters+k)->gridRow&&m->gridCol+1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				m->gridCol++;
				m->gridRow++;
				moved=1;
			}
		}
		if(pcRow<m->gridRow&&pcCol>m->gridCol&&!moved)
		{
			if(dungeon->dungeon[m->gridRow-1][m->gridCol+1].hardness==0)
			{
			if(m->gridRow-1==(dungeon->pc).gridRow&&m->gridCol+1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow-1==(dungeon->characters+k)->gridRow&&m->gridCol+1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				m->gridCol++;
				m->gridRow--;
				moved=1;
			}
		}
		if(pcRow>m->gridRow&&pcCol<m->gridCol&&!moved)
		{
			if(dungeon->dungeon[m->gridRow+1][m->gridCol-1].hardness==0)
			{
			if(m->gridRow+1==(dungeon->pc).gridRow&&m->gridCol-1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow+1==(dungeon->characters+k)->gridRow&&m->gridCol-1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				m->gridCol--;
				m->gridRow++;
				moved=1;
			}
		}
		if(pcCol<m->gridCol&&!moved)
		{
			if(dungeon->dungeon[m->gridRow][m->gridCol+1].hardness==0)
			{
			if(m->gridRow==(dungeon->pc).gridRow&&m->gridCol+1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow==(dungeon->characters+k)->gridRow&&m->gridCol+1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				m->gridCol++;
				moved=1;
			}
		}
		if(pcRow>m->gridRow&&!moved)
		{
			if(dungeon->dungeon[m->gridRow+1][m->gridCol].hardness==0)
			{
			if(m->gridRow+1==(dungeon->pc).gridRow&&m->gridCol==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow+1==(dungeon->characters+k)->gridRow&&m->gridCol==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				m->gridRow++;
				moved=1;
			}
		}
		if(pcCol<m->gridCol&&!moved)
		{
			if(dungeon->dungeon[m->gridRow][m->gridCol-1].hardness==0)
			{
			if(m->gridRow==(dungeon->pc).gridRow&&m->gridCol-1==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow==(dungeon->characters+k)->gridRow&&m->gridCol+1==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				m->gridCol--;
				moved=1;
			}
		}
		if(pcRow<m->gridRow&&!moved)
		{
			if(dungeon->dungeon[m->gridRow-1][m->gridCol].hardness==0)
			{
			if(m->gridRow-1==(dungeon->pc).gridRow&&m->gridCol==(dungeon->pc).gridCol)
								{
									///dungeon->pc.health-=m->Damage.roll();
								}
			for(int k=1; k<dungeon->monstersAlive; k++)
								{
								if(m->gridRow+1==(dungeon->characters+k)->gridRow&&m->gridCol==(dungeon->characters+k)->gridCol)
								{
									(dungeon->characters+k)->gridRow=m->gridRow;
									(dungeon->characters+k)->gridCol=m->gridCol;
								}
								}
				m->gridRow--;
				moved=1;
			}
		}
	}
	}


}

int nextTurn(dungeon *d, heap_t *h) {
  Character *c = (Character*)heap_peek_min(h);
	if(c->nextTurn <= d->turn&&c->dead!=1) { //if a charactor has a turn on the current turn, assuming datum not Pointer
	if(c->typeofChar != '@') {
		monster_move(c, d);
		for(int i=0; i<d->numOfCharacters; i++)
		{
			if((d->characters+i)->gridRow==c->gridRow&&(d->characters+i)->gridCol==c->gridCol&&(d->characters+i)!=c)
			{
				(d->characters+i)->dead=1;
			}
		}
		if(d->pc.gridRow==c->gridRow&&d->pc.gridCol==c->gridCol)
		{
			(d->characters)->dead=1;
		}
	}
	//if its pc's turn
	else{
		while(1){
		  //pc_move will return 0 if move is invalid or doing something other than move
		  if(pc_move(d) != 0){
			  erase();
				break;
		}
		}
	}
	//debug
	//printf("Char to be moved: %c\n", c->typeofChar);
	//debug

	// Assuming nothing died: removing from queue, updating nextTurn, adding to queue, incrementing turn
	heap_remove_min(h);
	c->nextTurn = d->turn + (1000/c->speed);
	heap_insert(h, c);
	d->turn++;
	return 1;
	} else {
	if(c->dead==1)
	{
		heap_remove_min(h);
	}
	}
	d->turn++;

	//debug
	//printf("Curr turn: %d\n", d->turn);
	//debug

	return 0;
}

void game_loop(dungeon *d, heap_t *h){
	initscr();
	raw();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);
  start_color();
  init_pair(COLOR_BLACK, COLOR_WHITE, COLOR_BLACK);
  init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
  init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
  init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
  init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
	while((d->characters)->dead != 1 && d->monstersAlive > 0){
		nextTurn(d, h);
			for(int j=0; j<d->monsDes.size(); j++)
			{
			if(((d->characters+j)->type & 1<<8)&& (d->characters+j)->dead == 1)
			{
			print_win();
			}
		}
	}
	if((d->characters)->dead == 1)
		{
		print_dead();
		}
	endwin();
	exit(0);
}

void getMonstDescrip(std::vector<MonsterDescription> &v){
	std::string home = getenv("HOME");
	std::ifstream monsterFile(home+"/.rlg327/monster_desc.txt"); //read from txt file
	std::string name;
	std::string currentline;
	int success = 1; //if there is an error in the text file the monster description will be discarded
	getline(monsterFile, name);
	if(name != "RLG327 MONSTER DESCRIPTION 1") //making sure its a description file
			return;
	while(monsterFile){ //while the text file still has lines
		getline(monsterFile, currentline);
		if(currentline == "BEGIN MONSTER"){
			MonsterDescription currentMon;
			while(getline(monsterFile, currentline)){
					std::vector<std::string> line;//list of words parsed by space
					std::stringstream ss(currentline);
					std::string item;
					while(getline(ss, item, ' '))
							line.push_back(item);
					if(line[0] == "NAME"){
						for(int i = 1; i < (int)line.size(); i++){
							currentMon.name.push_back(line[i]);
						}
						currentMon.name.push_back("\n");
					}
					else if(line[0] == "COLOR"){
						for(int i = 1; i < (int)line.size(); i++){
							if(line[i] == "BLACK"){
									currentMon.colorBits.push_back(COLOR_BLACK);
								}
							else if(line[i] == "BLUE"){
									currentMon.colorBits.push_back(COLOR_BLUE);
								}
							else if(line[i] == "GREEN"){
									currentMon.colorBits.push_back(COLOR_GREEN);
								}
							else if(line[i] == "CYAN"){
									currentMon.colorBits.push_back(COLOR_CYAN);
								}
							else if(line[i] == "RED"){
									currentMon.colorBits.push_back(COLOR_RED);
								}
							else if(line[i] == "MAGENTA"){
									currentMon.colorBits.push_back(COLOR_MAGENTA);
								}
							else if(line[i] == "YELLOW"){
									currentMon.colorBits.push_back(COLOR_YELLOW);
								}
							else if(line[i] == "WHITE"){
									currentMon.colorBits.push_back(COLOR_WHITE);
								}
							currentMon.color.push_back(line[i]);
						}
						currentMon.color.push_back("\n");
					}
					else if(line[0] == "SYMB"){
							currentMon.symbol = line[1] + "\n";
					}
					else if(line[0] == "DESC"){
							while(getline(monsterFile, currentline)){
								if(currentline == ".")
									break;
								currentMon.description += currentline;
								currentMon.description += "\n";
							}
					}
					else if(line[0] == "SPEED"){
						std::vector<std::string> diceLine;
						std::stringstream dl(line[1]);
						std::string item;
						while(getline(dl, item, '+'))
								diceLine.push_back(item);
						currentMon.speedtest.set_base(std::stoi(diceLine[0]));
						std::vector<std::string> dSplit;
						std::stringstream ds(diceLine[1]);
						std::string both;
						while(getline(ds, both, 'd'))
								dSplit.push_back(both);
						currentMon.speedtest.set_number(std::stoi(dSplit[0]));
						currentMon.speedtest.set_sides(std::stoi(dSplit[1]));
					}
					else if(line[0] == "DAM"){
						std::vector<std::string> diceLine;
						std::stringstream dl(line[1]);
						std::string item;
						while(getline(dl, item, '+'))
								diceLine.push_back(item);
						currentMon.Damage.set_base(std::stoi(diceLine[0]));
						std::vector<std::string> dSplit;
						std::stringstream ds(diceLine[1]);
						std::string both;
						while(getline(ds, both, 'd'))
								dSplit.push_back(both);
						currentMon.Damage.set_number(std::stoi(dSplit[0]));
						currentMon.Damage.set_sides(std::stoi(dSplit[1]));
					}
					else if(line[0] == "HP"){
						std::vector<std::string> diceLine;
						std::stringstream dl(line[1]);
						std::string item;
						while(getline(dl, item, '+'))
								diceLine.push_back(item);
						currentMon.hitpoints.set_base(std::stoi(diceLine[0]));
						std::vector<std::string> dSplit;
						std::stringstream ds(diceLine[1]);
						std::string both;
						while(getline(ds, both, 'd'))
								dSplit.push_back(both);
						currentMon.hitpoints.set_number(std::stoi(dSplit[0]));
						currentMon.hitpoints.set_sides(std::stoi(dSplit[1]));
					}
					else if(line[0] == "ABIL"){
							currentMon.abilBits = 0;
							for(int i = 1; i < (int)line.size(); i++){
								if(line[i] == "SMART"){ //first bit
									currentMon.abilBits += 1;
								}
								else if(line[i] == "TELE"){//second bit
									currentMon.abilBits += 2;
								}
								else if(line[i] == "TUNNEL"){
									currentMon.abilBits += 4;
								}
								else if(line[i] == "ERRATIC"){
									currentMon.abilBits += 8;
								}
								else if(line[i] == "PASS"){
									currentMon.abilBits += 16;
								}
								else if(line[i] == "PICKUP"){
									currentMon.abilBits += 32;
								}
								else if(line[i] == "DESTROY"){
									currentMon.abilBits += 64;
								}
								else if(line[i] == "UNIQ"){
									currentMon.abilBits += 128;
								}
								else if(line[i] == "BOSS"){
									currentMon.abilBits += 256;
								}
								currentMon.abilities.push_back(line[i]);
							}
							currentMon.abilities.push_back("\n");
					}
					else if(line[0] == "RRTY"){
							currentMon.rarity = std::stoi(line[1]);
					}
					else if(line[0] == "END"){
						break;
					}
					else{
						success = 0;
						break;
					}
				}
				if(success){
					currentMon.placed = 0;
					v.push_back(currentMon);
				}
			}
		}
	monsterFile.close();
}

int main(int argc, char *argv[]){
	dungeon cellDungeon;
  int numofmonsters;
  //intializing cell dungeon positions
  for(int i = 0; i < ROWS; i++){
    for(int j = 0; j < COLS; j++){
      cellDungeon.dungeon[i][j].gridRow = i;
      cellDungeon.dungeon[i][j].gridCol = j;
    }

  }

  //cellDungeon.pc.damage.set(1,1,4);
  cellDungeon.pc.playerChar = '@';
  cellDungeon.turn = 0;
  //will default to 10 if no switch
  numofmonsters = MAX_MONSTERS;
  if(argc>1 && strcmp(argv[1], "--nummon")==0){
    	numofmonsters = atoi(argv[2]);
   }
    	//mallocing for monsters array and total characters array
   cellDungeon.characters = (Character*) malloc((numofmonsters + 2) * sizeof(Character));
	//first character will always be the PC followed by the monsters
   cellDungeon.numOfCharacters = numofmonsters + 1;
	//initializing the dungeon
  parse_descriptions(&cellDungeon); //this hoe work
  getMonstDescrip(cellDungeon.monsDes); //THIS LINE IS STUPIDBFGJSKLABDFLJKASBFL
  init_dungeon(&cellDungeon); // i confusion
  heap_t moveQueue = movePrioQueueInit(&cellDungeon); //stores when characters will move
  game_loop(&cellDungeon, &moveQueue); //the actual game
  free(cellDungeon.characters); //freeing the memory
  
  return 0;
}
