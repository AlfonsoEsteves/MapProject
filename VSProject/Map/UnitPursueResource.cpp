#include "Common.h"

void Unit::pursueResource() {
	//Remember that a resource can be created at any moment, so this is the first thing I should check
	if (!checkReachedResource()) {
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
			hasToResetPath = true;
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

		int nextX = x + getX(attemptedMovement);
		int nextY = y + getY(attemptedMovement);
		z = steppableTileHeight(x, y, z, nextX, nextY);
		x = nextX;
		y = nextY;

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
		nextStep();
		return true;
	}
	Object* current = unitsMap[x][y][z];
	while (current != NULL) {
		if (current->objectType == objectUnit) {
			Unit* currentUnit = (Unit*)current;
			if (currentUnit->resourceSearchStatus == resourceSearchStatus - RESOURCE_TYPES) {
				currentUnit->removeFromTile();
				giveResource(currentUnit);
				currentUnit->addToTile();
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
					currentUnit->removeFromTile();
					currentUnit->giveResource(this);
					currentUnit->addToTile();
					return true;
				}
			}
			if (currentUnit->resourceType == resourceSearchStatus) {
				if (life >= currentUnit->life) {
					life -= currentUnit->life;
					pickUpUnit = true;
				}
				else {
					alive = false;
					currentUnit->life -= life;
				}
			}
		}
		else if (current->type() == objectResource) {
			Resource* currentResource = (Resource*)current;
			if (currentResource->resourceType == resourceSearchStatus) {
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
	life += calculateWorth();
	if (bag.size() == MAX_CYCLE_LENGTH) {
		bag.erase(bag.begin());
	}
	bag.push_back(resourceSearchStatus);
	nextStep();
}

void Unit::nextStep() {
	cycleCurrentStep = (cycleCurrentStep + 1) % cycleLength;
	initializeStep();
}

void Unit::initializeStep() {
	hasToResetPath = true;
	resourceSearchStatus = -1;
	if (cycle[cycleCurrentStep] < RESOURCE_TYPES) {
		resourceSearchStatus = cycle[cycleCurrentStep];
	}
	else if (cycle[cycleCurrentStep] == INSTRUCTION_GIVE_RESOURCE) {
		if (!bag.empty()) {
			char lastResourceInBag = bag[bag.size() - 1];
			if (lastResourceInBag < RESOURCE_TYPES) {
				resourceSearchStatus = RESOURCE_TYPES + lastResourceInBag;
			}
		}
	}
}

void Unit::giveResource(Unit* taker) {
	bag.erase(bag.end() - 1);
	nextStep();
	taker->aquireResource();
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