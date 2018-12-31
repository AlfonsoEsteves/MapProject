#pragma once

#include "Object.h"

class Generator : public Object {
public:
	char resourceGeneration;
	int duration;

	Generator(int _x, int _y, int _z);
	~Generator();
	void execute();
	unsigned char type();
};