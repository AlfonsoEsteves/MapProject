#pragma once

const unsigned char objectUnit = 0;
const unsigned char objectGenerator = 1;
const unsigned char objectResource = 2;

#ifdef DEBUG
extern int debug_objectsCreated;
extern int debug_objectsDestroyed;
#endif

class Object
{
public:
	int x, y, z;
	Object* sharesTileWithObject;
	unsigned char objectType;

	//Note that objects can not delete themselves or other objects
	//They just set their alive to false, and they will eventually be deleted by the map
	bool alive;

#   ifdef DEBUG
	int id;
#	endif

	Object(unsigned char _objectType, int _x, int _y, int _z);
	virtual ~Object();
	virtual void execute() = 0;
	virtual unsigned char type() = 0;
	void addToTile();
	void virtual addToTileExtra() {}
	void removeFromTile();
	void virtual removeFromTileExtra() {}

#	ifdef DEBUG
	void checkTileIsOccupiedByAliveObjects();
#	endif
};

