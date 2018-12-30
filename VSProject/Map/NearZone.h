#pragma once
#include "Parameters.h"

class NearZone
{
public:
	vector<Unit*> units[RESOURCE_TYPES];

	NearZone();
	~NearZone();

	void removeUnit(Unit* unit);
	void addUnit(Unit* unit);
};

