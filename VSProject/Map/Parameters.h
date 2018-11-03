#pragma once

#define SEED 35
#define FRAME_DURATION 50

//200
#define UNIT_COUNT 100

/*
#define MAP_WIDTH 384 //6*4*4*4
#define MAP_HEIGHT 24 //6*4

#define BASE_CHUNK_SIZE 6
#define SUPER_CHUNK_SIZE 4
*/

#define MAP_WIDTH 448 //6*4*4*4
#define MAP_HEIGHT 28 //6*4

#define BASE_CHUNK_SIZE 7
#define SUPER_CHUNK_SIZE 4

#define LEVELS 4
#define FIRST_FLAT_LEVEL 2

#define CIRCULAR_ARRAY_OF_TILES 10000
#define CIRCULAR_ARRAY_OF_AREAS 10000

#define VIEW_WIDTH 85
#define VIEW_HEIGHT 85

//This should be grater than the slowest object cycle
#define BUCKETS 701

//This variable must be equal or bigger than BASE_CHUNK_SIZE*2
//If the unit exceeds this distance to its destination, it must be in a different area
//#define DUMMY_MOVEMENT_TOWARDS_POINT_DISTANCE 12

#define GENERATORS_COUNT 120
#define	GENERATION_DURATION 700

#define RESOURCE_TYPES 6

//#define CALCULATE_HASH_AT_TIME 300
#define DEBUG true
#define SAFE true //Unneeded assignment of variables to maintain consistency
#define LOG_TIME true
//#define LOG_PF true

#define DEBUG_TIME -1
#define DEBUG_OBJECT -1

#ifdef DEBUG
void debug_checkGlobalVariableCorrectness();
#endif
