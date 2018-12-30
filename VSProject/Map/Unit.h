#pragma once

#include <vector>

#define INSTRUCTION_NEW_INSTRUCTION 0
#define INSTRUCTION_GIVE_RESOURCE 1
#define INSTRUCTIONS (RESOURCE_TYPES * 2)

#define LIFE 220

#define MAX_CYCLE_LENGTH 15

#define PATH_NOT_FOUND -1
#define PATH_OUTDATED -2

#ifdef DEBUG
extern int debug_unitCount;
#endif

class Area;

class Unit : public Object{
private:
	bool checkReachedResource();
	bool checkReachedResourceGive();
	bool checkReachedResourceSearch();
	void pursueResource();
	void createUnit();

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
	char resourceType;

	int life;

	bool hate[RESOURCE_TYPES];

	int slowness;//The amount of turns the unit has to wait after moving

	Unit* parent;
	int childs;
	Object* destinationObject;//The unit can be moving towards its parent
	
	//Whether the unit is is a bucket or not
	//If the last child of a dead unit dies, the unit can be deleted only if it is not in a bucket
	//If it is in a bucket, the main loop will take care of deleting it
	bool inBucket;
	
	char cycle[MAX_CYCLE_LENGTH];
	char cycleLength;
	char cycleCurrentStep;

	//If this var is less than RESOURCE_TYPES, then it measn the unit is looking for that resource
	//If it is grater, then it means the unit is trying to give that resource
	char resourceSearchStatus;

	std::vector<char> bag;

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

	Unit(int _x, int _y, int _z, int _life, Unit* _parent, int _resourceType);
	~Unit();
	void execute();
	void addRandomStepToCycle();
	void initializeStep();
	void nextStep();
	unsigned char type();
	void initializeUnit();
	//void adjustResourceType();
	int calculateWorth();
	void newInstruction();
	void giveResource(Unit* taker);
	void aquireResource();
	void addToTileExtra();
	void removeFromTileExtra();
	bool providesResource(char _resourceType);
};