#include "Common.h"


#ifdef DEBUG
int debug_chunksCreated = 0;
int debug_chunksDestroyed = 0;
#endif

Chunk::Chunk(int _x, int _y, int _z, int _lvl)
{
#	ifdef DEBUG
	debug_chunksCreated++;
#	endif

	x = _x;
	y = _y;
	z = _z;
	lvl = _lvl;
}

Chunk::~Chunk()
{
#	ifdef DEBUG
	debug_chunksDestroyed++;
#	endif

	for (std::list<Area*>::iterator it = areas.begin(); it != areas.end(); it++)
	{
		(*it)->~Area();
	}
}
