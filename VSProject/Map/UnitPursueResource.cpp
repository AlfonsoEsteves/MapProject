#include "Common.h"

void Unit::pursueResource() {
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
			checkIfPathfindingResetIsNeeded();
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

		if (oldArea != areasMap[x][y][z]) {
			oldArea->decreaseResource(resourceType);
			areasMap[x][y][z]->increaseResource(resourceType);
		}

		checkReachedResource();		

		if (areasMap[x][y][z] == baseDestinationArea) {
			reachedDestinationBaseArea = true;
		}
	}
}

bool Unit::checkReachedResource() {
	if (cycle[cycleCurrentStep] < RESOURCE_TYPES) {
		return checkReachedResourceSearch()
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
		cycleCurrentStep = (cycleCurrentStep + 1) % cycleLength;agregar esta linea al metodo que le sigue
		checkIfPathfindingResetIsNeeded();
		return true;
	}
	Object* current = unitsMap[x][y][z];
	while (current != NULL) {
		bool pickUpUnit = false;
		if (current->objectType == objectUnit) {
			Unit* currentUnit = (Unit*)current;
			if (currentUnit->cycle[currentUnit->cycleCurrentStep] == lookingForResource - RESOURCE_TYPES) {
				giveResource(currentUnit);
				return true;
			}
		}
	}
	return false;
}

bool Unit::checkReachedResourceSearch() {
#	ifdef DEBUG
	if (lookingForResource == -1) {
		error("lookingForResource should not be -1");
	}
	if (lookingForResource != cycle[cycleCurrentStep]) {
		error("lookingForResource should be equal to the the current step");
	}
#	endif
	Object* current = unitsMap[x][y][z];
	while (current != NULL) {
		bool pickUpUnit = false;
		if (current->objectType == objectUnit) {
			Unit* currentUnit = (Unit*)current;
			if (currentUnit->cycle[currentUnit->cycleCurrentStep] == INSTRUCTION_GIVE_RESOURCE) {
				if (currentUnit->lookingForResource - RESOURCE_TYPES == lookingForResource) {
					currentUnit->giveResource(this);
					return true;
				}
			}
			if (currentUnit->resourceType == lookingForResource) {
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
			if (current->resourceType == lookingForResource) {
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
	//There is 1 resource less available
	areasMap[x][y][z]->decreaseResource(lookingForResource);

	//There is 2 resource less being looked for
	areasMap[x][y][z]->decreaseResource(RESOURCE_TYPES + lookingForResource);

	life += calculateWorth();
	if (bag.size() == MAX_CYCLE_LENGTH) {
		bag.erase(bag.begin());
	}
	bag.push_back(lookingForResource);
	cycleCurrentStep = (cycleCurrentStep + 1) % cycleLength;
	checkIfPathfindingResetIsNeeded();
}

void Unit::checkIfPathfindingResetIsNeeded() {
#	ifdef DEBUG
	if (cycle[cycleCurrentStep] == resourceType) {
		error("A unit can not be the kind of resource it looks for");
	}
#	endif
	if (cycle[cycleCurrentStep] < RESOURCE_TYPES) {
		lookingForResource = cycle[cycleCurrentStep];
		hasToResetPath = true;
		lowestDestinationAreaReached = NULL;
		reachedDestinationBaseArea = false;
		baseDestinationArea = NULL;

		//There is 1 resource more being looked for
		areasMap[x][y][z]->increaseResource(RESOURCE_TYPES + lookingForResource);

#		ifdef LOG_PF
		if (unitId == DEBUG_UNIT) {
			printf("Started looking for %d\n\n", lookingForResource);
		}
#		endif
	}
	else if (cycle[cycleCurrentStep] == INSTRUCTION_GIVE_RESOURCE) {
		if (!bag.empty()) {
			//There is 1 resource more available
			areasMap[x][y][z]->increaseResource(lookingForResource);

			lookingForResource = RESOURCE_TYPES + bag[bag.size() - 1];
		}
	}
	else {
		lookingForResource = -1;
	}
}

void Unit::giveResource(Unit* taker) {
	taker->aquireResource();
	bag.erase(bag.end() - 1);
	cycleCurrentStep = (cycleCurrentStep + 1) % cycleLength;
	checkIfPathfindingResetIsNeeded();
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