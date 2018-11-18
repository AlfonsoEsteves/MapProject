#include "Common.h"

//This method should be used only to start the pathfinding and to reset it after a new obstacle obstructs the current path
int Unit::resetPathTowardsResource() {
	lowestDestinationAreaReached = NULL;
	reachedDestinationBaseArea = false;
	baseDestinationArea = NULL;

#	ifdef SAFE
	for (int i = 0; i < LEVELS - 1; i++) {
		destinationSuperAreas[i] = NULL;
		chunksToBeTraveled[i] = -1;
		oriAreas[i] = NULL;
	}
	tilesToBeTraveled = -1;
#   endif

	Area * area = areasMap[x][y][z];

	if (area->resources[resourceSearchStatus]>0) {
		lowestDestinationAreaReached = area;
		return dijkstraTowardsResourceOrArea(false);
	}

	while (area->resources[resourceSearchStatus]==0) {
		oriAreas[area->lvl] = area;
		area = area->superArea;
		if (area == NULL) {
			return PATH_NOT_FOUND;
		}
	}

	lowestDestinationAreaReached = area;

	return resetPathTowardsResource(area->lvl - 1);
}

int Unit::adjustPathTowardsResource() {
	Area * reachedArea = areasMap[x][y][z];

	if (reachedArea->resources[resourceSearchStatus]>0) {
		lowestDestinationAreaReached = reachedArea;
		baseDestinationArea = NULL;
		return dijkstraTowardsResourceOrArea(false);
	}

	//Here the unit decides how much of its path it should reset
	//  If it reached a level previous to its lowestDestinationAreaReached, it will reset all of it
	//  Otherwise it will reset the areas it already reached
	while (true) {
		oriAreas[reachedArea->lvl] = reachedArea;

		//Even if the unit didn't reach its lowestDestinationAreaReached, a resource may 
		//have popped up nearby, so the lowestDestinationAreaReached should be updated.
		if (reachedArea->superArea->resources[resourceSearchStatus]>0) {
			lowestDestinationAreaReached = reachedArea->superArea;
			return resetPathTowardsResource(reachedArea->lvl);
		}

		//If the unit reached a level previous to its lowestDestinationAreaReached, it reset all of its path
		if (reachedArea->lvl == lowestDestinationAreaReached->lvl - 1) {
			//I check that the lowestDestinationAreaReached still has the resource
			//  because it could have been removed
			if (lowestDestinationAreaReached->resources[resourceSearchStatus] > 0) {
				return resetPathTowardsResource(reachedArea->lvl);
			}
			else {
				return PATH_OUTDATED;
			}
		}

		//Check if the current super area destination has not been reached yet
		if (reachedArea->superArea != destinationSuperAreas[reachedArea->lvl]) {
			//I check that the destinationSuperArea still has the resource
			//  because it could have been removed
			if (lowestDestinationAreaReached->resources[resourceSearchStatus] > 0) {
				return resetPathTowardsResourceOrDestinationSuperArea(reachedArea->lvl);
			}
			else {
				return PATH_OUTDATED;
			}
		}

		reachedArea = reachedArea->superArea;
	}
}

int Unit::resetPathTowardsResource(int startingMacroLevel) {
	destinationSuperAreas[startingMacroLevel] = NULL;
	return resetPathTowardsResourceOrDestinationSuperArea(startingMacroLevel);
}

int Unit::resetPathTowardsResourceOrDestinationSuperArea(int startingMacroLevel) {
	while (startingMacroLevel > 0) {
		destinationSuperAreas[startingMacroLevel - 1] = findNextAreaTowardsResourceOrSuperArea(oriAreas[startingMacroLevel], destinationSuperAreas[startingMacroLevel], false);
		startingMacroLevel--;
	}
	baseDestinationArea = findNextAreaTowardsResourceOrSuperArea(oriAreas[0], destinationSuperAreas[0], false);
	return dijkstraTowardsResourceOrArea(false);
}

