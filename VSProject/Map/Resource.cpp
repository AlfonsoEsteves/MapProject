#include "Common.h"

Resource::Resource(int _x, int _y, int _z, char _resourceType) : Object(objectResource, _x, _y, _z) {
	resourceType = _resourceType;
	objects[(time + (rand() % RESOURCE_SLOWNESS)) % BUCKETS].push_back(this);
}

Resource::~Resource() {
}

unsigned char Resource::type() {
	return objectResource;
}

void Resource::execute() {
	removeFromTile();
	
	//It adds the branches' beginnings to the queue
	int dir = rand() % 4;
	int nextX = x + getX(dir);
	int nextY = y + getY(dir);
	int nextZ = steppableTileHeight(x, y, z, nextX, nextY);
	if (nextZ != -1) {
		x = nextX;
		y = nextY;
		z = nextZ;
		Object* current = unitsMap[x][y][z];
		while (current != NULL) {
			if (current->type() == objectResource) {
				Resource* currentResource = (Resource*)current;
				if (currentResource->resourceType == resourceType) {
					Unit* unit = new Unit(x, y, z, LIFE, -1);
					unit->addToTile();
					current->removeFromTile();
					current->alive = false;
					alive = false;
					return;
				}
			}
			current = current->sharesTileWithObject;
		}
	}

	addToTile();
	objects[(time +  RESOURCE_SLOWNESS) % BUCKETS].push_back(this);
}

void Resource::addToTileExtra() {
	areasMap[x][y][z]->increaseResource(resourceType);
}

void Resource::removeFromTileExtra() {
	areasMap[x][y][z]->decreaseResource(resourceType);
}