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
	slowness = 2 + rand() % 4;

	objects[(time + (rand() % slowness)) % BUCKETS].push_back(this);

	//This is needed because the method addStepToCycle needs the resourceType preset
	resourceType = rand() % RESOURCE_TYPES;

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
	adjustResourceType();
	initializeStep();
	addToTile();
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
/*
void popopopo(int x, int y, int z, int popo) {
	Resource* resource = new Resource(x, y, z, popo);
	resource->addToTile();
}*/

void Unit::execute() {

	/*unsigned char uc = (unsigned char)this;
	if ((time / slowness + uc) % (80 + uc % 81) == 0) {
		popopopo(x, y, z, popo);
	}*/



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
			adjustResourceType();
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
			pursueResource();
		}
		else {
			//The unit didn't have a last resource, or it was not givable
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
			Unit * unit = new Unit(x, y, z, newLife);
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
}

#define RESOURCE_WORTH 30
#define INSTRUCTION_WORTH 80
#define WORTH_DIVISOR 65

int Unit::calculateWorth() {
	int worth = 0;
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
	return (worth * worth + worth * 3) / WORTH_DIVISOR;
}

void Unit::newInstruction() {
	if (bag.size() > 0) {


		//TEMPORARY
		life += LIFE;



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