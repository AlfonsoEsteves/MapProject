#include "Common.h"

/*
CHUNK CONFINMENT:
The pathfinding algorithim can be implemented with 'chunk confinment' or not.
And the 2 implementation will NOT work the same.
Chunk confinment means that the A* (either for the tile level or the area level)
will not continue spreading after it exceeds the end of the chunk.

The advantage of CC is that it will be more performant in most cases.

The advantage of not having CC is that the algorithim could find shorter paths.

I chose to NOT have CC, because with CC some paths can be a lot (quite a fucking lot) longer than they could be.
*/

Unit* tileWasLastCheckedByUnit[MAP_WIDTH][MAP_WIDTH][MAP_HEIGHT];//It indicates the last unit that checked a tile
int tileWasLastCheckedAtTime[MAP_WIDTH][MAP_WIDTH][MAP_HEIGHT];//It indicates the last time a tile was checked
Area * areasMap[MAP_WIDTH][MAP_WIDTH][MAP_HEIGHT];

Chunk**** chunks[LEVELS];

NearZone nearZones[MAP_WIDTH / NEAR_ZONE_DISTANCE][MAP_WIDTH / NEAR_ZONE_DISTANCE];

PFTile circularArrayOfTiles[CIRCULAR_ARRAY_OF_TILES];
Area* circularArrayOfAreas[CIRCULAR_ARRAY_OF_AREAS];
int circularArrayStart;
int circularArrayEnd;

Area destTileAuxArea;//This is a single tile area used to mark the destination when the unit reaches the last chunk

Area* oriAreas[LEVELS - 1];

void setAdjacencyBetweenLeftAndRightBaseChunks(BaseChunk * left, BaseChunk * right);
void setAdjacencyBetweenUpAndDownBaseChunks(BaseChunk * up, BaseChunk * down);
void setAdjacencyBetweenLeftAndRightSuperChunks(SuperChunk * left, SuperChunk * right);
void setAdjacencyBetweenUpAndDownSuperChunks(SuperChunk * up, SuperChunk * down);
void checkIfInnerChunksAreAdjacent(Chunk* chunk1, Chunk* chunk2);

void pathfinderInitialize() {
	int mapWidthInChunks = MAP_WIDTH / BASE_CHUNK_SIZE;
	int mapHeightInChunks = MAP_HEIGHT / BASE_CHUNK_SIZE;
	int chunkWidthInTiles = BASE_CHUNK_SIZE;
	for (int l = 0; l < LEVELS; l++) {
		chunks[l] = new Chunk***[mapWidthInChunks];
		for (int i = 0; i < mapWidthInChunks; i++) {
			chunks[l][i] = new Chunk**[mapWidthInChunks];
			for (int j = 0; j < mapWidthInChunks; j++) {
				chunks[l][i][j] = new Chunk*[mapHeightInChunks];
				for (int k = 0; k < mapHeightInChunks; k++) {
					if (l == 0) {
						chunks[l][i][j][k] = new BaseChunk(i*chunkWidthInTiles, j*chunkWidthInTiles, k*chunkWidthInTiles);
					}
					else {
						chunks[l][i][j][k] = new SuperChunk(i*chunkWidthInTiles, j*chunkWidthInTiles, k*chunkWidthInTiles, l);
					}
				}
			}
		}
		mapWidthInChunks /= SUPER_CHUNK_SIZE;
		mapHeightInChunks /= SUPER_CHUNK_SIZE;
		if (mapHeightInChunks == 0) {
			mapHeightInChunks = 1;
		}
		chunkWidthInTiles *= SUPER_CHUNK_SIZE;
	}

	//I set the inner chunks of each superchunk
	mapWidthInChunks = MAP_WIDTH / BASE_CHUNK_SIZE;
	mapHeightInChunks = MAP_HEIGHT / BASE_CHUNK_SIZE;
	for (int l = 0; l < LEVELS - 1; l++) {
		for (int i = 0; i < mapWidthInChunks; i++) {
			for (int j = 0; j < mapWidthInChunks; j++) {
				for (int k = 0; k < mapHeightInChunks || k==0; k++) {
					int superChunkX = i / SUPER_CHUNK_SIZE;
					int superChunkY = j / SUPER_CHUNK_SIZE;
					int superChunkZ = k / SUPER_CHUNK_SIZE;
					SuperChunk* superChunk = (SuperChunk*)chunks[l + 1][superChunkX][superChunkY][superChunkZ];
					superChunk->innerChunks[i % SUPER_CHUNK_SIZE][j % SUPER_CHUNK_SIZE][k % SUPER_CHUNK_SIZE] = chunks[l][i][j][k];
					chunks[l][i][j][k]->superChunk = superChunk;
				}
			}
		}
		mapWidthInChunks /= SUPER_CHUNK_SIZE;
		mapHeightInChunks /= SUPER_CHUNK_SIZE;
		if (mapHeightInChunks == 0) {
			mapHeightInChunks = 1;
		}
	}

	mapWidthInChunks = MAP_WIDTH / BASE_CHUNK_SIZE;
	mapHeightInChunks = MAP_HEIGHT / BASE_CHUNK_SIZE;
	for (int l = 0; l < LEVELS; l++) {
		//I create the areas
		for (int i = 0; i < mapWidthInChunks; i++) {
			for (int j = 0; j < mapWidthInChunks; j++) {
				for (int k = 0; k < mapHeightInChunks; k++) {
					chunks[l][i][j][k]->setAreas();
				}
			}
		}

		//I join the areas
		for (int i = 0; i < mapWidthInChunks; i++) {
			for (int j = 0; j < mapWidthInChunks; j++) {
				for (int k = 0; k < mapHeightInChunks; k++) {
					chunks[l][i][j][k]->joinWithAdjacentChunks();
				}
			}
		}

		mapWidthInChunks /= SUPER_CHUNK_SIZE;
		mapHeightInChunks /= SUPER_CHUNK_SIZE;
		if (mapHeightInChunks == 0) {
			mapHeightInChunks = 1;
		}
	}

	//Other initializations
	for (int i = 0; i < MAP_WIDTH; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			for (int k = 0; k < MAP_WIDTH; k++) {
				tileWasLastCheckedByUnit[i][j][k] = NULL;
				//Since the unit has been set to NULL
				//There is no need to set the time to a default value
			}
		}
	}
}

