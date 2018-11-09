#include "Common.h"

void Unit::pursueResource() {








	if (id == DEBUG_OBJECT) {
		Area* asd = areasMap[x][y][z];
		if (asd->resources[11] > 1) {
			printf("");
		}
		printf("%d\n", asd->resources[11]);
	}








	if (!checkReachedResource()) {//Remember that a resource can be created at any moment, so this is the first thing I should check
		int attemptedMovement = -1;
		if (hasToResetPath) {
			hasToResetPath = false;
			attemptedMovement = resetPathTowardsResource();
#			ifdef DEBUG
			checkDestinationAreas();
#			endif
#			ifdef LOG_PF
			printPath();
#			endif
		}
		else {
			if (reachedDestinationBaseArea) {
				reachedDestinationBaseArea = false;
				attemptedMovement = adjustPathTowardsResource();
#				ifdef DEBUG
				checkDestinationAreas();
#				endif
#				ifdef LOG_PF
				if (attemptedMovement != -1) {
					printPath();
				}
#				endif
			}
			else {
				attemptedMovement = dijkstraTowardsResourceOrArea(true);
			}
		}
		if (attemptedMovement == PATH_OUTDATED) {
			//If the path adjustment is outdated, the unit will calculate its path again in the next iteration
			//It is important not to do it in the next iteration because the checked areas and tiles
			//already have their lastCheckedAtTime variables set to the current time
			resetPathPoronga();
			return;
		}
		else if (attemptedMovement == PATH_NOT_FOUND) {
			attemptedMovement = rand() % 4;
			int nextX = x + getX(attemptedMovement);
			int nextY = y + getY(attemptedMovement);
			if (steppableTileHeight(x, y, z, nextX, nextY) == -1) {
				return;
			}
		}
		Area* oldArea = areasMap[x][y][z];
		removeFromTile();
		int nextX = x + getX(attemptedMovement);
		int nextY = y + getY(attemptedMovement);
		z = steppableTileHeight(x, y, z, nextX, nextY);
		x = nextX;
		y = nextY;
		addToTile();
		Area* newArea = areasMap[x][y][z];

		if (oldArea != newArea) {
			oldArea->decreaseResource(resourceType);
			newArea->increaseResource(resourceType);
			if (resourceSearchStatus != -1) {
				if (resourceSearchStatus < RESOURCE_TYPES) {
					oldArea->decreaseResource(RESOURCE_TYPES + resourceSearchStatus);
					newArea->increaseResource(RESOURCE_TYPES + resourceSearchStatus);
				}
				else {
					oldArea->decreaseResource(resourceSearchStatus - RESOURCE_TYPES);
					newArea->increaseResource(resourceSearchStatus - RESOURCE_TYPES);
				}
			}
		}

		checkReachedResource();		

		if (areasMap[x][y][z] == baseDestinationArea) {
			reachedDestinationBaseArea = true;
		}
	}
}

bool Unit::checkReachedResource() {
	if (cycle[cycleCurrentStep] < RESOURCE_TYPES) {
		return checkReachedResourceSearch();
	}
	else if (cycle[cycleCurrentStep] == INSTRUCTION_GIVE_RESOURCE) {
		return checkReachedResourceGive();
	}
	else {
		error("Wrong cycle instruction");
	}
}

bool Unit::checkReachedResourceGive() {
	if (bag.empty()) {
		nextStep(true);
		return true;
	}
	Object* current = unitsMap[x][y][z];
	while (current != NULL) {
		bool pickUpUnit = false;
		if (current->objectType == objectUnit) {
			Unit* currentUnit = (Unit*)current;
			if (currentUnit->cycle[currentUnit->cycleCurrentStep] == resourceSearchStatus - RESOURCE_TYPES) {
				giveResource(currentUnit);
				return true;
			}
		}
		current = current->sharesTileWithObject;
	}
	return false;
}

