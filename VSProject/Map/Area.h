#pragma once

#include <list>

class Chunk;

extern int debug_areasCreated;
extern int debug_areasDestroyed;

class Area
{
public:
	int lvl;
	Chunk * chunk;
	Area * superArea;
	vector<Area*> adjacentAreas;
	int resources[RESOURCE_TYPES * 3];

	//These variable is only used by the pathfinding algorithm
	Unit* lastCheckedByUnit;
	int lastCheckedAtTime;
	Area* pathFindingBranch;

#   ifdef DEBUG
	int areaId;
#	endif

	Area();
	Area(Chunk * _chunk);
	~Area();
	void increaseResource(int resource);
	void decreaseResource(int resource);
};