void setAdjacencyBetweenLeftAndRightBaseChunks(BaseChunk * left, BaseChunk * right) {
	for (int j = 0; j < BASE_CHUNK_SIZE; j++) {//j for the Y dimension
		for (int k = 0; k < BASE_CHUNK_SIZE; k++) {//k for the Z dimension
			if (tileIsSteppable(left->x + BASE_CHUNK_SIZE - 1, left->y + j, k) &&
				tileIsSteppable(right->x, right->y + j, k)) {
				Area* areaLeft = areasMap[left->x + BASE_CHUNK_SIZE - 1][left->y + j][k];
				Area* areaRight = areasMap[right->x][right->y + j][k];
				connectAreas(areaLeft, areaRight);
			}
		}
	}
}

void setAdjacencyBetweenUpAndDownBaseChunks(BaseChunk * up, BaseChunk * down) {
	for (int i = 0; i < BASE_CHUNK_SIZE; i++) {//i for the X dimension
		for (int k = 0; k < BASE_CHUNK_SIZE; k++) {//k for the Z dimension
			if (tileIsSteppable(up->x + i, up->y + BASE_CHUNK_SIZE - 1, k) &&
				tileIsSteppable(down->x + i, down->y, k)) {
				Area* areaUp = areasMap[up->x + i][up->y + BASE_CHUNK_SIZE - 1][k];
				Area* areaDown = areasMap[down->x + i][down->y][k];
				connectAreas(areaUp, areaDown);
			}
		}
	}
}

void setAdjacencyBetweenLeftAndRightSuperChunks(SuperChunk * left, SuperChunk * right) {
	int maxZ = SUPER_CHUNK_SIZE;
	if (left->lvl>=FIRST_FLAT_LEVEL) {
		maxZ = 1;
	}
	for (int y = 0; y < SUPER_CHUNK_SIZE; y++) {
		for (int z = 0; z < maxZ; z++) {
			Chunk* innerLeft = left->innerChunks[SUPER_CHUNK_SIZE - 1][y][z];
			Chunk* innerRight = right->innerChunks[0][y][z];
			checkIfInnerChunksAreAdjacent(innerLeft, innerRight);
		}
	}
}

void setAdjacencyBetweenUpAndDownSuperChunks(SuperChunk * up, SuperChunk * down) {
	int maxZ = SUPER_CHUNK_SIZE;
	if (up->lvl >= FIRST_FLAT_LEVEL) {
		maxZ = 1;
	}
	for (int x = 0; x < SUPER_CHUNK_SIZE; x++) {
		for (int z = 0; z < maxZ; z++) {
			Chunk* innerUp = up->innerChunks[x][SUPER_CHUNK_SIZE - 1][z];
			Chunk* innerDown = down->innerChunks[x][0][z];
			checkIfInnerChunksAreAdjacent(innerUp, innerDown);
		}
	}
}

void checkIfInnerChunksAreAdjacent(Chunk* chunk1, Chunk* chunk2) {
	for (std::list<Area*>::iterator it1 = chunk1->areas.begin(); it1 != chunk1->areas.end(); it1++) {
		Area * chunk1CurrentArea = *it1;
		for (std::list<Area*>::iterator it2 = chunk2->areas.begin(); it2 != chunk2->areas.end(); it2++) {
			Area * chunk2CurrentArea = *it2;
			if (std::find(chunk1CurrentArea->adjacentAreas.begin(), chunk1CurrentArea->adjacentAreas.end(), chunk2CurrentArea) != chunk1CurrentArea->adjacentAreas.end()) {
				connectAreas(chunk1CurrentArea->superArea, chunk2CurrentArea->superArea);
			}
		}
	}
}

void connectAreas(Area* area1, Area* area2) {
	//I connect the 2 areas only if they weren't connected before
	if (std::find(area1->adjacentAreas.begin(), area1->adjacentAreas.end(), area2) == area1->adjacentAreas.end()) {
		area1->adjacentAreas.push_back(area2);
		area2->adjacentAreas.push_back(area1);
	}
}

void pathfinderClose() {
	int mapWidthInChunks = MAP_WIDTH / BASE_CHUNK_SIZE;
	int mapHeightInChunks = MAP_HEIGHT / BASE_CHUNK_SIZE;
	int chunkWidthInTiles = BASE_CHUNK_SIZE;
	for (int l = 0; l < LEVELS; l++) {
		for (int i = 0; i < mapWidthInChunks; i++) {
			for (int j = 0; j < mapWidthInChunks; j++) {
				for (int k = 0; k < mapHeightInChunks; k++) {
					chunks[l][i][j][k]->~Chunk();
				}
				delete[] chunks[l][i][j];
			}
			delete[] chunks[l][i];
		}
		delete[] chunks[l];
		mapWidthInChunks /= SUPER_CHUNK_SIZE;
		mapHeightInChunks /= SUPER_CHUNK_SIZE;
		if (mapHeightInChunks == 0) {
			mapHeightInChunks = 1;
		}
		chunkWidthInTiles *= SUPER_CHUNK_SIZE;
	}
}