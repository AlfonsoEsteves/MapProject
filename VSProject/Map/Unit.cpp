#include "Common.h"

#ifdef DEBUG
int debug_unitCount = 0;
#endif

Unit::Unit(int _x, int _y, int _z, int _life) : Object(objectUnit, _x, _y, _z)
{
	life = _life;

	for (int i = 0; i < LEVELS - 1; i++) {
		destinationSuperAreas[i] = NULL;
	}
	baseDestinationArea = NULL;
	hasToResetPath = true;
	slowness = 2 + rand() % 4;

	cycleCurrentStep = 0;

	resourceType = -1;//This default value, avoid decreasing areas' resources when adjustResourceType is called

	objects[(time + (rand() % slowness)) % BUCKETS].push_back(this);

#	ifdef DEBUG
	debug_unitCount++;

	if (!tileIsSteppable(x, y, z)) {
		error("A unit can only occupy steppable tiles");
	}
#	endif
}

#ifdef DEBUG
Unit::~Unit(){
	debug_unitCount--;
}
#endif

unsigned char Unit::type() {
	return objectUnit;
}

void Unit::execute() {
#	ifdef DEBUG
	if (!tileIsSteppable(x, y, z)) {
		error("A unit can only occupy steppable tiles");
	}
	if (cycleLength==0) {
		error("A unit can not have an empty cycle");
	}
#	endif
	life--;
	if (life == 0) {
		modifyCycle(CHANCES_OF_ADDING_A_STEP);
		if (cycleLength == 0) {
			removeFromTile();
			areasMap[x][y][z]->decreaseResource(resourceType);
			alive = false;
			return;
		}
		adjustResourceType();
	}
	if (cycle[cycleCurrentStep] < RESOURCE_TYPES) {
		pursueResource();
	}
	else if (cycle[cycleCurrentStep] == OPEN_BRACKET) {
		createUnit();
	}
#	ifdef DEBUG
	else {
 		error("Units can only execute resource searches or open brackets");
	}
#	endif
	if (alive) {
		objects[(time + slowness) % BUCKETS].push_back(this);
	}
}

void Unit::createUnit() {
	char openness = 1;
	char oldIndex = cycleCurrentStep + 1;
	int newLife = (life * 9) / 10;
	if (newLife > 0) {
		Unit * unit = new Unit(x, y, z, newLife);
		unit->cycleLength = 0;
		char newIndex = 0;
		while (true) {
#			ifdef DEBUG
			if (oldIndex >= cycleLength) {
				error("All blocks should close before the end of the cycle");
			}
#			endif
			if (cycle[oldIndex] == OPEN_BRACKET) {
				openness++;
			}
			if (cycle[oldIndex] == CLOSE_BRACKET) {
				openness--;
				if (openness == 0) {
					break;
				}
			}
			unit->cycle[newIndex] = cycle[oldIndex];
			unit->cycleLength++;
			oldIndex++;
			newIndex++;
		}
		unit->adjustResourceType();
#		ifdef DEBUG
		checkUnitSteps(unit);
#		endif
	}
	else {
		while (true) {
#			ifdef DEBUG
			if (oldIndex == cycleLength) {
				error("All blocks should close before the end of the cycle");
			}
#			endif
			if (cycle[oldIndex] == OPEN_BRACKET) {
				openness++;
			}
			if (cycle[oldIndex] == CLOSE_BRACKET) {
				openness--;
				if (openness == 0) {
					break;
				}
			}
			oldIndex++;
		}
	}
	cycleCurrentStep = (oldIndex + 1) % cycleLength;
	checkIfPathfindingResetIsNeeded();
}

