#pragma once

#include <list>
#include "Parameters.h"
#include "Unit.h"

const unsigned char tileGround = 0;//The first 128 values are the steppable terrains
const unsigned char tileGenerator = 1;
const unsigned char tileEmpty = 128;//The last 128 values are the non-steppable terrains
const unsigned char tileWater = 129;

extern unsigned char tilesMap[MAP_WIDTH][MAP_WIDTH][MAP_HEIGHT];

//Units have a pointer the the next unit if there is more than one unit in the same tile
extern Object* unitsMap[MAP_WIDTH][MAP_WIDTH][MAP_HEIGHT];

extern int time;

extern std::list<Object*> objects[BUCKETS];

extern int viewX, viewY, viewZ;

void map_init();

void execute_frame();

void map_close();

bool tileIsSteppable(const int& x, const int& y, const int& z);

int steppableTileHeight(const int& x, const int& y, const int& z, const int& nextX, const int& nextY);

//Map Creation
void createTerrainWithMidPointDisplacement();