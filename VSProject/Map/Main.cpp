#include "Common.h"

SDL_Event e;

void execute_game_frame();

el siguiente paso seria hacer que se resten la vida cuando una unidad come a otra
  eso va a hacer que las convinaciones buenas duren un poco mas
  y va a permitir que las unidades grandes que son estables sigan variando (ya que podria quedar con poca vida y starvear)
  si la unidad no es capas de matar igual va y se suicida
	de lo contrario o no hace nada o persigue a la misma unidad sin comerla
  si 2 unidades se crusan pero no se intentan comer no pasa nada

int main(int argc, char* args[])
{
	#ifdef DEBUG
	debug_checkGlobalVariableCorrectness();
	#endif

	graphics_init();
	map_init();

	unsigned int oldTime = SDL_GetTicks();
	unsigned int newTime;

#   ifdef LOG_TIME
	unsigned int accumulated = 0;
	int frameCount = 0;
#   endif

	while (!programExecutionQuit) {
		newTime = SDL_GetTicks();
		if (newTime - oldTime>FRAME_DURATION) {
			oldTime += FRAME_DURATION;
			execute_game_frame();

#			ifdef LOG_TIME
			accumulated += SDL_GetTicks() - newTime;
			frameCount++;
			if (frameCount == 50) {
				printf("frame working time ms: %d   delay time ms: %d\n", accumulated / 50, newTime - oldTime);
				frameCount = 0;
				accumulated = 0;
			}
#			endif

#			ifdef CALCULATE_HASH_AT_TIME
			if (time == CALCULATE_HASH_AT_TIME) {
				int hash = 0;
				for (int i = 0; i < BUCKETS; i++) {
					for (std::list<Object*>::iterator it = objects[i].begin(); it != objects[i].end(); it++)
					{
						Object* object = *it;
						hash = (hash + object->x + object->y * 7 + object->z * 49) % 1000;
					}
				}
				printf("HASH: %d\n", hash);
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

	int jump = 3;
	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_QUIT) {
			programExecutionQuit = true;
		}
		else if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym)
			{
			case SDLK_KP_0:
				jump = 7 - jump;
				break;
			case SDLK_KP_2:
				viewX += jump;
				break;

			case SDLK_KP_1:
				viewY += jump;
				break;

			case SDLK_KP_4:
				viewX -= jump;
				break;

			case SDLK_KP_5:
				viewY -= jump;
				break;

			case SDLK_KP_6:
				viewZ++;
				break;

			case SDLK_KP_3:
				viewZ--;
				break;

			case SDLK_ESCAPE:
				programExecutionQuit = true;
				break;
			}
		}
		else if (e.type == SDL_MOUSEBUTTONDOWN) {
			selected = mouseOverObject;
		}
	}
}

void execute_game_frame() {
	readInput();
	execute_frame();
	graphics_draw();
}