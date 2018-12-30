#pragma once
//#include <list>

class Area;
class SuperChunk;

extern int debug_chunksCreated;
extern int debug_chunksDestroyed;

class Chunk
{
public:
	int x, y, z;
	int lvl;
	std::list<Area*> areas;
	SuperChunk* superChunk;

	Chunk(int _x, int _y, int _z, int _lvl);
	~Chunk();

	virtual void setAreas()=0;

	//This method sets the adjacent areas of this chunk's areas
	virtual void joinWithAdjacentChunks()=0;
};

