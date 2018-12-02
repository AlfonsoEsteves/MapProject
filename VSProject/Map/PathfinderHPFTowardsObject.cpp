#include "Common.h"

//This method should be used only to start the pathfinding and to reset after a new obstacle obstructed the path
int Unit::resetPathTowardsObject() {
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

	Area * oriArea = areasMap[x][y][z];
	Area * destArea = areasMap[destinationObject->x][destinationObject->y][destinationObject->z];

	if (oriArea == destArea) {
		lowestDestinationAreaReached = destArea;
		return dijkstraTowardsObject(false);
	}

	Area * oriSuperArea = oriArea->superArea;
	Area * destSuperArea = destArea->superArea;
	while (oriSuperArea != destSuperArea) {
		oriAreas[oriArea->lvl] = oriArea;
		oriArea = oriSuperArea;
		destArea = destSuperArea;
		oriSuperArea = oriSuperArea->superArea;
		destSuperArea = destSuperArea->superArea;
	}

	if (oriSuperArea == NULL) {
		//If the destination area and the origin are not connected I return PATH_NOT_FOUND
		return PATH_NOT_FOUND;
	}

	lowestDestinationAreaReached = oriSuperArea;

	return resetPathGoingFromAreaToArea(oriArea, destArea);
}

//This method adjusts the path of the unit as the unit moves.
//Remember that this method is executed only if the unit reaches at least one of its destination areas.
//If in an iteration the path isn't as good as before, the unit marks that a path reset is needed.
//To keep a path, the unit has to be at least 1 tile closer in each iteration.
int Unit::adjustPathTowardsObject() {
	Area * oriArea = areasMap[x][y][z];
	Area * destArea = areasMap[destinationObject->x][destinationObject->y][destinationObject->z];

	if (oriArea == destArea) {
		lowestDestinationAreaReached = destArea;
		return dijkstraTowardsObject(false);
	}

	while (true) {
		oriAreas[oriArea->lvl] = oriArea;

		//If the unit reached a destination area with a lower level then its lowestDestinationAreaReached, it reset all of its path.
		if (oriArea->superArea == destArea->superArea) {
			lowestDestinationAreaReached = oriArea->superArea;
			return resetPathGoingFromAreaToArea(oriArea, destArea);
		}

		//If the unit reached the level previous to its lowestDestinationAreaReached, it reset the whole path applying same-level-dijkstra.
		if (oriArea->lvl == lowestDestinationAreaReached->lvl - 1) {
			Area* nextArea = findNextAreaAux(oriArea, destArea, true, false);
			if (nextArea == NULL) {
				return PATH_OUTDATED;
			}
			if (oriArea->lvl == 0) {
				baseDestinationArea = nextArea;
				return dijkstraTowardsArea(nextArea, false);
			}
			else {
				destinationSuperAreas[oriArea->lvl - 1] = nextArea;
				return resetPathGoingFromAreasToSuperAreas(oriArea->lvl - 1);
			}
		}

		//The algorithim keeps on iterating until it reaches a super area destination that has not been reached yet
		if (oriArea->superArea != destinationSuperAreas[oriArea->lvl]) {
			//I check that the lowestDestinationAreaReached still has the destination object
			while (destArea->lvl < lowestDestinationAreaReached->lvl) {
				destArea = destArea->superArea;
			}
			if (destArea != lowestDestinationAreaReached) {
				return PATH_OUTDATED;
			}

			return resetPathGoingFromAreasToSuperAreas(oriArea->lvl);
		}

		oriArea = oriArea->superArea;
		destArea = destArea->superArea;
	}
}

//Thid method basically calls resetPathGoingFromAreasToSuperAreas
//But I needed because at the first level (the highest level) the search is a same-level-search
//  All the following searches in the lower levels are from-area-to-super-area searches
int Unit::resetPathGoingFromAreaToArea(Area* oriArea, Area* destArea) {
#	ifdef DEBUG
	if (lowestDestinationAreaReached->lvl != destArea->lvl + 1) {
		error("When executing same level dijkstra, the origin and destination areas should be one level below the lowest destination area reached.");
	}
#	endif
	if (oriArea->lvl > 0) {
		destinationSuperAreas[oriArea->lvl - 1] = findNextAreaAux(oriArea, destArea, false, false);
		return resetPathGoingFromAreasToSuperAreas(oriArea->lvl - 1);
	}
	else {
		baseDestinationArea = findNextAreaAux(oriArea, destArea, false, false);
		return dijkstraTowardsArea(baseDestinationArea, false);
	}
}

