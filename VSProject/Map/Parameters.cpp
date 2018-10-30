#include "Common.h"

#ifdef DEBUG

void debug_checkGlobalVariableCorrectness() {
	int w = BASE_CHUNK_SIZE;
	for (int i = 0; i < LEVELS-1; i++) {
		w *= SUPER_CHUNK_SIZE;
	}
	if (w!= MAP_WIDTH) {
		error("Wrong MAP_WIDTH");
	}
	int h = BASE_CHUNK_SIZE;
	for (int i = 0; i < FIRST_FLAT_LEVEL - 1; i++) {
		h *= SUPER_CHUNK_SIZE;
	}
	if (h != MAP_HEIGHT) {
		error("Wrong MAP_HEIGHT");
	}
	/*if (DUMMY_MOVEMENT_TOWARDS_POINT_DISTANCE < BASE_CHUNK_SIZE * 2) {
		error("DUMMY_MOVEMENT_TOWARDS_POINT_DISTANCE is too small");
	}*/
	if (BUCKETS <= GENERATION_DURATION) {
		error("The amount of buckets should be grater than the slowest object cycle");
	}
	if (RESOURCE_TYPES != OPEN_BRACKET || OPEN_BRACKET + 1 != CLOSE_BRACKET) {
		error("Wrong OPEN_BRACKET and CLOSE_BRACKET values");
	}
}

#endif

