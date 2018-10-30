#include "Common.h"

BaseChunk::BaseChunk(int _x, int _y, int _z) : Chunk(_x, _y, _z, 0)
{
}

BaseChunk::~BaseChunk()
{
}

void BaseChunk::setAreas() {
	for (int i = 0; i < BASE_CHUNK_SIZE; i++) {
		for (int j = 0; j < BASE_CHUNK_SIZE; j++) {
			for (int k = 0; k < BASE_CHUNK_SIZE; k++) {
				areasMap[x + i][y + j][z + k] = NULL;
			}
		}
	}
	for (int i = 0; i < BASE_CHUNK_SIZE; i++) {
		for (int j = 0; j < BASE_CHUNK_SIZE; j++) {
			for (int k = 0; k < BASE_CHUNK_SIZE; k++) {
				if (areasMap[x + i][y + j][z + k] == NULL && tileIsSteppable(x + i, y + j, z + k)) {
					Area* area = new Area(this);
					areas.push_back(area);
					spreadArea(i, j, k, area);
				}
			}
		}
	}
}

void BaseChunk::spreadArea(int i, int j, int k, Area* area) {
	int thisX = x + i;
	int thisY = y + j;
	int thisZ = z + k;
	if (areasMap[thisX][thisY][thisZ] == NULL && tileIsSteppable(thisX, thisY, thisZ)) {
		areasMap[thisX][thisY][thisZ] = area;
		for (int d = 0; d < 4; d++) {
			int nextI = i + getX(d);
			int nextJ = j + getY(d);
			if (nextI >= 0 && nextI<BASE_CHUNK_SIZE && nextJ >= 0 && nextJ<BASE_CHUNK_SIZE) {
				int nextZ = steppableTileHeight(thisX, thisY, thisZ, x + nextI, y + nextJ);
				if (nextZ != -1) {
					int nextK = nextZ - z;
					if (nextK >= 0 && nextK < BASE_CHUNK_SIZE) {
						spreadArea(nextI, nextJ, nextK, area);
					}
				}
			}
		}
	}
}

void BaseChunk::joinWithAdjacentChunks() {
	for (int currentX = x; currentX < x + BASE_CHUNK_SIZE; currentX++) {
		for (int currentY = y; currentY < y + BASE_CHUNK_SIZE; currentY++) {
			for (int currentZ = z; currentZ < z + BASE_CHUNK_SIZE; currentZ++) {
				Area* currentArea = areasMap[currentX][currentY][currentZ];
				if (currentArea != NULL) {
					for (int dir = 0; dir < 4; dir++) {
						int nextX = currentX + getX(dir);
						int nextY = currentY + getY(dir);
						//The current area must be inside the chunk
						//But the next area can belong to an adjacent chunk
						if (nextX >= 0 && nextX<MAP_WIDTH && nextY >= 0 && nextY<MAP_WIDTH) {
							int nextZ = steppableTileHeight(currentX, currentY, currentZ, nextX, nextY);
							if (nextZ != -1) {
								Area* nextArea = areasMap[nextX][nextY][nextZ];
								if (nextArea != NULL) {
									if (currentArea != nextArea) {
										connectAreas(currentArea, nextArea);
									}
								}
							}
						}
					}
				}
			}
		}
	}
}