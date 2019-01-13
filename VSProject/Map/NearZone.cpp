#include "Common.h"

NearZone::NearZone()
{
}

NearZone::~NearZone()
{
}

void NearZone::addUnit(Unit* unit) {
	units.push_back(unit);
}

void NearZone::removeUnit(Unit* unit) {
	vector<Unit*>::iterator it = find(units.begin(), units.end(), unit);
	if (it == units.end()) {
		error("Unit not found");
	}
	else {
		units.erase(it);
	}
}
/*
void NearZone::removeUnit(Unit* unit) {
	vector<Unit*>::iterator it = units.begin();
	while (*it != unit) {
		it++;
#		ifdef DEBUG
		if (it == units.end()) {
			error("Unit not found");
		}
#		endif
	}
	units.erase(it);
}*/