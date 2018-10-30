#pragma once

#include "Area.h"
#include "Chunk.h"

class BaseChunk : public Chunk
{
private:
	void spreadArea(int i, int j, int k, Area* area);

public:
	BaseChunk(int _x, int _y, int _z);
	~BaseChunk();
	void setAreas();
	void joinWithAdjacentChunks();
};

