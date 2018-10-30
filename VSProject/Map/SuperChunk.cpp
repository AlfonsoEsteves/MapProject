#include "Common.h"

SuperChunk::SuperChunk(int _x, int _y, int _z, int _lvl) : Chunk(_x, _y, _z, _lvl)
{
}

SuperChunk::~SuperChunk()
{
	//The chunks of the inner chunks matrix are destroyed by the pathfinderClose method
}

void SuperChunk::setAreas() {
	int maxZ = SUPER_CHUNK_SIZE;
	if (lvl >= FIRST_FLAT_LEVEL) {
		maxZ = 1;
	}
	for (int i = 0; i < SUPER_CHUNK_SIZE; i++) {
		for (int j = 0; j < SUPER_CHUNK_SIZE; j++) {
			for (int k = 0; k < maxZ; k++) {
				Chunk* subChunk = innerChunks[i][j][k];
				for (std::list<Area*>::iterator it = subChunk->areas.begin(); it != subChunk->areas.end(); it++)
				{
					Area* subArea = *it;
					if (subArea->superArea == NULL) {
						Area* area = new Area(this);
						areas.push_back(area);
						spreadArea(subArea, area);
					}
				}
			}
		}
	}
}

void SuperChunk::spreadArea(Area* subArea, Area* area) {
	subArea->superArea = area;
	for (std::list<Area*>::iterator it = subArea->adjacentAreas.begin(); it != subArea->adjacentAreas.end(); it++)
	{
		Area* adjacentArea = *it;
		if (adjacentArea->superArea == NULL && adjacentArea->chunk->superChunk==area->chunk) {
			spreadArea(adjacentArea, area);
		}
	}
}

void SuperChunk::joinWithAdjacentChunks() {
	int maxZ = SUPER_CHUNK_SIZE;
	if (lvl >= FIRST_FLAT_LEVEL) {
		maxZ = 1;
	}
	for (int i = 0; i < SUPER_CHUNK_SIZE; i++) {
		for (int j = 0; j < SUPER_CHUNK_SIZE; j++) {
			for (int k = 0; k < maxZ; k++) {
				Chunk* subChunk = innerChunks[i][j][k];
				for (std::list<Area*>::iterator it = subChunk->areas.begin(); it != subChunk->areas.end(); it++)
				{
					Area* subArea = *it;
					for (std::list<Area*>::iterator it2 = subArea->adjacentAreas.begin(); it2 != subArea->adjacentAreas.end(); it2++) {
						Area* adjacentSubArea = *it2;
						//If both sub areas belong to the same super area, they are both within this chunk
						if (subArea->superArea != adjacentSubArea->superArea) {
							connectAreas(subArea->superArea, adjacentSubArea->superArea);
						}
					}
				}
			}
		}
	}
}