#include "Common.h"

unsigned char tilesMap[MAP_WIDTH][MAP_WIDTH][MAP_HEIGHT];

Object* unitsMap[MAP_WIDTH][MAP_WIDTH][MAP_HEIGHT];

std::list<Object*> objects[BUCKETS];

int time = 0;

void fill(int x, int y, int init, int end) {
	for (int z = init; z < end; z++) {
		tilesMap[x][y][z] = tileGround;
	}
}

void map_init()
{
	srand(SEED);
	
	//Clear the map
	for (int i = 0; i < MAP_WIDTH; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			for (int z = 0; z < MAP_HEIGHT; z++) {
				unitsMap[i][j][z] = NULL;
				tilesMap[i][j][z] = tileEmpty;
			}
		}
	}

	createTerrainWithMidPointDisplacement();
	
	int dir = rand() % 400;
	tunnel(MAP_WIDTH / 2, MAP_WIDTH / 2, MAP_HEIGHT / 2, dir, 100, 550);
	tunnel(MAP_WIDTH / 2, MAP_WIDTH / 2, MAP_HEIGHT / 2, (dir + 200) % 400, 100, 550);
	tilesMap[MAP_WIDTH / 2][MAP_WIDTH / 2][MAP_HEIGHT / 2] = tileGround;

	for (int i = 0; i < MAP_WIDTH; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			int init = 0;
			for (int z = 0; z < MAP_HEIGHT; z++) {
				if (tilesMap[i][j][z] == tileGround) {
					if (init == 0) {
						fill(i, j, init, z);
					}
					else {
						fill(i, j, init + (z - init) / 2, z);
					}
					init = z + 3;
				}
			}
		}
	}

	for (int i = 0; i < MAP_WIDTH; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (tilesMap[i][j][0] == tileEmpty) {
				tilesMap[i][j][0] = tileWater;
			}
		}
	}

	pathfinderInitialize();

	for (int i = 0; i < UNIT_COUNT; i++) {
		int x, y, z;
		do {
			x = rand() % MAP_WIDTH;
			y = rand() % MAP_WIDTH;
			z = rand() % MAP_HEIGHT;
		} while (!tileIsSteppable(x, y, z));

		Unit * unit = new Unit(x, y, z, rand() % LIFE + 10);
		for (int i = rand() % 3; i >= 0; i--) {
			unit->modifyCycle(1);
		}
		unit->cycleCurrentStep = 0;
		unit->adjustResourceType();




		if (unit->id == DEBUG_OBJECT) {
			int asd = 0;
			asd++;
		}






		unit->addToTile();
	}

	for (int i = 0; i < GENERATORS_COUNT; i++) {
		int x, y, z;
		do {
			x = rand() % MAP_WIDTH;
			y = rand() % MAP_WIDTH;
			z = rand() % MAP_HEIGHT;
		} while (!tileIsSteppable(x, y, z));
		Generator* generator = new Generator(x, y, z - 1);
		generator->addToTile();
		tilesMap[x][y][z - 1] = tileGenerator;
	}
}

