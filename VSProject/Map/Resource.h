#pragma once

#define RESOURCE_SLOWNESS 40

class Resource : public Object {
public:
	char resourceType;

	Resource(int _x, int _y, int _z, char _resourceType);
	~Resource();
	void execute();
	unsigned char type();
	void addToTileExtra();
	void removeFromTileExtra();
	bool providesResource(char _resourceType);
};

