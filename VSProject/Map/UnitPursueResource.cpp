#include "Common.h"

void Unit::pursueResource() {
	//Remember that a resource can be created at any moment, so this is the first thing I should check
	if (!checkReachedResource()) {
		int attemptedMovement = -1;
		if (hasToResetPath) {
			hasToResetPath = false;
			if (destinationObject != NULL) {
				attemptedMovement = resetPathTowardsObject();
			}
			else {
				attemptedMovement = resetPathTowardsResource();
			}
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
				if (destinationObject != NULL) {
					attemptedMovement = adjustPathTowardsObject();
				}
				else {
					attemptedMovement = adjustPathTowardsResource();
				}
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
				if (destinationObject != NULL) {
					attemptedMovement = dijkstraTowardsObjectOrArea(true);
				}
				else {
					attemptedMovement = dijkstraTowardsResourceOrArea(true);
				}
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
	else {
		if (cycle[cycleCurrentStep]  % 2 != INSTRUCTION_GIVE_RESOURCE) {
			error("Wrong cycle instruction");
		}
		return checkReachedResourceGive();
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
			if (currentUnit->resourceSearchStatus - RESOURCE_TYPES == resourceSearchStatus) {
				currentUnit->removeFromTile();
				currentUnit->giveResource(this);
				currentUnit->addToTile();


				if (currentUnit->parent == this) {
					currentUnit->life += 200;
					life += 300;




					printf("%d\n", id);
					selected = currentUnit;



				}


				return true;
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
	if (bag.size() == MAX_CYCLE_LENGTH) {
		bag.erase(bag.begin());
	}
	bag.push_back(resourceSearchStatus);
	nextStep();
}

void Unit::nextStep() {



	if (slowness < 6 && rand() % 5 == 0) {
		slowness++;
	}
	else if (slowness > 2 && rand() % 5 == 0) {
		slowness--;
	}



	if (resourceSearchStatus != -1) {
		//The unit won't gain life if it executed:
		// - A new instruction instruction
		// - A give instruction when it didn't have anything to give
		life += calculateWorth();
	}
	cycleCurrentStep = (cycleCurrentStep + 1) % cycleLength;
	if (cycleCurrentStep == 0 && bag.size() > 0) {
		createUnit();
	}
	initializeStep();
}

void Unit::initializeStep() {
	hasToResetPath = true;
	resourceSearchStatus = -1;
	destinationObject = NULL;
	if (cycle[cycleCurrentStep] < RESOURCE_TYPES) {
		resourceSearchStatus = cycle[cycleCurrentStep];
	}
	else if (cycle[cycleCurrentStep] % 2 == INSTRUCTION_GIVE_RESOURCE) {
		if (!bag.empty()) {
			char lastResourceInBag = bag[bag.size() - 1];
			if (lastResourceInBag < RESOURCE_TYPES) {



				//char shiftedResource = (lastResourceInBag + 1) % RESOURCE_TYPES;
				char shiftedResource = lastResourceInBag;
				
				
				
				
				
				bag[bag.size() - 1] = shiftedResource;
				resourceSearchStatus = RESOURCE_TYPES + shiftedResource;
			}



			if (slowness > 2) {
				slowness--;
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