#include "Common.h"

#ifdef DEBUG
int debug_unitCount = 0;
#endif

Unit::Unit(int _x, int _y, int _z, int _life) : Object(objectUnit, _x, _y, _z)
{
	life = _life;
	parent = NULL;
	childs = 0;
	inBucket = true;
	destinationObject = NULL;

	for (int i = 0; i < LEVELS - 1; i++) {
		destinationSuperAreas[i] = NULL;
	}
	slowness = 2 + rand() % 6;

	objects[(time + (rand() % slowness)) % BUCKETS].push_back(this);

#	ifdef DEBUG

	baseDestinationArea = NULL;
	hasToResetPath = true;
#	endif

#	ifdef DEBUG
	debug_unitCount++;

	if (!tileIsSteppable(x, y, z)) {
		error("A unit can only occupy steppable tiles");
	}
#	endif
}

void Unit::initializeUnit() {
	initializeStep();
	addToTile();
}

Unit::~Unit(){
#	ifdef DEBUG
	if (childs > 0) {
		error("Do not delete a unit if it still has childs");
	}

	debug_unitCount--;
#	endif
	if (parent != NULL) {
		parent->childs--;
		if (parent->childs == 0 && !parent->inBucket) {
			delete parent;
		}
	}
}

void Unit::addToTileExtra() {
	Area* area = areasMap[x][y][z];
	nearZones[x / NEAR_ZONE_DISTANCE][y / NEAR_ZONE_DISTANCE].addUnit(this);
	if (resourceSearchStatus != -1) {
		if (resourceSearchStatus < RESOURCE_TYPES) {
			area->increaseResource(RESOURCE_TYPES + resourceSearchStatus);
		}
		else {
			area->increaseResource(resourceSearchStatus - RESOURCE_TYPES);
		}
	}
}

void Unit::removeFromTileExtra() {
	Area* area = areasMap[x][y][z];
	nearZones[x / NEAR_ZONE_DISTANCE][y / NEAR_ZONE_DISTANCE].removeUnit(this);
	if (resourceSearchStatus != -1) {
		if (resourceSearchStatus < RESOURCE_TYPES) {
			//The unit was looking for a resource
			area->decreaseResource(RESOURCE_TYPES + resourceSearchStatus);
		}
		else {
			//The unit was trying to give a resource
			area->decreaseResource(resourceSearchStatus - RESOURCE_TYPES);
		}
	}
}

unsigned char Unit::type() {
	return objectUnit;
}

void Unit::execute() {
#	ifdef DEBUG
	if (!tileIsSteppable(x, y, z)) {
		error("A unit can only occupy steppable tiles");
	}
	if (life < 0) {
		error("A unit can not have less than 0 life");
	}
#	endif
	
	removeFromTile();

	life--;

	if (life <= 0) {
		if (rand() % 2 == 0) {
			life = LIFE;
			randomModification();
		}
		else {
			alive = false;
			return;
		}
	}

	destinationObject = findNearEnemy();

	pursueResource();

	if (alive) {
		addToTile();
		objects[(time + slowness) % BUCKETS].push_back(this);
	}
}

void Unit::randomModification() {
	re implementar

	int position = rand() % (cycleLength + 1);
	for (int i = cycleLength; i > position; i--) {
		cycle[i] = cycle[i - 1];
	}
	int instruction;
	bool correct = false;
	while(!correct){
		instruction = (resourceType + rand() % (INSTRUCTIONS - 1) + 1) % INSTRUCTIONS;
		if (instruction < RESOURCE_TYPES) {
			correct = true;
		}
		if (instruction >= RESOURCE_TYPES) {
			if (position > 0) {
				if (cycle[position - 1] < RESOURCE_TYPES) {
					correct = true;
				}
			}
		}
	}
	cycle[position] = instruction;

	cycleLength++;
	if (position <= cycleCurrentStep) {
		cycleCurrentStep++;
	}
}

Unit* Unit::findNearEnemy() {
	Unit* nearestEnemy = NULL;
	int nearestDistance = NEAR_ZONE_DISTANCE;
	int xB = x / NEAR_ZONE_DISTANCE - 1;
	if (xB < 0) {
		xB = 0;
	}
	int xE = x / NEAR_ZONE_DISTANCE + 1;
	if (xE >= MAP_WIDTH / NEAR_ZONE_DISTANCE) {
		xE = MAP_WIDTH / NEAR_ZONE_DISTANCE - 1;
	}
	int yB = y / NEAR_ZONE_DISTANCE - 1;
	if (yB < 0) {
		yB = 0;
	}
	int yE = y / NEAR_ZONE_DISTANCE + 1;
	if (yE >= MAP_WIDTH / NEAR_ZONE_DISTANCE) {
		yE = MAP_WIDTH / NEAR_ZONE_DISTANCE - 1;
	}
	for (int i = xB; i <= xE; i++) {
		for (int j = yB; j <= yE; j++) {
			for (int k = 0; k < nearZones[i][j].units.size(); k++) {
				Unit* unit = nearZones[i][j].units[k];
				if (master() != unit->master()) {
					int dist = abs(x - unit->x) + abs(y - unit->y);
					if (dist < nearestDistance) {
						nearestDistance = dist;
						nearestEnemy = unit;
					}
				}
			}
		}
	}
	return nearestEnemy;
}