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

Unit::~Unit(){
#	ifdef DEBUG
	debug_unitCount--;
#	endif
}

void Unit::addToTileExtra() {
	Area* area = areasMap[x][y][z];
	area->increaseResource(resourceType);
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
	area->decreaseResource(resourceType);
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
#	endif

	removeFromTile();

	life--;
	if (life == 0) {
		modifyCycle(CHANCES_OF_ADDING_A_STEP);
		if (alive) {
			adjustResourceType();
		}
		else {
			return;
		}
	}
	if (cycle[cycleCurrentStep] < RESOURCE_TYPES) {
		pursueResource();
	}
	else if (cycle[cycleCurrentStep] == INSTRUCTION_GIVE_RESOURCE) {
		if (resourceSearchStatus != -1) {
			pursueResource();
		}
		else {
			//The unit didn't have a last resource, or it was not givable
			nextStep();
		}
	}
	else if (cycle[cycleCurrentStep] == INSTRUCTION_DUPLICATE) {
		createUnit();
	}
	else if (cycle[cycleCurrentStep] == INSTRUCTION_NEW_INSTRUCTION) {
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


		//TEMPORARY
		life += LIFE;




		int newLife = (life * 9) / 10;
		if (newLife > 0) {
			Unit * unit = new Unit(x, y, z, newLife);
			for (int i = 0; i < bag.size(); i++) {
				unit->cycle[i] = bag[i];
			}
			unit->cycleLength = (char)bag.size();
			unit->adjustResourceType();
			unit->addToTile();
			bag.clear();
		}
	}
	nextStep();
}

void Unit::modifyCycle(int chancesOfAddingStep) {
	if (rand() % chancesOfAddingStep == 0 && cycleLength < MAX_CYCLE_LENGTH) {//Add a step or a block
		life = LIFE;
		int x = rand() % (cycleLength + 1);
		for (int i = cycleLength; i > x; i--) {
			cycle[i] = cycle[i - 1];
		}
		cycle[x] = (resourceSearchStatus + rand() % (INSTRUCTIONS - 1) + 1) % INSTRUCTIONS;
		cycleLength++;
		if (x <= cycleCurrentStep) {
			cycleCurrentStep++;
		}
	}
	else {//Remove a step or a block
		cycleLength--;
		if (cycleLength > 0) {
			life = LIFE;
			int x = rand() % (cycleLength + 1);
			for (int i = x; i < cycleLength; i++) {
				cycle[i] = cycle[i + 1];
			}
			if (x < cycleCurrentStep) {
				cycleCurrentStep--;
			}
			else {
				cycleCurrentStep = cycleCurrentStep % cycleLength;
			}
		}
		else {
			alive = false;
		}
	}
}

void Unit::adjustResourceType() {
	bool availableResource[RESOURCE_TYPES];
	int availableResources = RESOURCE_TYPES;
	for (int i = 0; i < 6; i++) {
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
	initializeStep();
}

int Unit::calculateWorth() {
	int w = (LIFE * cycleLength) / 6;
	if (hasDuplicate()) {
		w += (LIFE * cycleLength * cycleLength) / 4;
	}
	return w;
}

bool Unit::hasDuplicate() {
	for (int i = 0; i < cycleLength; i++) {
		if (cycle[i] == INSTRUCTION_DUPLICATE) {
			return true;
		}
	}
	return false;
}

void Unit::newInstruction() {
	if (bag.size() > 0) {


		//TEMPORARY
		life += LIFE;



		int resource = bag[bag.size() - 1];
		if (resource % 2 == 0) {
			bag[bag.size() - 1] = INSTRUCTION_DUPLICATE;
		}
		else {
			bag[bag.size() - 1] = INSTRUCTION_NEW_INSTRUCTION;
		}
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