//This is a diagram that shows the order in which the heghit is decided
//0 0 0 0 0 0 0 0 0
//0 2 2 2 2 2 2 2 0
//0 2 1 2 1 2 1 2 0
//0 2 2 2 2 2 2 2 0
//0 2 1 2 0 2 1 2 0
//0 2 2 2 2 2 2 2 0
//0 2 1 2 1 2 1 2 0
//0 2 2 2 2 2 2 2 0
//0 0 0 0 0 0 0 0 0
//
//Note that some points are decided as the average of 2 other points, and some other as the average of 4
//* * * * * * * * *
//* 4 2 4 2 4 2 4 *
//* 2 4 2 2 2 4 2 *
//* 4 2 4 2 4 2 4 *
//* 2 2 2 * 2 2 2 *
//* 4 2 4 2 4 2 4 *
//* 2 4 2 2 2 4 2 *
//* 4 2 4 2 4 2 4 *
//* * * * * * * * *
void createTerrainWithMidPointDisplacement() {
	int size = 1;
	while ((size + 1) * 2 <= MAP_WIDTH) {
		size = size * 2;
	}
	int sizePlus1 = size + 1;



	/*
	size = 128;
	sizePlus1 = 129;*/



	double** height = new double*[sizePlus1];
	for (int i = 0; i < sizePlus1; i++) {
		height[i] = new double[sizePlus1];
	}
	for (int i = 0; i < sizePlus1; i++) {
		for (int j = 0; j < sizePlus1; j++) {
			height[i][j] = -1;
		}
	}
	for (int i = 0; i < sizePlus1; i++) {
		height[i][0] = 0;
		height[i][sizePlus1 - 1] = 0;
		height[0][i] = 0;
		height[sizePlus1 - 1][i] = 0;
	}
	height[size / 2][size / 2] = MAP_HEIGHT - 1;
	int sectionLength = size / 4;
	int maxVariationTimes100 = 100 * MAP_HEIGHT * 3 / 4;
	while (sectionLength > 0) {
		for (int repetition = 0; repetition < 2; repetition++) {
			for (int i = 0; i < size; i += sectionLength) {
				for (int j = 0; j < size; j += sectionLength) {
					if (height[i][j] == -1) {
						int adjacent = 0;
						if (height[i + sectionLength][j] != -1) {
							adjacent++;
						}
						if (height[i][j + sectionLength] != -1) {
							adjacent++;
						}
						if (height[i - sectionLength][j] != -1) {
							adjacent++;
						}
						if (height[i][j - sectionLength] != -1) {
							adjacent++;
						}
						double midPoint = -1;
						if (adjacent == 2) {
							//Note that the 2 adjacents may not be opposites
							//So both if statements can be skipped
							if (height[i - sectionLength][j] != -1 && height[i + sectionLength][j] != -1) {
								midPoint = (height[i - sectionLength][j] + height[i + sectionLength][j]) / 2.0;
							}
							if (height[i][j - sectionLength] != -1 && height[i][j + sectionLength] != -1) {
								midPoint = (height[i][j - sectionLength] + height[i][j + sectionLength]) / 2.0;
							}
						}
						if (adjacent == 4) {
							midPoint = (height[i - sectionLength][j] + height[i + sectionLength][j] + height[i][j - sectionLength] + height[i][j + sectionLength]) / 4;
						}
						if (midPoint != -1) {
							height[i][j] = midPoint + (rand() % (maxVariationTimes100 + 1) - (double)maxVariationTimes100 / 2.0) / 100.0;
							if (height[i][j] >= MAP_HEIGHT) {
								height[i][j] = MAP_HEIGHT - 1;
							}
							if (height[i][j] < 0) {
								height[i][j] = 0;
							}
						}
					}
				}
			}
		}
		maxVariationTimes100 = maxVariationTimes100 / 2;
		sectionLength = sectionLength / 2;
	}

	double scale = (double)MAP_WIDTH / (double)size;
	for (int i = 0; i < MAP_WIDTH; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			int x = (int)(i / scale);
			int y = (int)(j / scale);
			double xDisplacement = i / scale - (double)x;
			double ponderation00 = 1 - xDisplacement;
			double ponderation01 = 1 - xDisplacement;
			double ponderation10 = xDisplacement;
			double ponderation11 = xDisplacement;
			double yDisplacement = j / scale - (double)y;
			ponderation00 = ponderation00 * (1 - yDisplacement);
			ponderation10 = ponderation10 * (1 - yDisplacement);
			ponderation01 = ponderation01 * yDisplacement;
			ponderation11 = ponderation11 * yDisplacement;
			double h00 = height[x][y] * ponderation00;
			double h01 = height[x][y + 1] * ponderation01;
			double h10 = height[x + 1][y] * ponderation10;
			double h11 = height[x + 1][y + 1] * ponderation11;
			int tileHeight = h00 + h01 + h10 + h11;
			fill(i, j, 0, tileHeight);
		}
	}
}