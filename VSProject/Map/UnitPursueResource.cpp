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

//This method checks if the unit reached the resource it was looking for and also picks it up if it did
bool Unit::checkReachedResource() {
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
		if (current->resourceType == lookingForResource) {
#			ifdef LOG_PF
			if (unitId == DEBUG_UNIT) {
				printf("Unit reached resource\n\n");
			}
#			endif
			if (current->objectType == objectUnit) {
				Unit* currentUnit = (Unit*)current;
				if (life > currentUnit->life) {
					life -= currentUnit->life;
				}
				else {
					currentUnit->alive = false;
				}
				if(currentUnit->life > life) {
					currentUnit->life -= life;
				}
				else {
					alive = false;
				}
			}
			else {
				current->alive = false;
			}
			if (alive) {
				life += LIFE * cycleLength / 2;
				if (hasBrackets()) {
					life += LIFE * cycleLength / 2;
				}
				cycleCurrentStep = (cycleCurrentStep + 1) % cycleLength;
				checkIfPathfindingResetIsNeeded();
			}
			else{
				areasMap[x][y][z]->decreaseResource(resourceType);
				removeFromTile();
			}
			if (!current->alive) {
				areasMap[x][y][z]->decreaseResource(current->resourceType);
				current->removeFromTile();
				current->alive = false;
			}
			return true;
		}
		current = current->sharesTileWithObject;
	}
	return false;
}

void Unit::checkIfPathfindingResetIsNeeded() {
#	ifdef DEBUG
	if (cycle[cycleCurrentStep] == resourceType) {
		error("A unit can be the kind of resource it looks for");
	}
#	endif
	if (cycle[cycleCurrentStep] < RESOURCE_TYPES) {
		lookingForResource = cycle[cycleCurrentStep];
		hasToResetPath = true;
		lowestDestinationAreaReached = NULL;
		reachedDestinationBaseArea = false;
		baseDestinationArea = NULL;
#		ifdef LOG_PF
		if (unitId == DEBUG_UNIT) {
			printf("Started looking for %d\n\n", lookingForResource);
		}
#		endif
	}
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