#include "Common.h"

void Unit::pursueGoal() {
	//Remember that a resource can be created at any moment, so this is the first thing I should check
	if (!checkReachedGoal()) {
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

		if (!checkReachedGoal()) {
			searchTime++;
			if (searchTime == SEARCH_TIME_OUT) {
				resetActivity(true);
			}
		}

		if (areasMap[x][y][z] == baseDestinationArea) {
			reachedDestinationBaseArea = true;
		}
	}
}

bool Unit::checkReachedGoal() {
	if (destinationObject != NULL) {
		if (destinationObject->x == x && destinationObject->y == y && destinationObject->z == z) {
			//It adopts the orphan
			if (destinationObject == parent) {
				int transference = (2 * life - parent->life ) / 3;
				life -= transference;
				parent->life += transference;
			}
			else if (destinationObject->isOrphan()) {
				destinationObject->parent = this;
				childs++;
			}
			else {//It attacks the enemy
				int auxLife = life;
				if (destinationObject->life <= life) {
					destinationObject->alive = false;
					destinationObject->removeFromTile();
					life -= destinationObject->life;
				}
				if (auxLife <= destinationObject->life) {
					alive = false;
					destinationObject->life -= auxLife;
				}
			}

			/*
			//Reset pathfinding
			hasToResetPath = true;
			destinationObject = NULL;*/

			return true;
		}
	}

	//Checks if it reached a resource
	return checkReachedResourceSearch();

}

bool Unit::checkReachedResourceSearch() {
	Object* current = unitsMap[x][y][z];
	while (current != NULL) {
		if (current->type() == objectResource) {
			Resource* currentResource = (Resource*)current;
			bool rightResource = false;
			if (currentResource->resourceType == searching1) {
				searching1 = NO_RESOURCE;
				rightResource = true;
			}
			if (currentResource->resourceType == searching2) {
				searching2 = NO_RESOURCE;
				rightResource = true;
			}
			if (rightResource) {
				current->removeFromTile();
				current->alive = false;
				if (consuming) {
					life += LIFE;
					if (currentResource->resourceType > RESOURCE_TYPES) {
						life += LIFE;
						if (currentResource->resourceType > RESOURCE_TYPES * 2) {
							life += LIFE;
						}
					}
					resetActivity(false);
					return true;
				}
				else {
					if (carrying == NO_RESOURCE && storingResource > RESOURCE_TYPES) {
						carrying = currentResource->resourceType;
						life += LIFE * 3;
					}
					else {//It stores the resource (for the time being it just drops it)
						carrying = NO_RESOURCE;
						Resource* resource = new Resource(x, y, z, storingResource);
						resource->addToTile();
						resetActivity(true);
					}
					return true;
				}
			}
		}
		current = current->sharesTileWithObject;
	}
	return false;
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