//The last parameter indicates weather the ori is one lvl bellow dest or not
Area* Unit::findNextAreaTowardsResourceOrSuperArea(Area * oriArea, Area * destArea, bool adjusting) {
#	ifdef DEBUG
	if (oriArea == destArea) {
		error("findNextAreaTowardsResourceOrArea should not be called with the same origin and destination area");
	}
#	endif

	circularArrayStart = 0;
	circularArrayEnd = 0;

	//It adds the branches' beginnings to the queue
	std::vector<Area*>::iterator it;
	std::vector<Area*>::iterator listBeginning = oriArea->adjacentAreas.begin();
	std::vector<Area*>::iterator listEnd = oriArea->adjacentAreas.end();
	bool foward = (rand() % 2 == 0);//It randomizes the iteration order to solve the "always right issue"
	if (foward) {
		it = oriArea->adjacentAreas.begin();
	}
	else {
		it = oriArea->adjacentAreas.end();
	}
	while (true) {
		if(!foward) {
			if (it == listBeginning) {
				break;
			}
			it--;
		}
		Area* branch = *it;
		branch->pathFindingBranch = branch;
		branch->lastCheckedByUnit = this;
		branch->lastCheckedAtTime = time;
		circularArrayOfAreas[circularArrayEnd] = branch;
		circularArrayEnd++;
		if (foward) {
			it++;
			if (it == listEnd) {
				break;
			}
		}
	}

	int distance = 1;
	int firstAreaOfTheNextDistance = circularArrayEnd;
	while (true) {
#       ifdef DEBUG
		if (circularArrayStart == circularArrayEnd) {
			error("The circular array of areas is empty");
		}
		if ((circularArrayEnd + 1) % CIRCULAR_ARRAY_OF_AREAS == circularArrayStart) {
			error("The circular array of areas is about overflow");
		}
#       endif

		if (circularArrayStart == firstAreaOfTheNextDistance) {
			distance++;
			//Bear in mind that the chunksToBeTraveled will normally be one less than compared to when
			//the distance was set, because this method gets excuted after the unit advances one chunk
			if (adjusting && distance >= chunksToBeTraveled[oriArea->lvl]) {
#				ifdef LOG_PF
				if (unitId == DEBUG_UNIT) {
					printf("Current path is too long, expected %d c%d or less\n\n", chunksToBeTraveled[oriArea->lvl], oriArea->lvl);
				}
#				endif
				return NULL;
			}
			firstAreaOfTheNextDistance = circularArrayEnd;
		}

		Area* currentArea = circularArrayOfAreas[circularArrayStart];
		circularArrayStart = (circularArrayStart + 1) % CIRCULAR_ARRAY_OF_AREAS;

		if (currentArea->superArea == destArea || currentArea->resources[resourceSearchStatus]>0) {
			chunksToBeTraveled[currentArea->lvl] = distance;
			return currentArea->pathFindingBranch;
		}

		//It checks the current area's adjacent areas
		//Regarding the "always right issue", remember that it is irrelevant the order the adjacent areas are iterated here
		//Even if the area that ends up reaching the goal could differ, the branch won't differ, and it is the branch what
		//determines where the unit will go
		for (std::vector<Area*>::iterator it = currentArea->adjacentAreas.begin(); it != currentArea->adjacentAreas.end(); it++) {
			Area * followingArea = *it;
			if (followingArea->lastCheckedByUnit != this || followingArea->lastCheckedAtTime != time) {
				followingArea->pathFindingBranch = currentArea->pathFindingBranch;
				followingArea->lastCheckedByUnit = this;
				followingArea->lastCheckedAtTime = time;
				circularArrayOfAreas[circularArrayEnd] = followingArea;
				circularArrayEnd++;
			}
		}
	}
}

