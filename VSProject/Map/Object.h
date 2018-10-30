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
	char resourceType;

	//An object can delete itself, but it can not delete another object.
	//If A wants to delete B, A has to set B's active to false, and B will eventually be deleted
	bool alive;

#   ifdef DEBUG
	int id;
#	endif

	Object(int _x, int _y, int _z);
	virtual ~Object();
	virtual void execute()=0;
	virtual unsigned char type()=0;
	void removeFromTile();
	void addToTile();
};

