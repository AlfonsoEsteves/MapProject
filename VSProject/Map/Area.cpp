#include "Common.h"

#ifdef DEBUG
int debug_areasCreated=0;
int debug_areasDestroyed=0;
#endif

Area::Area() {//This is for the area of the dest tile in the pathfinder
#	ifdef DEBUG
	areaId = debug_areasCreated;
	debug_areasCreated++;
#	endif

	chunk = NULL;
	superArea = NULL;
	lvl = 0;
	lastCheckedByUnit = NULL;

	creo que esto no precisa ser safe
#	ifdef SAFE
	for (int i = 0; i < RESOURCE_TYPES * 2; i++) {
		resources[i] = 0;
	}
#	endif
}

Area::Area(Chunk * _chunk)
{
#	ifdef DEBUG
	debug_areasCreated++;
#	endif

	chunk = _chunk;
	superArea = NULL;
	lvl = chunk->lvl;
	lastCheckedByUnit =NULL;
}

Area::~Area()
{
#	ifdef DEBUG
	debug_areasDestroyed++;
#	endif
}

void Area::increaseResource(int resource) {
	resources[resource]++;
	if (superArea != NULL) {
		superArea->increaseResource(resource);
	}
}

void Area::decreaseResource(int resource) {
	resources[resource]--;
#	ifdef DEBUG
	if (resources[resource] < 0) {
		error("Negative amount of resource");
	}
#	endif
	if (superArea != NULL) {
		superArea->decreaseResource(resource);
	}
}