int Unit::dijkstraTowardsResourceOrArea(bool adjusting) {
	circularArrayStart = 0;
	circularArrayEnd = 0;

	//It adds the branches' beginnings to the queue
	int dir;
	bool foward = (rand() % 2 == 0);//It randomizes the iteration order to solve the "always right issue"
	for (int i = 0; i < 4; i++) {
		if (foward) {
			dir = i;
		}
		else {
			dir = 3 - i;
		}
		int nextTileX = x + getX(dir);
		int nextTileY = y + getY(dir);
		int nextTileZ = steppableTileHeight(x, y, z, nextTileX, nextTileY);
		if (nextTileZ != -1) {
			tileWasLastCheckedByUnit[nextTileX][nextTileY][nextTileZ] = this;
			tileWasLastCheckedAtTime[nextTileX][nextTileY][nextTileZ] = time;
			circularArrayOfTiles[circularArrayEnd].x = nextTileX;
			circularArrayOfTiles[circularArrayEnd].y = nextTileY;
			circularArrayOfTiles[circularArrayEnd].z = nextTileZ;
			circularArrayOfTiles[circularArrayEnd].direction = dir;
			circularArrayEnd++;
		}
	}
	int distance = 1;
	int firstTileOfTheNextDistance = circularArrayEnd;
	while (true) {
#       ifdef DEBUG
		if (circularArrayStart == circularArrayEnd) {
			error("The circular array of tiles is empty");
		}
		if ((circularArrayEnd + 1) % CIRCULAR_ARRAY_OF_TILES == circularArrayStart) {
			error("The circular array of tiles is about to overflow");
		}
#       endif

		if (circularArrayStart == firstTileOfTheNextDistance) {
			distance++;
			if (adjusting && distance >= tilesToBeTraveled) {
#				ifdef LOG_PF
				if (unitId == DEBUG_UNIT) {
					printf("Current path is too long, expected %d tiles or less\n\n", tilesToBeTraveled);
				}
#				endif
				return PATH_OUTDATED;
			}
			firstTileOfTheNextDistance = circularArrayEnd;
		}

		PFTile tile = circularArrayOfTiles[circularArrayStart];
		circularArrayStart = (circularArrayStart + 1) % CIRCULAR_ARRAY_OF_TILES;

		int tileX = tile.x;
		int tileY = tile.y;
		int tileZ = tile.z;
		int direction = tile.direction;

		if (areasMap[tileX][tileY][tileZ] == baseDestinationArea) {
			tilesToBeTraveled = distance;
			return direction;
		}

		Object* current = unitsMap[tileX][tileY][tileZ];
		while (current != NULL) {
			if (current->providesResource(resourceSearchStatus)) {
				tilesToBeTraveled = distance;
				return direction;
			}
			current = current->sharesTileWithObject;
		}

		//It checks the current tile's adjacent tiles
		//Regarding the "always right issue", remember that it is irrelevant the order the adjacent tiles are iterated here
		//Even if the tile that ends up reaching the goal could differ, the branch won't differ, and it is the branch what
		//determines where the unit will go
		for (int i = 0; i < 4; i++) {
			int tileNextX = tileX + getX(i);
			int tileNextY = tileY + getY(i);
			int tileNextZ = steppableTileHeight(tileX, tileY, tileZ, tileNextX, tileNextY);
			if (tileNextZ != -1) {
				if (tileWasLastCheckedAtTime[tileNextX][tileNextY][tileNextZ] != time || tileWasLastCheckedByUnit[tileNextX][tileNextY][tileNextZ] != this) {
					tileWasLastCheckedAtTime[tileNextX][tileNextY][tileNextZ] = time;
					tileWasLastCheckedByUnit[tileNextX][tileNextY][tileNextZ] = this;
					circularArrayOfTiles[circularArrayEnd].x = tileNextX;
					circularArrayOfTiles[circularArrayEnd].y = tileNextY;
					circularArrayOfTiles[circularArrayEnd].z = tileNextZ;
					circularArrayOfTiles[circularArrayEnd].direction = direction;
					circularArrayEnd = (circularArrayEnd + 1) % CIRCULAR_ARRAY_OF_TILES;
				}
			}
		}
	}
}