//This method decides the destination area in multiple levels
//It iterates starting from the highest levels going to the lowest levels
int Unit::resetPathGoingFromAreasToSuperAreas(int startingLevel) {
	while (startingLevel > 0) {
		Area* nextArea = findNextAreaAux(oriAreas[startingLevel], destinationSuperAreas[startingLevel], false, true);
		if (nextArea == NULL) {
			return PATH_OUTDATED;
		}
		destinationSuperAreas[startingLevel - 1] = nextArea;
		startingLevel--;
	}
	baseDestinationArea = findNextAreaAux(oriAreas[0], destinationSuperAreas[0], false, true);
	return dijkstraTowardsArea(baseDestinationArea, false);
}

//The last parameter indicates weather the ori is one lvl bellow dest or not
Area * Unit::findNextAreaAux(Area * oriArea, Area * destArea, bool adjusting, bool subArea) {
#	ifdef DEBUG
	if (oriArea == destArea) {
		error("findNextAreaAux should not be called with the same origin and destination area");
	}
#	endif

	circularArrayStart = 0;
	circularArrayEnd = 0;

	//I add the branches' beginnings to the queue
	for (std::vector<Area*>::iterator it = oriArea->adjacentAreas.begin(); it != oriArea->adjacentAreas.end(); it++) {
		Area* branch = *it;
		branch->pathFindingBranch = branch;
		branch->lastCheckedByUnit = this;
		branch->lastCheckedAtTime = time;
		circularArrayOfAreas[circularArrayEnd] = branch;
		circularArrayEnd++;
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
				return NULL;
			}
			firstAreaOfTheNextDistance = circularArrayEnd;
		}

		Area* area = circularArrayOfAreas[circularArrayStart];
		circularArrayStart = (circularArrayStart + 1) % CIRCULAR_ARRAY_OF_AREAS;

		if (subArea) {
			if (area->superArea == destArea) {
				chunksToBeTraveled[area->lvl] = distance;
				return area->pathFindingBranch;
			}
		}
		else {
			if (area == destArea) {
				chunksToBeTraveled[area->lvl] = distance;
				return area->pathFindingBranch;
			}
		}
		for (std::vector<Area*>::iterator it = area->adjacentAreas.begin(); it != area->adjacentAreas.end(); it++)
		{
			Area * followingArea = *it;
			if (followingArea->lastCheckedByUnit != this || followingArea->lastCheckedAtTime != time) {
				followingArea->pathFindingBranch = area->pathFindingBranch;
				followingArea->lastCheckedByUnit = this;
				followingArea->lastCheckedAtTime = time;
				circularArrayOfAreas[circularArrayEnd] = followingArea;
				circularArrayEnd++;
			}
		}
	}
}

int Unit::dijkstraTowardsObjectOrArea(bool adjusting) {
	Area* oriArea = areasMap[x][y][z];
	if (oriArea == baseDestinationArea) {
		return dijkstraTowardsObject(adjusting);
	}
	else {
		return dijkstraTowardsArea(baseDestinationArea, adjusting);
	}
}

int Unit::dijkstraTowardsObject(bool adjusting) {
	//Swap the tile's area with the destination area
	Area* temporary = areasMap[destinationObject->x][destinationObject->y][destinationObject->z];
	areasMap[destinationObject->x][destinationObject->y][destinationObject->z] = &destTileAuxArea;

	int direction = dijkstraTowardsArea(&destTileAuxArea, adjusting);

	//undo the swapping
	areasMap[destinationObject->x][destinationObject->y][destinationObject->z] = temporary;

	return direction;
}

int Unit::dijkstraTowardsArea(Area * dest, bool adjusting) {
	circularArrayStart = 0;
	circularArrayEnd = 0;
	for (int i = 0; i < 4; i++) {
		int nextTileX = x + getX(i);
		int nextTileY = y + getY(i);
		int nextTileZ = steppableTileHeight(x, y, z, nextTileX, nextTileY);
		if (nextTileZ != -1) {
			tileWasLastCheckedByUnit[nextTileX][nextTileY][nextTileZ] = this;
			tileWasLastCheckedAtTime[nextTileX][nextTileY][nextTileZ] = time;
			circularArrayOfTiles[circularArrayEnd].x = nextTileX;
			circularArrayOfTiles[circularArrayEnd].y = nextTileY;
			circularArrayOfTiles[circularArrayEnd].z = nextTileZ;
			circularArrayOfTiles[circularArrayEnd].direction = i;
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

		if (areasMap[tileX][tileY][tileZ] == dest) {
			tilesToBeTraveled = distance;
			return direction;
		}

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