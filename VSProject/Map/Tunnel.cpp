#include "Common.h"

void tunnel(int x, int y, int z, int horizontalDir, int verticalDir, int life)
{
	bool movedVertically = false;
	while (life>0) {
		life--;
		if (rand() % 2) {
			verticalDir += (200 - verticalDir) / 5;
		}
		else {
			verticalDir -= verticalDir / 5;
		}
		verticalDir = verticalDir % 200;
		if (z > MAP_HEIGHT * 3 / 4 && verticalDir > 150) {
			verticalDir = (verticalDir + 100) / 2;
		}
		if (z < MAP_HEIGHT / 8 && verticalDir < 25) {
			verticalDir = (verticalDir + 100) / 2;
		}
		int vDir = verticalDir / 100;
		if (rand() % 100 < verticalDir % 100) {
			vDir++;
		}
		z += vDir - 1;
		if (rand() % 2) {
			horizontalDir += 10;
		}
		else {
			horizontalDir += 400 - 10;
		}
		horizontalDir = horizontalDir % 400;
		int hDir = horizontalDir / 100;
		if (rand() % 100 < horizontalDir % 100) {
			hDir++;
			hDir = hDir % 4;
		}
		x += getX(hDir);
		y += getY(hDir);
		if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_WIDTH && z>=0 && z<MAP_HEIGHT - 1) {
			tilesMap[x][y][z] = tileGround;
			if (rand() % 200 == 0) {
				tunnel(x, y, z, (horizontalDir + (rand() % 100) + 50) % 400, verticalDir, life);
			}
			if (rand() % 200 == 0) {
				tunnel(x, y, z, (horizontalDir + 400 - (rand() % 100) - 50) % 400, verticalDir, life);
			}
		}
		else {
			return;
		}
	}
	int squareRadius = 2 + rand() % 3;
	for (int i = -squareRadius; i <= squareRadius; i++) {
		for (int j = -squareRadius; j <= squareRadius; j++) {
			int x2 = x + i;
			int y2 = y + j;
			if (x2 >= 0 && x2 < MAP_WIDTH && y2 >= 0 && y2 < MAP_WIDTH) {
				if (!tileIsSteppable(x2, y2, z) && (z==0 || !tileIsSteppable(x2, y2, z-1))) {
					tilesMap[x2][y2][z] = tileGround;
				}
			}
		}
	}
}