void Unit::modifyCycle(int chancesOfAddingStep) {
	if (rand() % chancesOfAddingStep == 0 && cycleLength < MAX_CYCLE_SIZE) {//Add a step or a block
		int x = rand() % (cycleLength + 1);
		if (rand() % CHANCES_OF_ADDING_A_BLOCK_STEP == 0 && cycleLength < MAX_CYCLE_SIZE - 2) {
			for (int i = cycleLength + 2; i > x + 2; i--) {
				cycle[i] = cycle[i - 3];
			}
			cycle[x] = OPEN_BRACKET;
			cycle[x + 1] = (lookingForResource + rand() % (RESOURCE_TYPES - 1) + 1) % RESOURCE_TYPES;
			cycle[x + 2] = CLOSE_BRACKET;//It closes with a curly bracket
			cycleLength += 3;
			if (x <= cycleCurrentStep) {
				cycleCurrentStep += 3;
			}
		}
		else {
			for (int i = cycleLength; i > x; i--) {
				cycle[i] = cycle[i - 1];
			}
			cycle[x] = (lookingForResource + rand() % (RESOURCE_TYPES - 1) + 1) % RESOURCE_TYPES;
			cycleLength++;
			if (x <= cycleCurrentStep) {
				cycleCurrentStep++;
			}
		}
		life = LIFE;
	}
	else {//Remove a step or a block
		char x;
		char y;
		do {
			x = rand() % cycleLength;
		} while (cycle[x] >= RESOURCE_TYPES);
		y = x;
		//I check if the step (or block) is the only step of a block. If that is the case, then I should remove the containing block too
		while (x > 0 && y < cycleLength - 1 && cycle[x - 1] == OPEN_BRACKET && cycle[y + 1] == CLOSE_BRACKET) {
			x--;
			y++;
		}
		int stepsRemoved = y - x + 1;
		cycleLength -= stepsRemoved;
		if (cycleLength > 0) {
			for (int i = x; i < cycleLength; i++) {
				cycle[i] = cycle[i + stepsRemoved];
			}
			if (cycleCurrentStep >= x){
				if (cycleCurrentStep <= y) {
					cycleCurrentStep = x % cycleLength;
				}
				else {
					cycleCurrentStep -= stepsRemoved;
				}

			}
			life = LIFE;
		}
	}
#	ifdef DEBUG
	checkUnitSteps(this);
#	endif
}

void Unit::adjustResourceType() {
	bool availableResource[RESOURCE_TYPES];
	int availableResources = RESOURCE_TYPES;
	for (int i = 0; i < 6; i++) {
		availableResource[i] = true;
	}
	int hash = 0;
	int openness = 0;
	for (int i = 0; i < cycleLength; i++) {
		hash += cycle[i];
		if (cycle[i] == OPEN_BRACKET) {
			openness++;
		}
		else if (cycle[i] == CLOSE_BRACKET) {
			openness--;
		}
		else if (openness == 0) {
			if (availableResource[cycle[i]]) {
				availableResource[cycle[i]] = false;
				availableResources--;
			}
		}
	}
	hash = hash % availableResources;
	char newResourceType = 0;
	while (true) {
		if (availableResource[newResourceType]) {
			if (hash == 0) {
				break;
			}
			else {
				hash--;
			}
		}
		newResourceType++;
	}
	if (newResourceType != resourceType) {
		if (resourceType != -1) {
			areasMap[x][y][z]->decreaseResource(resourceType);
		}
		areasMap[x][y][z]->increaseResource(newResourceType);
		resourceType = newResourceType;
	}
	checkIfPathfindingResetIsNeeded();
}

#ifdef DEBUG
void Unit::checkUnitSteps(Unit* unit) {
	int openness = 0;
	for (int i = 0; i < unit->cycleLength; i++) {
		if (unit->cycle[i] == OPEN_BRACKET) {
			openness++;
			if (unit->cycle[i + 1] == CLOSE_BRACKET) {
				error("A steps block can not be empty");
			}
		}
		if (unit->cycle[i] == CLOSE_BRACKET) {
			openness--;
			if (openness < 0) {
				error("It can not close more brackets than it opens");
			}
		}
		if (unit->cycleCurrentStep == i){
			if (openness > 1) {
				error("The unit can not execute a block inner content");
			}
			if (openness == 1 && unit->cycle[i] != OPEN_BRACKET) {
				error("The unit can not execute a block inner content");
			}
		}
		if (unit->cycleCurrentStep > unit->cycleLength) {
			error("There is no such step");
		}
	}
	if (openness != 0) {
		error("All brackets should be closed");
	}
}
#endif

bool Unit::hasBrackets() {
	for (int i = 0; i < cycleLength; i++) {
		if (cycle[i] == OPEN_BRACKET) {
			return true;
		}
	}
	return false;
}