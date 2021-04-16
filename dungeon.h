#ifndef DUNGEON_H
#define DUNGEON_H

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>

#include "heap.h"
#include "dice.h"
#include "object_descriptions.h"

#define ROWS 21
#define COLS 80
#define MAX_ROOMS 6
#define MAX_STAIRS 3
#define BORDER_HARDNESS 255
#define ROCK_HARDNESS 250
#define FLOOR_HARDNESS 0
#define MAX_MONSTERS 10
#define PC_SPEED 10
#define VEIW_RADIUS 2
#define MAX_OBJECTS 10
#define PC_COLOR 7
#define PC_INVENTORY_SIZE 22
#define PC_EQUIPMENT_SLOTS 12

class MonsterDescription
{
public:
	std::vector<std::string> name;
	std::string description;
	std::vector<std::string> color; //for printing
	std::vector<int> colorBits;		//storing type of colors
	std::string speed;
	dice speedtest;
	std::vector<std::string> abilities; //for printing
	int abilBits;						//order of abilities in bit order from largest bit to smallest: BOSS|UNIQ|DESTROY|PICKUP|PASS|ERATIC|TUNNEL|TELE|SMART
	std::string HP;
	dice hitpoints;
	std::string attackDamage;
	dice Damage;
	std::string symbol;
	int rarity;
	int placed;
};

class Object
{
public:
	std::string name; //to see if there is another instance
	int color;
	dice Damage;
	char symbol;
	object_type_t type;
	int hit;
	int dodge;
	int defense;
	int weight;
	int speed;
	int attribute;
	int value;
	int rarity;
	bool artifact;
	int gridRow;
	int gridCol;
	bool pickedUp;
	std::string description;
};

class Character
{
public:
	int dead; //0=alive 1=dead
	int speed;
	char typeofChar;
	int gridRow;
	int gridCol;
	int nextTurn;
	int sequenceNum;
	int type; //order of abilities in bit order from largest bit to smallest: BOSS|UNIQ|DESTROY|PICKUP|PASS|ERATIC|TUNNEL|TELE|SMART
	//last known place of pc
	int lastSeenRow;
	int lastSeenCol;
	std::vector<int> colorBits; //storing type of colors
	int hitpoints;
	dice Damage;
	int rarity;
	std::string description;
};
//this will contain all the info for a cell in the dungeon
class cell
{
public:
	u_int8_t hardness;
	int priority;
	char character;
	heap_node_t *hn;
	int gridRow;
	int gridCol;
};
//struct for a room
class room
{
public:
	int rows;
	int cols;
	int gridRow;
	int gridCol;
};

//player character
class PC
{
public:
	uint8_t gridRow;
	uint8_t gridCol;
	char playerChar;
	int nextTurn;
	int sequenceNum;
	heap_node_t *hn;
	int health=30;
	dice damage;
};

//struct for stairs to store where placed on grid
class stairs
{
public:
	int gridRow;
	int gridCol;
	char direction;
};

//struct for the dungeon
class dungeon
{
public:
	room Rooms[MAX_ROOMS];
	int dungeonGrid[ROWS][COLS];
	cell dungeon[ROWS][COLS];
	stairs Stairs[MAX_STAIRS];
	Character *characters;
	PC pc;
	char revelaedBoard[ROWS][COLS];
	int turn;
	int numOfCharacters;
	int monstersAlive;
	std::vector<MonsterDescription> monsDes;
	std::vector<object_description> object_descriptions;
	bool fullDungeon;
	std::vector<Object> objects;
	std::vector<Object*> pcInv;
};

class BoldBoard
{
public:
	char board[ROWS][COLS];
	bool bold[ROWS][COLS];
	int color[ROWS][COLS];
};

#endif
