#include "Common.h"

SDL_Event e;

unsigned int frameDuration = 50;

bool scrolling = false;

void execute_game_frame();

#ifdef LOG_TIME
unsigned int workingTime = 0;
unsigned int drawingTime = 0;
int frameCount = 0;
#endif

int main(int argc, char* args[])
{
	#ifdef DEBUG
	debug_checkGlobalVariableCorrectness();
	#endif

	graphics_init();
	map_init();

	unsigned int oldTime = SDL_GetTicks();
	unsigned int newTime;

#	ifdef FAST_FOWARD
	while (time < FAST_FOWARD) {
		execute_frame();
	}
	newTime = SDL_GetTicks();
	printf("fastfoward time: %d.%d\n", (newTime-oldTime) / 1000, (newTime - oldTime) % 1000);
	oldTime = newTime;
#	endif

	while (!programExecutionQuit) {
		newTime = SDL_GetTicks();
		if (newTime - oldTime > frameDuration) {
			oldTime += frameDuration;
			execute_game_frame();

#			ifdef LOG_TIME
			frameCount++;
			if (frameCount == 50) {
				printf("working time ms: %d   drawing time ms: %d   delay time ms: %d\n", workingTime / 50, drawingTime / 50, newTime - oldTime);
				frameCount = 0;
				workingTime = 0;
				drawingTime = 0;
			}
#			endif

#			ifdef CALCULATE_HASH_AT_TIME
			int hash = 0;
			for (int i = 0; i < BUCKETS; i++) {
				for (std::list<Object*>::iterator it = objects[i].begin(); it != objects[i].end(); it++)
				{
					Object* object = *it;
					hash = (hash + object->x + object->y * 7 + object->z * 49) % 1000;
				}
			}
			printf("%d:%d ", time, hash);
			if (time == CALCULATE_HASH_AT_TIME) {
				pause();
			}
#			endif
		}
	}

	map_close();

	graphics_close();

	return 0;
}

void readInput() {
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	mouseOverObject = NULL;
	double minDist = 30;
	for (int i = 0; i < VIEW_WIDTH; i++) {
		int x = viewX + i;
		for (int j = 0; j < VIEW_WIDTH; j++) {
			int y = viewY + j;
			int sx = screenX[i][j] + 5;
			for (int k = VIEW_WIDTH - 1; k > 0; k--) {
				int z = viewZ + k;
				if (x >= 0 && y >= 0 && z >= 0 && x < MAP_WIDTH && y < MAP_WIDTH && z < MAP_HEIGHT) {
					if (unitsMap[x][y][z] != NULL) {
						int sy = screenY[i][j][k] + 6;
						double dist = sqrt((sx - mx)*(sx - mx) + (sy - my)*(sy - my));
						if (dist < minDist) {
							minDist = dist;
							mouseOverObject = unitsMap[x][y][z];
						}
					}
				}
			}
		}
	}

	const int pixelsPerTileSpeed = 100;
	if (scrolling) {
		int downRight = my - SCREEN_HEIGHT / 2 + mx - SCREEN_WIDTH / 2;
		int downLeft = my - SCREEN_HEIGHT / 2 - mx + SCREEN_WIDTH / 2;
		viewX += downRight / pixelsPerTileSpeed;
		viewY += downLeft / pixelsPerTileSpeed;
	}

	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_QUIT) {
			programExecutionQuit = true;
		}
		else if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym)
			{

			case SDLK_KP_6:
				viewZ++;
				break;

			case SDLK_KP_3:
				viewZ--;
				break;

			case SDLK_KP_PLUS:
				if (frameDuration > 20) {
					frameDuration = frameDuration / 2;
				}
				printf("Frame duration: %d\n", frameDuration);
				break;

			case SDLK_KP_MINUS:
				if (frameDuration < 2000) {
					frameDuration = frameDuration * 2;
				}
				printf("Frame duration: %d\n", frameDuration);
				break;

			case SDLK_ESCAPE:
				programExecutionQuit = true;
				break;
			}
		}
		else if (e.type == SDL_MOUSEBUTTONDOWN) {
			selected = mouseOverObject;
			scrolling = true;
		}
		else if (e.type == SDL_MOUSEBUTTONUP) {
			scrolling = false;
		}
	}
}

void execute_game_frame() {
	unsigned int time1 = SDL_GetTicks();

	readInput();
	execute_frame();

	unsigned int time2 = SDL_GetTicks();
	workingTime += time2 - time1;

#	ifdef DRAW_GRAPHICS_EVERY_X_FRAMES
	if (time % DRAW_GRAPHICS_EVERY_X_FRAMES == 0) {
		graphics_draw();
	}
#	else
	graphics_draw();
#	endif

	drawingTime += SDL_GetTicks() - time2;
}