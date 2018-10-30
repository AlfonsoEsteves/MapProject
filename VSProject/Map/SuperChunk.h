#pragma once

#include <list>
#include "Parameters.h"
#include "Area.h"
#include "BaseChunk.h"

class SuperChunk : public Chunk
{
private:
	void spreadArea(Area* subArea, Area* area);

public:
	Chunk * innerChunks[SUPER_CHUNK_SIZE][SUPER_CHUNK_SIZE][SUPER_CHUNK_SIZE];

	SuperChunk(int _x, int _y, int _z, int _lvl);
	~SuperChunk();
	void setAreas();

	void joinWithAdjacentChunks();
};

