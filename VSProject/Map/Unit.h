#pragma once

#include <vector>

#define INSTRUCTION_DUPLICATE RESOURCE_TYPES 
#define INSTRUCTION_NEW_INSTRUCTION (RESOURCE_TYPES + 1)
#define INSTRUCTION_GIVE_RESOURCE (RESOURCE_TYPES + 2)
#define INSTRUCTIONS (RESOURCE_TYPES + 2)

#define LIFE 220

#define CHANCES_OF_ADDING_A_STEP 6

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
	void pursueResource();
	void createUnit();

	//Pathfinding towards point
	int resetPathTowardsPoint(int destX, int destY, int destZ);
	int adjustPathTowardsPoint(int destX, int destY, int destZ);
	int resetPathTo(Area * oriArea, Area * destArea);
	int resetPathGoingFromAreasToSuperAreas(int startingLevel);
	Area* findNextAreaAux(Area * oriArea, Area * destArea, bool adjusting, bool subArea);
	int dijkstraTowardsPoint(int destX, int destY, int destZ, bool adjusting);
	int dijkstraTowardsArea(Area * dest, bool adjusting);

	//Pathfinding towards resource
	int resetPathTowardsResource();
	int adjustPathTowardsResource();
	int dijkstraTowardsResourceOrArea(bool adjusting);
	Area* findNextAreaTowardsResourceOrSuperArea(Area * oriArea, Area * destArea, bool adjusting);
	int resetPathTowardsResource(int startingMacroLevel);
	int resetPathTowardsResourceOrDestinationSuperArea(int startingMacroLevel);

#	ifdef DEBUG
	void checkDestinationAreas();
#	endif

#	ifdef LOG_PF
	void printPath();
#	endif

public:
	int life;

	int slowness;//The amount of turns the unit has to wait after moving
	
	char cycle[MAX_CYCLE_LENGTH];
	char cycleLength;
	char cycleCurrentStep;

	char lookingForResource;

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

	Unit(int _x, int _y, int _z, int _life);
	~Unit();
	void execute();
	void modifyCycle(int chancesOfDecreasing);
	void checkIfPathfindingResetIsNeeded();
	unsigned char type();
	void adjustResourceType();
	bool hasDuplicate();
	int calculateWorth();
	void newInstruction();
};