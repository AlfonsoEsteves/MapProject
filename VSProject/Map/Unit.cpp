#include "Common.h"

#ifdef DEBUG
int debug_unitCount = 0;
#endif

Unit::Unit(int _x, int _y, int _z, int _life, Unit* _parent, int _resourceType) : Object(objectUnit, _x, _y, _z)
{
	life = _life;
	parent = _parent;
	if (parent == NULL) {
		resourceType = _resourceType;
	}
	childs = 0;
	inBucket = true;
	destinationObject = NULL;


	for (int i = 0; i < LEVELS - 1; i++) {
		destinationSuperAreas[i] = NULL;
	}
	slowness = 2 + rand() % 6;

	objects[(time + (rand() % slowness)) % BUCKETS].push_back(this);

	if (parent == NULL) {
		for (int i = 0; i < RESOURCE_TYPES; i++) {
			if (rand() % 4 == 0) {
				hate[i] = false;
			}
			else {
				hate[i] = true;
			}
		}
		hate[resourceType] = false;
	}
	else {
		for (int i = 0; i < RESOURCE_TYPES; i++) {
			hate[i] = parent->hate[i];
		}
		resourceType = rand() % RESOURCE_TYPES;
		while (hate[resourceType]) {
			resourceType = (resourceType + 1) % RESOURCE_TYPES;
		}
	}

#	ifdef DEBUG
	cycleCurrentStep = 0;
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
	cycleCurrentStep = 0;
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
	if (cycleLength==0) {
		error("A unit can not have an empty cycle");
	}
	if (life < 0) {
		error("A unit can not have less than 0 life");
	}
#	endif
	
	removeFromTile();

	life--;
	if (life == 0) {
		if (rand() % 2 == 0 && cycleLength < MAX_CYCLE_LENGTH) {
			life = LIFE;
			addRandomStepToCycle();
		}
		else {
			alive = false;
			return;
		}
	}
	if (cycle[cycleCurrentStep] < RESOURCE_TYPES) {
		pursueResource();
	}
	else if (cycle[cycleCurrentStep] % 2 == INSTRUCTION_GIVE_RESOURCE) {
		if (resourceSearchStatus != -1) {
			destinationObject = NULL;
			if (parent != NULL && parent->alive) {
				if (parent->resourceSearchStatus == resourceSearchStatus - RESOURCE_TYPES) {
					destinationObject = parent;
				}
			}
			pursueResource();
		}
		else {
			//The unit didn't have a last resource
			nextStep();
		}
	}
	else if (cycle[cycleCurrentStep] % 2 == INSTRUCTION_NEW_INSTRUCTION) {
		newInstruction();
	}
#	ifdef DEBUG
	else {
 		error("Wrong instruction");
	}
#	endif

	if (alive) {
		addToTile();
		objects[(time + slowness) % BUCKETS].push_back(this);
	}
}

void Unit::createUnit() {
	if (bag.size() > 0) {
		int alpha = 6;
		int newLife = sqrt(life * alpha * alpha) + 1 - alpha;
		if (newLife > 0) {
			childs++;
			Unit * unit = new Unit(x, y, z, newLife, this, -1);
			for (int i = 0; i < bag.size(); i++) {
				unit->cycle[i] = bag[i];
			}
			unit->cycleLength = (char)bag.size();
			unit->initializeUnit();
			bag.clear();
		}
	}
}

void Unit::addRandomStepToCycle() {
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
/*
void Unit::adjustResourceType() {
	if (cycleLength == 1) {
		resourceType = (cycle[0] + 1) % RESOURCE_TYPES;
	}
	else{
		bool availableResource[RESOURCE_TYPES];
		int availableResources = RESOURCE_TYPES;
		for (int i = 0; i < RESOURCE_TYPES; i++) {
			availableResource[i] = true;
		}
		int hash = 0;
		for (int i = 0; i < cycleLength; i++) {
			hash += cycle[i];
			if (cycle[i] < RESOURCE_TYPES) {
				if (availableResource[cycle[i]]) {
					availableResource[cycle[i]] = false;
					availableResources--;
				}
			}
		}
		hash = hash % availableResources;
		resourceType = 0;
		while (true) {
			if (availableResource[resourceType]) {
				if (hash == 0) {
					break;
				}
				else {
					hash--;
				}
			}
			resourceType++;
		}
	}
	hasToResetPath = true;
}*/

#define INITIAL_WORTH 80
#define RESOURCE_WORTH 30
#define INSTRUCTION_WORTH 60
#define LINEAR_FACTOR 30
#define WORTH_DIVISOR 400

int Unit::calculateWorth() {
	int worth = INITIAL_WORTH;
	bool instructions[INSTRUCTIONS];
	for (int i = 0; i < INSTRUCTIONS; i++) {
		instructions[i] = false;
	}
	for (int i = 0; i < cycleLength; i++) {
		if (!instructions[cycle[i]]) {
			instructions[cycle[i]] = true;
			if (cycle[i] < RESOURCE_TYPES) {
				worth += RESOURCE_WORTH;
			}
			else {
				worth += INSTRUCTION_WORTH;
			}
		}
	}
	worth = (worth * worth + worth * LINEAR_FACTOR) / WORTH_DIVISOR;
	return worth;
}

void Unit::newInstruction() {
	if (bag.size() > 0) {
		int resource = bag[bag.size() - 1];
		bag[bag.size() - 1] = resource % RESOURCE_TYPES + RESOURCE_TYPES;
	}
	nextStep();
}

bool Unit::providesResource(char _resourceType) {
	if (_resourceType == resourceType) {
		return true;
	}
	else{
		if (resourceSearchStatus != -1) {
			if (resourceSearchStatus < RESOURCE_TYPES) {
				if (RESOURCE_TYPES + resourceSearchStatus == _resourceType) {
					return true;
				}
			}
			else {
				if (resourceSearchStatus - RESOURCE_TYPES == _resourceType) {
					return true;
				}
			}
		}
	}
	return false;
}