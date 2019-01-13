#pragma once

#define SEED 143

#define UNIT_COUNT 100

/*#define MAP_WIDTH 875 //7*5*5*5
#define MAP_HEIGHT 35 //7*5

#define BASE_CHUNK_SIZE 7
#define SUPER_CHUNK_SIZE 5*/

#define MAP_WIDTH 750 //6*5*5*5
#define MAP_HEIGHT 30 //6*5

#define BASE_CHUNK_SIZE 6
#define SUPER_CHUNK_SIZE 5

/*#define MAP_WIDTH 448 //7*4*4*4
#define MAP_HEIGHT 28 //7*4

#define BASE_CHUNK_SIZE 7
#define SUPER_CHUNK_SIZE 4*/

/*#define MAP_WIDTH 384 //6*4*4*4
#define MAP_HEIGHT 24 //6*4

#define BASE_CHUNK_SIZE 6
#define SUPER_CHUNK_SIZE 4*/

#define NEAR_ZONE_DISTANCE (BASE_CHUNK_SIZE * SUPER_CHUNK_SIZE)

#define LEVELS 4
#define FIRST_FLAT_LEVEL 2

#define CIRCULAR_ARRAY_OF_TILES 10000
#define CIRCULAR_ARRAY_OF_AREAS 10000

//This should be grater than the slowest object cycle
#define BUCKETS 801

//This variable must be equal or bigger than BASE_CHUNK_SIZE*2
//If the unit exceeds this distance to its destination, it must be in a different area
//#define DUMMY_MOVEMENT_TOWARDS_POINT_DISTANCE 12

#define GENERATORS_COUNT (MAP_WIDTH * MAP_WIDTH / 1000)
#define	GENERATION_DURATION 800

#define RESOURCE_TYPES 5
#define RESOURCE_CATEGORIES 3
#define NO_RESOURCE (RESOURCE_TYPES * RESOURCE_CATEGORIES)

#define FAST_FOWARD 4000
//#define CALCULATE_HASH_AT_TIME 5001
#define DEBUG true
#define SAFE true //Unneeded assignment of variables to maintain consistency
#define LOG_TIME true
//#define LOG_PF true

#define DEBUG_TIME 44
#define DEBUG_OBJECT 1454

#ifdef DEBUG
void debug_checkGlobalVariableCorrectness();
#endif