bool Unit::checkReachedResourceSearch() {
#	ifdef DEBUG
	if (resourceSearchStatus == -1) {
		error("lookingForResource should not be -1");
	}
	if (resourceSearchStatus != cycle[cycleCurrentStep]) {
		error("lookingForResource should be equal to the the current step");
	}
#	endif
	Object* current = unitsMap[x][y][z];
	while (current != NULL) {
		bool pickUpUnit = false;
		if (current->objectType == objectUnit) {
			Unit* currentUnit = (Unit*)current;
			if (currentUnit->cycle[currentUnit->cycleCurrentStep] == INSTRUCTION_GIVE_RESOURCE) {
				if (currentUnit->resourceSearchStatus - RESOURCE_TYPES == resourceSearchStatus) {
					currentUnit->giveResource(this);
					return true;
				}
			}
			if (currentUnit->resourceType == resourceSearchStatus) {
				if (life >= currentUnit->life) {
					life -= currentUnit->life;
					pickUpUnit = true;
				}
				else {
					areasMap[x][y][z]->decreaseResource(resourceType);
					removeFromTile();
					alive = false;
					currentUnit->life -= life;
				}
			}
		}
		else {
			if (current->resourceType == resourceSearchStatus) {
				pickUpUnit = true;
			}
		}
		if (pickUpUnit) {
			current->removeFromTile();
			current->alive = false;
			aquireResource();
			return true;
		}
		if (!alive) {
			return true;
		}
		current = current->sharesTileWithObject;
	}
	return false;
}

void Unit::aquireResource() {





	if (id == DEBUG_OBJECT) {
		printf("");
	}
	Area* asd = areasMap[x][y][z];






	//There is 1 resource less available
	areasMap[x][y][z]->decreaseResource(resourceSearchStatus);

	//There is 1 resource less being looked for
	areasMap[x][y][z]->decreaseResource(RESOURCE_TYPES + resourceSearchStatus);

	life += calculateWorth();
	if (bag.size() == MAX_CYCLE_LENGTH) {
		bag.erase(bag.begin());
	}
	bag.push_back(resourceSearchStatus);
	nextStep(true);
}

void Unit::resetPathPoronga() {
#	ifdef DEBUG
	if (cycle[cycleCurrentStep] == resourceType) {
		error("A unit can not be the kind of resource it looks for");
	}
#	endif
	hasToResetPath = true;
	lowestDestinationAreaReached = NULL;
	reachedDestinationBaseArea = false;
	baseDestinationArea = NULL;
}

void Unit::nextStep(bool moveToTheNextStep) {
	if (moveToTheNextStep) {
		cycleCurrentStep = (cycleCurrentStep + 1) % cycleLength;
	}
	resetPathPoronga();
	resourceSearchStatus = -1;
	if (cycle[cycleCurrentStep] < RESOURCE_TYPES) {
		resourceSearchStatus = cycle[cycleCurrentStep];

		//There is 1 resource more being looked for
		areasMap[x][y][z]->increaseResource(RESOURCE_TYPES + resourceSearchStatus);
	}
	else if (cycle[cycleCurrentStep] == INSTRUCTION_GIVE_RESOURCE) {
		if (!bag.empty()) {
			char lastResourceInBag = bag[bag.size() - 1];
			resourceSearchStatus = RESOURCE_TYPES + lastResourceInBag;

			//There is 1 resource more available
			areasMap[x][y][z]->increaseResource(lastResourceInBag);
		}
	}
}

void Unit::giveResource(Unit* taker) {
	taker->aquireResource();
	bag.erase(bag.end() - 1);
	nextStep(true);
}

#ifdef DEBUG
void Unit::checkDestinationAreas() {
	for (int i = 0; i < LEVELS - 1; i++) {
		if (destinationSuperAreas[i] != NULL && destinationSuperAreas[i]->lvl != i + 1) {
			error("Wrong destination areas level");
		}
	}
}
#endif

#ifdef LOG_PF
void Unit::printPath() {
	if (unitId == DEBUG_UNIT) {
		printf("unit location: %d %d\n", x, y);
		if (lowestDestinationAreaReached->lvl > 0) {
			Chunk* chunk = baseDestinationArea->chunk;
			printf("dest area   lvl: %d   at: %d %d   dist in tiles: %d\n", chunk->lvl, chunk->x, chunk->y, tilesToBeTraveled);
			for (int i = 0; i < lowestDestinationAreaReached->lvl - 1; i++) {
				chunk = destinationSuperAreas[i]->chunk;
				printf("dest area   lvl: %d   at: %d %d   dist in c%d: %d\n", chunk->lvl, chunk->x, chunk->y, i, chunksToBeTraveled[i]);
			}
			printf("reached   lvl: %d   at: %d %d\n", lowestDestinationAreaReached->lvl, lowestDestinationAreaReached->chunk->x, lowestDestinationAreaReached->chunk->y);
		}
		printf("\n");
	}
}
#endif