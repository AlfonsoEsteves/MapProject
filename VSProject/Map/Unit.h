#pragma once

#include <vector>

#define LIFE 220

#define PATH_NOT_FOUND -1
#define PATH_OUTDATED -2

#define SEARCH_TIME_OUT 120

#ifdef DEBUG
extern int debug_unitCount;
#endif

class Area;

class Unit : public Object{
private:
	//Pathfinding towards point
	int resetPathTowardsObject();
	int adjustPathTowardsObject();
	int resetPathGoingFromAreaToArea(Area * oriArea, Area * destArea);
	int resetPathGoingFromAreasToSuperAreas(int startingLevel);
	Area* findNextAreaAux(Area * oriArea, Area * destArea, bool adjusting, bool subArea);
	int dijkstraTowardsObjectOrArea(bool adjusting);
	int dijkstraTowardsObject(bool adjusting);
	int dijkstraTowardsArea(Area * dest, bool adjusting);

	//Pathfinding towards resource
	int resetPathTowardsResource();
	int adjustPathTowardsResource();
	int resetPathTowardsResource(int startingMacroLevel);
	Area* findNextAreaTowardsResourceOrSuperArea(Area * oriArea, Area * destArea, bool adjusting);
	int resetPathTowardsResourceOrDestinationSuperArea(int startingMacroLevel);
	int dijkstraTowardsResourceOrArea(bool adjusting);

#	ifdef DEBUG
	void checkDestinationAreas();
#	endif

#	ifdef LOG_PF
	void printPath();
#	endif

public:
	int life;

	int slowness;//The amount of turns the unit has to wait after moving

	Unit* parent;
	int childs;

	//This can be used to go to a specific point
	//For instance going to an enemy
	Unit* destinationObject;
	
	//Whether the unit is in a bucket or not
	//If the last child of a dead unit dies, the unit can be deleted only if it is not in a bucket
	//If it is in a bucket, the main loop will take care of deleting it
	bool inBucket;

	bool consuming;//Indicates if the unit is looking for a resource to consume it or to store it
	char storingResource;
	char searching1;
	char searching2;

	char carrying;

	char desiredResources[RESOURCE_CATEGORIES];

	int searchTime;

	int seed;

	//Pathfinding vars
	bool hasToResetPath;
	bool reachedDestinationBaseArea;
	bool movingTowardsPoint;

	//This array indicates the destination super area of the origin areas
	//For instance, the first level indicates the destination kilo-area from the origin base-area
	Area* destinationSuperAreas[LEVELS - 1];
	//This array indicates the amount of chunks needed to be traveled to reach the destination super-area
	//For instance, the first level indicates the amount of base-chunks to reach the destination kilo-area
	int chunksToBeTraveled[LEVELS - 1];
	//These two variable are similar the the previous arrays, but for the tile-level-movement
	Area* baseDestinationArea;
	int tilesToBeTraveled;
	//The amount of chunks and tiles needed to be traveled are used to detect whether a blocking obstacle appeared
	//  If so, th eunit should recalculate its path

	//The lowest of the destination areas the unit reached
	//Note that if the unit leaves the area (because it found a shortcut) this veriable keeps its value
	Area * lowestDestinationAreaReached;

	//bool dummyMovementSeemsFine;

	Unit(int _x, int _y, int _z, int _life, int parentSeed);
	~Unit();
	void execute();
	unsigned char type();
	void addToTileExtra();
	void removeFromTileExtra();
	Unit* findNearObjective();
	Unit* master();
	void resetActivity(bool _consuming);
	void detachFromParent();
	bool isOrphan();
	bool isEnemyOf(Unit* unit);
	bool checkReachedGoal();
	bool checkReachedResourceSearch();
	void pursueGoal();
};