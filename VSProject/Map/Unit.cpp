#include "Common.h"

#ifdef DEBUG
int debug_unitCount = 0;
#endif

Unit::Unit(int _x, int _y, int _z, int _life, int parentSeed) : Object(objectUnit, _x, _y, _z)
{
	life = _life;
	parent = NULL;
	childs = 0;
	inBucket = true;
	destinationObject = NULL;

	if (parentSeed == -1) {
		seed = rand() % MAX_SEED;
	}
	else {
		seed = randFunction(parentSeed);
	}

	int aux = seed;
	for (int i = 0; i < RESOURCE_CATEGORIES;i++) {
		aux = randFunction(aux);
		desiredResources[i] = aux % RESOURCE_TYPES;
	}

	for (int i = 0; i < LEVELS - 1; i++) {
		destinationSuperAreas[i] = NULL;
	}
	slowness = 2 + rand() % 6;

	consuming = false;
	resetActivity();

	objects[(time + (rand() % slowness)) % BUCKETS].push_back(this);

#	ifdef SAFE
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

Unit::~Unit(){
#	ifdef DEBUG
	if (childs > 0) {
		error("Do not delete a unit if it still has childs");
	}

	debug_unitCount--;
#	endif
	if (parent != NULL) {
		detachFromParent();
	}
}

void Unit::detachFromParent() {
	parent->childs--;
	if (parent->childs == 0 && !parent->inBucket) {
		delete parent;
	}
}

void Unit::addToTileExtra() {
	nearZones[x / NEAR_ZONE_DISTANCE][y / NEAR_ZONE_DISTANCE].addUnit(this);
}

void Unit::removeFromTileExtra() {
	nearZones[x / NEAR_ZONE_DISTANCE][y / NEAR_ZONE_DISTANCE].removeUnit(this);
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

	if (parent != NULL && !parent->alive) {
		detachFromParent();
		parent = NULL;
	}
	
	removeFromTile();

	life--;

	if (life <= 0) {
		alive = false;
		return;
	}

	destinationObject = findNearObjective();

	pursueGoal();

	if (alive) {
		addToTile();
		objects[(time + slowness) % BUCKETS].push_back(this);
	}
}

//Returns the nearest orphan or enemy if there is any near
Unit* Unit::findNearObjective() {
	Unit* nearestOrphan = NULL;
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
				int dist = abs(x - unit->x) + abs(y - unit->y);
				if (dist < nearestDistance) {
					//It checks that it is an orphan or an enemy
					if (unit->isOrphan() || isEnemyOf(unit)) {
						nearestDistance = dist;
						nearestEnemy = unit;
					}
				}
			}
		}
	}
	return nearestEnemy;
}

bool Unit::isEnemyOf(Unit* unit) {
	if (parent == NULL || unit->parent == NULL) {
		return false;
	}
	else {
		return master() != unit->master();
	}
}

bool Unit::isOrphan() {
	return parent == NULL && childs == 0;
}

Unit* Unit::master() {
	if (parent == NULL) {
		return this;
	}
	else {
		return parent->master();
	}
}

void Unit::resetActivity() {
	storingResource = NO_RESOURCE;
	searching1 = NO_RESOURCE;
	searching2 = NO_RESOURCE;
	carrying = NO_RESOURCE;
	consuming = !consuming;
	if (consuming) {
		int r = rand() % RESOURCE_CATEGORIES;
		searching1 = desiredResources[r] + RESOURCE_TYPES * r;
	}
	else {
#		ifdef DEBUG
		if (RESOURCE_CATEGORIES != 3) {
			error("This implementation only works with RESOURCE_CATEGORIES=3");
		}
#		endif
		int r = rand() % RESOURCE_CATEGORIES;
		if (r == 0) {//First category resource
			storingResource = desiredResources[0];
			searching1 = storingResource;
		}
		else if (r == 1) {
			r = rand() % 3;
			if (r == 0) {//Second category resource
				storingResource = desiredResources[1] + RESOURCE_TYPES;
				searching1 = desiredResources[1];
				searching2 = (desiredResources[1] + 1) % RESOURCE_TYPES;
			}
			else if (r == 1) {//First component
				storingResource = desiredResources[1];
				searching1 = storingResource;
			}
			else if (r == 2) {//Second component
				storingResource = (desiredResources[1] + 1) % RESOURCE_TYPES;
				searching1 = storingResource;
			}
		}
		else if(r == 2) {
			r = rand() % 7;
			if (r == 0) {//Third category resource
				storingResource = desiredResources[2] + RESOURCE_TYPES * 2;
				searching1 = desiredResources[2] + RESOURCE_TYPES;
				searching2 = desiredResources[2] + 2 + RESOURCE_TYPES;
			}
			else if (r == 1) {//First component
				storingResource = desiredResources[2] + RESOURCE_TYPES;
				searching1 = desiredResources[2];
				searching2 = desiredResources[2] + 1;
			}
			else if (r == 2) {//Second component
				storingResource = desiredResources[2] + 2 + RESOURCE_TYPES;
				searching1 = desiredResources[2] + 2;
				searching2 = desiredResources[2] + 3;
			}
			else if (r == 3) {//First component first subcomponent
				storingResource = desiredResources[2];
				searching1 = storingResource;
			}
			else if (r == 4) {//First component second subcomponent
				storingResource = desiredResources[2] + 1;
				searching1 = storingResource;
			}
			else if (r == 5) {//Second component first subcomponent
				storingResource = desiredResources[2] + 2;
				searching1 = storingResource;
			}
			else if (r == 6) {//Second component second subcomponent
				storingResource = desiredResources[2] + 3;
				searching1 = storingResource;
			}
		}
	}

	//Pathfinding stuff
	hasToResetPath = true;
	destinationObject = NULL;
}