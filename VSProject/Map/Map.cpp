#include "Common.h"

void execute_frame()
{
	Area* asd = areasMap[172][228][13];
	for (int i = 0; i < RESOURCE_TYPES * 2; i++) {
		printf("%d", asd->resources[i]);
	}
	printf("\n");








	for (std::list<Object*>::iterator it = objects[time % BUCKETS].begin(); it != objects[time % BUCKETS].end(); it++) {
		Object* object = *it;
#		ifdef DEBUG
		lastExecutedObject = object->id;
		if (object->id == DEBUG_OBJECT) {
			if (object->type() == objectUnit) {
				Unit* unit = (Unit*)object;
				if (time + unit->slowness == DEBUG_TIME) {
					printf("");
				}
			}
			if (time == DEBUG_TIME) {
				printf("");
			}
		}
#		endif

		if (object->alive) {
			object->execute();
#			ifdef DEBUG
			object->checkTileIsOccupiedByAliveObjects();
#			endif
		}
		//The execution may have set alive to false
		if (!object->alive) {
			delete object;
		}
	}
	objects[time % BUCKETS].clear();
	time++;
}

void map_close() {
	for (int i = 0; i < BUCKETS; i++) {
		for (std::list<Object*>::iterator it = objects[i].begin(); it != objects[i].end(); it++) {
			(*it)->~Object();
		}
	}
	pathfinderClose();

#	ifdef DEBUG
	if (debug_areasCreated != debug_areasDestroyed + 1) {
		//The destination dummy area of the Pathfinder algorithm will be destroyed at the main execution end
		error("AREAS NOT DESTROYED");
	}
	if (debug_chunksCreated != debug_chunksDestroyed) {
		error("CHUNKS NOT DESTROYED");
	}
	if (debug_objectsCreated != debug_objectsDestroyed) {
		error("OBJECTS NOT DESTROYED");
	}
#	endif
}

bool tileIsSteppable(const int& x, const int& y, const int& z) {
	return tilesMap[x][y][z]==tileEmpty && tilesMap[x][y][z-1] < tileEmpty;
}

//This method returns the height of the tile the unit wants to move towards.
//It returns -1 if the unit can not move in that direction.
int steppableTileHeight(const int& x, const int& y, const int& z, const int& nextX, const int& nextY) {
	if (tilesMap[nextX][nextY][z] == tileEmpty) {
		if (tilesMap[nextX][nextY][z - 1] < tileEmpty) {
			return z;
		}
		else {
			if (tilesMap[nextX][nextY][z - 1] == tileEmpty) {
				if (tilesMap[nextX][nextY][z - 2] < tileEmpty) {
					return z - 1;
				}
			}
		}
	}
	else {
		if (tilesMap[nextX][nextY][z] < tileEmpty) {
			if (tilesMap[nextX][nextY][z + 1] == tileEmpty) {
				if (tilesMap[x][y][z + 1] == tileEmpty) {
					return z + 1;
				}
			}
		}
	}
	return -1;
}