#pragma once

#include "Unit.h"
#include "Map.h"

extern Unit* tileWasLastCheckedByUnit[MAP_WIDTH][MAP_WIDTH][MAP_HEIGHT];//It indicates the last unit that checked a tile
extern int tileWasLastCheckedAtTime[MAP_WIDTH][MAP_WIDTH][MAP_HEIGHT];//It indicates the last time a tile was checked
extern Area * areasMap[MAP_WIDTH][MAP_WIDTH][MAP_HEIGHT];//It indicates the area to which each tile belongs

extern Chunk**** chunks[LEVELS];

struct PFTile {
	int x, y, z;
	int direction;
};

extern PFTile circularArrayOfTiles[CIRCULAR_ARRAY_OF_TILES];
extern Area* circularArrayOfAreas[CIRCULAR_ARRAY_OF_AREAS];
extern int circularArrayStart;
extern int circularArrayEnd;

extern Area destTileAuxArea;//This is a single tile area used to mark the destination when the unit reaches the last chunk

extern Area* oriAreas[LEVELS - 1];

void pathfinderInitialize();
void pathfinderClose();
void connectAreas(Area* area1, Area* area2);

/*
int resetPath(Unit* unit);
int adjustPath(Unit* unit);
bool checkDummyMovementTowardsPoint(int oriX, int oriY, int oriZ, int destX, int destY, int destZ);
bool checkDummyMovementTowardsArea(int oriX, int oriY, int oriZ, Area* dest);
int dummyMovementTowardsPoint(int oriX, int oriY, int oriZ, int destX, int destY, int destZ);
int dummyMovementTowardsArea(int oriX, int oriY, int oriZ, Area* dest);
int dijkstraTowardsPoint(Unit* unit, bool adjusting);
int dijkstraTowardsArea(Unit* unit, Area* dest, bool adjusting);*/