#include "Common.h"

Resource::Resource(int _x, int _y, int _z, char _resourceType) : Object(_x, _y, _z) {
	resourceType = _resourceType;
	objects[(time + (rand() % RESOURCE_SLOWNESS)) % BUCKETS].push_back(this);
}

Resource::~Resource() {
}

unsigned char Resource::type() {
	return objectResource;
}

void Resource::execute() {
	int beginning = rand() % 4;
	int increment = (rand() % 2) * 2 + 1;//Either 1 or 3
	for (int i = 0; i < 4; i++) {
		int dir = (beginning + i * increment) % 4;
		int nextX = x + getX(dir);
		int nextY = y + getY(dir);
		int nextZ = steppableTileHeight(x, y, z, nextX, nextY);
		if (nextZ != -1) {
			Area* oldArea = areasMap[x][y][z];
			removeFromTile();
			Object* current = unitsMap[nextX][nextY][nextZ];
			while (current != NULL) {
				if (current->type() == objectResource && current->resourceType == resourceType) {
					Unit* unit = new Unit(nextX, nextY, nextZ, LIFE);
					//This line will depend on the implementation of the resourceType assignment of units
					unit->cycle[0]=(resourceType - 1 + RESOURCE_TYPES) % RESOURCE_TYPES;
					unit->cycleLength = 1;
					unit->cycleCurrentStep = 0;
					unit->adjustResourceType();
					current->removeFromTile();
					current->alive = false;
					areasMap[nextX][nextY][nextZ]->decreaseResource(resourceType);
					alive = false;
					oldArea->decreaseResource(resourceType);
#					ifdef DEBUG
					if (unit->resourceType != resourceType) {
						error("When 2 resources of the same type collide they should create a unit of the same type");
					}
#					endif
					return;
				}
				current = current->sharesTileWithObject;
			}
			x = nextX;
			y = nextY;
			z = nextZ;
			addToTile();
			if (oldArea != areasMap[x][y][z]) {
				oldArea->decreaseResource(resourceType);
				areasMap[x][y][z]->increaseResource(resourceType);
			}
			break;
		}
	}
	objects[(time +  RESOURCE_SLOWNESS) % BUCKETS].push_back(this);
}