#include "Common.h"

//THESE FUNCTIONS ARE CURRENTLY NOT BEEN USER

int dummyMovementTowardsPoint(int oriX, int oriY, int oriZ, int destX, int destY, int destZ) {
	int hDiff = abs(oriX - destX);
	int vDiff = abs(oriY - destY);
	if (hDiff > vDiff) {
		if (destX > oriX) {
			if (steppableTileHeight(oriX, oriY, oriZ, oriX + 1, oriY) != -1) {
				return 0;
			}
		}
		else {
			if (steppableTileHeight(oriX, oriY, oriZ, oriX - 1, oriY) != -1) {
				return 2;
			}
		}
		if (oriY == destY) {
			return -1;
		}
		else {
			if (destY > oriY) {
				if (steppableTileHeight(oriX, oriY, oriZ, oriX, oriY + 1) != -1) {
					return 1;
				}
			}
			else {
				if (steppableTileHeight(oriX, oriY, oriZ, oriX, oriY - 1) != -1) {
					return 3;
				}
			}
		}
	}
	else {
		if (destY > oriY) {
			if (steppableTileHeight(oriX, oriY, oriZ, oriX, oriY + 1) != -1) {
				return 1;
			}
		}
		else {
			if (steppableTileHeight(oriX, oriY, oriZ, oriX, oriY - 1) != -1) {
				return 3;
			}
		}
		if (oriX == destX) {
			return -1;
		}
		else {
			if (destX > oriX) {
				if (steppableTileHeight(oriX, oriY, oriZ, oriX + 1, oriY) != -1) {
					return 0;
				}
			}
			else {
				if (steppableTileHeight(oriX, oriY, oriZ, oriX - 1, oriY) != -1) {
					return 2;
				}
			}
		}
	}
	return -1;
}

bool checkDummyMovementTowardsPoint(int oriX, int oriY, int oriZ, int destX, int destY, int destZ) {
	while (oriX != destX && oriY != destY) {
		int dir=dummyMovementTowardsPoint(oriX, oriY, oriZ, destX, destY, destZ);
		if (dir == -1) {
			return false;
		}
		else {
			int nextX = oriX + getX(dir);
			int nextY = oriY + getY(dir);
			oriZ = steppableTileHeight(oriX, oriY, oriZ, nextX, nextY);
			oriX = nextX;
			oriY = nextY;
		}
	}
	if (oriZ == destZ) {
		return true;
	}
	else {
		return false;
	}
}

int dummyMovementTowardsArea(int oriX, int oriY, int oriZ, Area* dest) {
	Chunk* oriChunk = areasMap[oriX][oriY][oriZ]->chunk;
	Chunk* destChunk = dest->chunk;
	#ifdef DEBUG
	if (oriChunk == destChunk) {
		error("oriChunk and destChunkmust not be the same");
	}
	#endif
	if (destChunk->x > oriChunk->x) {
		if (steppableTileHeight(oriX, oriY, oriZ, oriX+1, oriY)!=-1) {
			return 0;
		}
		else {
			return -1;
		}
	}
	if (destChunk->y > oriChunk->y) {
		if (steppableTileHeight(oriX, oriY, oriZ, oriX, oriY + 1) != -1) {
			return 1;
		}
		else {
			return -1;
		}
	}
	if (destChunk->x < oriChunk->x) {
		if (steppableTileHeight(oriX, oriY, oriZ, oriX - 1, oriY) != -1) {
			return 2;
		}
		else {
			return -1;
		}
	}
	if (destChunk->y < oriChunk->y) {
		if (steppableTileHeight(oriX, oriY, oriZ, oriX, oriY - 1) != -1) {
			return 3;
		}
		else {
			return -1;
		}
	}
	//If the destination area is right above or right below the current area, it returns -1
	return -1;
}

bool checkDummyMovementTowardsArea(int oriX, int oriY, int oriZ, Area* dest) {
	Area* oriArea = areasMap[oriX][oriY][oriZ];
	while (areasMap[oriX][oriY][oriZ] == oriArea) {
		int dir = dummyMovementTowardsArea(oriX, oriY, oriZ, dest);
		if (dir == -1) {
			return false;
		}
		else {
			int nextX = oriX + getX(dir);
			int nextY = oriY + getY(dir);
			oriZ = steppableTileHeight(oriX, oriY, oriZ, nextX, nextY);
			oriX = nextX;
			oriY = nextY;
		}
	}
	if (areasMap[oriX][oriY][oriZ] == dest) {
		return true;
	}
	else {
		return false;
	}
}
