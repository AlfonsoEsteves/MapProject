#include "Common.h"

NearZone::NearZone()
{
}

NearZone::~NearZone()
{
}

void NearZone::addUnit(Unit* unit) {
	units[unit->resourceType].push_back(unit);
}

void NearZone::removeUnit(Unit* unit) {
	vector<Unit*>::iterator it = units[unit->resourceType].begin();
	while (*it != unit) {
		it++;
#		ifdef DEBUG
		if (it == units[unit->resourceType].end()) {
			error("Unit not found");
		}
#		endif
	}
	units[unit->resourceType].erase(it);
}