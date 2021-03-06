#include "Common.h"

Generator::Generator(int _x, int _y, int _z) : Object(objectGenerator, _x, _y, _z)
{
	resourceGeneration = rand() % RESOURCE_TYPES;
	duration = GENERATION_DURATION / 2 + (rand() % (GENERATION_DURATION / 2 + 1));
	objects[(time + (rand() % duration)) % BUCKETS].push_back(this);
}

Generator::~Generator()
{
}

unsigned char Generator::type() {
	return objectGenerator;
}

void Generator::execute() {
	Resource* resource = new Resource(x, y, z + 1, resourceGeneration);
	resource->addToTile();
	objects[(time + duration) % BUCKETS].push_back(this);
}