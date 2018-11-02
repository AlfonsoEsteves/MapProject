#include "Common.h"

#ifdef DEBUG
int debug_objectsCreated = 0;
int debug_objectsDestroyed = 0;
#endif

Object::Object(unsigned char _objectType, int _x, int _y, int _z)
{
#	ifdef DEBUG
	id = debug_objectsCreated;
	debug_objectsCreated++;
	if (id == DEBUG_OBJECT) {
		selected = this;
	}
	lastExecutedObject = -id;
#	endif
	objectType = _objectType;
	x = _x;
	y = _y;
	z = _z;
	alive = true;
	addToTile();
}

Object::~Object(){
#	ifdef DEBUG
	debug_objectsDestroyed++;
#	endif
	if (this == selected) {
		selected = NULL;
	}
}

void Object::addToTile() {
	sharesTileWithObject = unitsMap[x][y][z];
	unitsMap[x][y][z] = this;
}

void Object::removeFromTile() {
	Object* current = unitsMap[x][y][z];
	if (current == this) {
		unitsMap[x][y][z] = sharesTileWithObject;
	}
	else {
		while (current->sharesTileWithObject != this) {
			current = current->sharesTileWithObject;
		}
		current->sharesTileWithObject = sharesTileWithObject;
	}
#	ifdef SAFE
	sharesTileWithObject = NULL;
#	endif
}

#ifdef DEBUG
void Object::checkTileIsOccupiedByAliveObjects() {
	Object* current = unitsMap[x][y][z];
	while (current != NULL) {
		if (!current->alive) {
			error("Only alive objects can occupy the tiles");
		}
		current = current->sharesTileWithObject;
	}
}
#endif