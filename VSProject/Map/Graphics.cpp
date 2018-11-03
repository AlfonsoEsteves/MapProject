#include "Common.h"

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 750;

int viewX = MAP_WIDTH / 2 - VIEW_WIDTH / 2;
int viewY = MAP_WIDTH / 2 - VIEW_WIDTH / 2;
int viewZ = MAP_HEIGHT / 2 - VIEW_HEIGHT / 2;

bool graphics_init();

void graphics_close();

SDL_Window* window = NULL;

SDL_Surface* screenSurface = NULL;

SDL_Surface* blackTopImage = NULL;
SDL_Surface* blackLeftImage = NULL;
SDL_Surface* blackLeftPlusImage = NULL;
SDL_Surface* blackRightImage = NULL;
SDL_Surface* blackRightPlusImage = NULL;
SDL_Surface* groundImage = NULL;
SDL_Surface* groundTopImage = NULL;
SDL_Surface* grassImage = NULL;
SDL_Surface* grassTopImage = NULL;
SDL_Surface* waterTopImage = NULL;
SDL_Surface* generatorImage = NULL;
SDL_Surface* generatorTopImage = NULL;
SDL_Surface* unitImage[6];
SDL_Surface* resourceImages[RESOURCE_TYPES];

int screenX[VIEW_WIDTH][VIEW_WIDTH];
int screenY[VIEW_WIDTH][VIEW_WIDTH][VIEW_WIDTH];
int screenYfloor[VIEW_WIDTH][VIEW_WIDTH];

SDL_Surface* message = NULL;
TTF_Font* font = NULL;
SDL_Color textColor = { 255, 255, 255 };

SDL_Surface* loadSurface(std::string path)
{
	SDL_Surface* optimizedSurface = NULL;
	SDL_Surface* loadedSurface = SDL_LoadBMP(path.c_str());
	if (loadedSurface == NULL)
	{
		error("Unable to load image");
	}
	else
	{
		optimizedSurface = SDL_ConvertSurface(loadedSurface, screenSurface->format, NULL);
		if (optimizedSurface == NULL)
		{
			error("Unable to optimize image");
		}
		else
		{
			SDL_SetColorKey(optimizedSurface, SDL_TRUE, SDL_MapRGB(optimizedSurface->format, 0xFF, 0, 0xFF));
		}
		SDL_FreeSurface(loadedSurface);
	}
	return optimizedSurface;
}

bool graphics_init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		error("SDL could not initialize");
		return false;
	}
	if (TTF_Init() == -1) {
		error("SDL_ttf could not initialize");
		return false;
	}
	window = SDL_CreateWindow("MAP", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		error("Window could not be created");
		return false;
	}

	screenSurface = SDL_GetWindowSurface(window);

	string path = "C:/Users/Usuario/Documents/Projects/MapProject/Resources/";
	//string path = "C:/Users/alfonso.esteves/Documents/Otros/MapProject/Resources/";

	blackTopImage = loadSurface(path + "Images/black_top.bmp");
	blackLeftImage = loadSurface(path + "Images/black_left.bmp");
	blackLeftPlusImage = loadSurface(path + "Images/black_left_plus.bmp");
	blackRightImage = loadSurface(path + "Images/black_right.bmp");
	blackRightPlusImage = loadSurface(path + "Images/black_right_plus.bmp");
	groundImage = loadSurface(path + "Images/ground.bmp");
	groundTopImage = loadSurface(path + "Images/ground_top.bmp");
	grassImage = loadSurface(path + "Images/grass.bmp");
	grassTopImage = loadSurface(path + "Images/grass_top.bmp");
	waterTopImage = loadSurface(path + "Images/water_top.bmp");
	generatorImage = loadSurface(path + "Images/generator.bmp");
	generatorTopImage = loadSurface(path + "Images/generator_top.bmp");
	unitImage[0] = loadSurface(path + "Images/unitRed.bmp");
	unitImage[1] = loadSurface(path + "Images/unitYellow.bmp");
	unitImage[2] = loadSurface(path + "Images/unitGreen.bmp");
	unitImage[3] = loadSurface(path + "Images/unitSky.bmp");
	unitImage[4] = loadSurface(path + "Images/unitBlue.bmp");
	unitImage[5] = loadSurface(path + "Images/unitPurple.bmp");
	resourceImages[0] = loadSurface(path + "Images/resourceRed.bmp");
	resourceImages[1] = loadSurface(path + "Images/resourceYellow.bmp");
	resourceImages[2] = loadSurface(path + "Images/resourceGreen.bmp");
	resourceImages[3] = loadSurface(path + "Images/resourceSky.bmp");
	resourceImages[4] = loadSurface(path + "Images/resourceBlue.bmp");
	resourceImages[5] = loadSurface(path + "Images/resourcePurple.bmp");
	
	font = TTF_OpenFont((path + "OpenSans-Regular.ttf").c_str(), 14);
	
	for (int i = 0; i < VIEW_WIDTH; i++) {
		for (int j = 0; j < VIEW_WIDTH; j++) {
			screenX[i][j] = i * 6 - j * 6 + SCREEN_WIDTH / 2 - 5;
			for (int k = 0; k < VIEW_WIDTH; k++) {
				screenY[i][j][k] = (i - VIEW_WIDTH / 2) * 4 + (j - VIEW_WIDTH / 2) * 4 - 6 * (k - VIEW_WIDTH / 2) + SCREEN_HEIGHT / 2 - 5;
			}
			screenYfloor[i][j] = (i - VIEW_WIDTH / 2) * 4 + (j - VIEW_WIDTH / 2) * 4 - 6 * (0 - 1 - VIEW_WIDTH / 2) + SCREEN_HEIGHT / 2 - 5;
		}
	}

	return true;
}

void graphics_draw_text() {
	if (selected != NULL) {
#		ifdef DEBUG
		format("Object's id: %d", selected->id);
		message = TTF_RenderText_Solid(font, formated, textColor);
		SDL_Rect position = { 20, 20, 0, 0 };
		SDL_BlitSurface(message, NULL, screenSurface, &position);
		SDL_FreeSurface(message);
#		endif
		if (selected->type() == objectUnit) {
			Unit* selectedUnit = (Unit*)selected;
			format("Unit's life: %d", selectedUnit->life);
			message = TTF_RenderText_Solid(font, formated, textColor);
			SDL_Rect position = { 20, 40, 0, 0 };
			SDL_BlitSurface(message, NULL, screenSurface, &position);
			SDL_FreeSurface(message);

			if (selected->sharesTileWithObject != NULL) {
				format("More than 1 object in this time", selectedUnit->life);
				message = TTF_RenderText_Solid(font, formated, textColor);
				position = { 20, 60, 0, 0 };
				SDL_BlitSurface(message, NULL, screenSurface, &position);
				SDL_FreeSurface(message);
			}

			for (int i = 0; i < selectedUnit->cycleLength; i++) {
				if (i == selectedUnit->cycleCurrentStep) {
					format("%s <<", stepName(selectedUnit->cycle[i]).c_str());
				}
				else {
					format("%s", stepName(selectedUnit->cycle[i]).c_str());
				}
				message = TTF_RenderText_Solid(font, formated, textColor);
				SDL_Rect position = { 20, 80 + i * 15, 0, 0 };
				SDL_BlitSurface(message, NULL, screenSurface, &position);
				SDL_FreeSurface(message);
			}

			for (int i = 0; i < selectedUnit->bag.size(); i++) {
				format("%s <<", stepName(selectedUnit->bag[i]).c_str());
				message = TTF_RenderText_Solid(font, formated, textColor);
				SDL_Rect position = { 20, 400 + i * 15, 0, 0 };
				SDL_BlitSurface(message, NULL, screenSurface, &position);
				SDL_FreeSurface(message);
			}
		}
	}
#	ifdef DEBUG
	format("Units: %d", debug_unitCount);
	message = TTF_RenderText_Solid(font, formated, textColor);
	SDL_Rect position = { 20, 670, 0, 0 };
	SDL_BlitSurface(message, NULL, screenSurface, &position);
	SDL_FreeSurface(message);
#	endif
}

void graphics_draw_map() {
	SDL_Rect position;
	if (viewZ > 0) {
		for (int i = 0; i < VIEW_WIDTH; i++) {
			int x = viewX + i;
			for (int j = 0; j < VIEW_WIDTH; j++) {
				position.x = screenX[i][j];
				position.y = screenYfloor[i][j];
				int y = viewY + j;
				if (x >= 0 && y >= 0 && x < MAP_WIDTH && y < MAP_WIDTH) {
					if (tilesMap[x][y][viewZ - 1] == tileGround) {
						if ((x / BASE_CHUNK_SIZE + y / BASE_CHUNK_SIZE + (viewZ - 1) / BASE_CHUNK_SIZE) % 2 == 0) {
							SDL_BlitSurface(groundTopImage, NULL, screenSurface, &position);
						}
						else {
							SDL_BlitSurface(grassTopImage, NULL, screenSurface, &position);
						}
					}
					else if (tilesMap[x][y][viewZ - 1] == tileGenerator) {
						SDL_BlitSurface(generatorTopImage, NULL, screenSurface, &position);
					}
					else if (tilesMap[x][y][viewZ - 1] == tileWater) {
						SDL_BlitSurface(waterTopImage, NULL, screenSurface, &position);
					}
				}
				else {
					SDL_BlitSurface(waterTopImage, NULL, screenSurface, &position);
				}
			}
		}
	}
	for (int i = 0; i < VIEW_WIDTH; i++) {
		int x = viewX + i;
		for (int j = 0; j < VIEW_WIDTH; j++) {
			int y = viewY + j;
			position.x = screenX[i][j];
			if (x >= 0 && y >= 0 && x < MAP_WIDTH && y < MAP_WIDTH) {
				for (int k = 0; k < VIEW_HEIGHT; k++) {
					int z = viewZ + k;
					if (z >= 0 && z < MAP_HEIGHT) {
						position.y = screenY[i][j][k];
						if (tilesMap[x][y][z] == tileEmpty) {
							Object* current = unitsMap[x][y][z];
							while (current != NULL) {
								if (current->type() == objectUnit) {
									Unit* unit = (Unit*)current;
									position.y = screenY[i][j][k] + 4;
									SDL_BlitSurface(unitImage[unit->resourceType], NULL, screenSurface, &position);
									if (unit->hasBrackets()) {
										position.y = screenY[i][j][k] + 1;
										SDL_BlitSurface(unitImage[unit->resourceType], NULL, screenSurface, &position);
									}
									position.y = screenY[i][j][k];
								}
								else if (current->type() == objectResource) {
									Resource* resource = (Resource*)current;
									SDL_BlitSurface(resourceImages[resource->resourceType], NULL, screenSurface, &position);
								}
								current = current->sharesTileWithObject;
							}
						}
						else if (tilesMap[x][y][z] == tileGround) {
							if ((x / BASE_CHUNK_SIZE + y / BASE_CHUNK_SIZE + z / BASE_CHUNK_SIZE) % 2 == 0) {
								SDL_BlitSurface(groundImage, NULL, screenSurface, &position);
							}
							else {
								SDL_BlitSurface(grassImage, NULL, screenSurface, &position);
							}
						}
						else if (tilesMap[x][y][z] == tileGenerator) {
							SDL_BlitSurface(generatorImage, NULL, screenSurface, &position);
						}
						else if (tilesMap[x][y][z] == tileWater) {
							SDL_BlitSurface(waterTopImage, NULL, screenSurface, &position);
						}
					}
				}
			}
			else {
				if (viewZ <= 0) {
					position.y = screenY[i][j][0 - viewZ];
					SDL_BlitSurface(waterTopImage, NULL, screenSurface, &position);
				}
			}
		}
	}
	SDL_Rect positionLeft;
	SDL_Rect positionRight;
	SDL_Rect positionRightPlus;
	int leftSideY = viewY + VIEW_WIDTH - 1;
	int rightSideX = viewX + VIEW_WIDTH - 1;
	for (int i = 0; i < VIEW_WIDTH; i++) {
		int leftSideX = i + viewX;
		int rightSideY = i + viewY;
		positionLeft.x = screenX[i][VIEW_WIDTH - 1];
		positionRight.x = screenX[VIEW_WIDTH - 1][i] + 5;
		positionRightPlus.x = positionRight.x - 1;
		for (int k = 0; k < VIEW_HEIGHT; k++) {
			int z = viewZ + k;
			if (z < MAP_HEIGHT) {
				positionLeft.y = screenY[i][VIEW_WIDTH - 1][k] + 5;
				positionRight.y = screenY[VIEW_WIDTH - 1][i][k] + 5;
				positionRightPlus.y = positionRight.y;
				if (leftSideX >= 0 && leftSideY >= 0 && leftSideX < MAP_WIDTH && leftSideY < MAP_WIDTH) {
					if (z < 0) {
						SDL_BlitSurface(blackLeftPlusImage, NULL, screenSurface, &positionLeft);
					}
					else {
						if (tilesMap[leftSideX][leftSideY][z] != tileEmpty) {
							if (leftSideX + 1 < MAP_WIDTH && tilesMap[leftSideX + 1][leftSideY][z] != tileEmpty) {
								SDL_BlitSurface(blackLeftPlusImage, NULL, screenSurface, &positionLeft);
							}
							else {
								SDL_BlitSurface(blackLeftImage, NULL, screenSurface, &positionLeft);
							}
						}
					}
				}
				if (rightSideX >= 0 && rightSideY >= 0 && rightSideX < MAP_WIDTH && rightSideY < MAP_WIDTH) {
					if (z < 0) {
						SDL_BlitSurface(blackRightPlusImage, NULL, screenSurface, &positionRightPlus);
					}
					else {
						if (tilesMap[rightSideX][rightSideY][z] != tileEmpty) {
							if (rightSideY + 1 < MAP_WIDTH && tilesMap[rightSideX][rightSideY + 1][z] != tileEmpty) {
								SDL_BlitSurface(blackRightPlusImage, NULL, screenSurface, &positionRightPlus);
							}
							else {
								SDL_BlitSurface(blackRightImage, NULL, screenSurface, &positionRight);
							}
						}
					}
				}
			}
		}
	}
	if (viewZ + VIEW_HEIGHT < MAP_HEIGHT) {
		for (int i = 0; i < VIEW_WIDTH; i++) {
			int x = viewX + i;
			if (x >= 0 && x < MAP_WIDTH) {
				for (int j = 0; j < VIEW_WIDTH; j++) {
					int y = viewY + j;
					if (y >= 0 && y < MAP_WIDTH) {
						int z = viewZ + VIEW_HEIGHT - 1;
						if (z < MAP_HEIGHT) {
							if (tilesMap[x][y][z] != tileEmpty) {
								position.x = screenX[i][j];
								position.y = screenY[i][j][VIEW_HEIGHT - 1];
								SDL_BlitSurface(blackTopImage, NULL, screenSurface, &position);
							}
						}
					}
				}
			}
		}
	}
}

void graphics_draw() {
	SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0, 0, 0));
	if (selected != NULL) {
		viewX = selected->x - VIEW_WIDTH / 2;
		viewY = selected->y - VIEW_WIDTH / 2;
		viewZ = selected->z - VIEW_HEIGHT / 2;
	}
	graphics_draw_map();
	graphics_draw_text();
	SDL_UpdateWindowSurface(window);
}

void graphics_close()
{
	SDL_FreeSurface(blackTopImage);
	SDL_FreeSurface(blackLeftImage);
	SDL_FreeSurface(blackLeftPlusImage);
	SDL_FreeSurface(blackRightImage);
	SDL_FreeSurface(blackRightPlusImage);
	SDL_FreeSurface(groundImage);
	SDL_FreeSurface(groundTopImage);
	SDL_FreeSurface(grassImage);
	SDL_FreeSurface(grassTopImage);
	SDL_FreeSurface(waterTopImage);
	SDL_FreeSurface(generatorImage);
	SDL_FreeSurface(generatorTopImage);
	for (int i = 0; i < RESOURCE_TYPES; i++) {
		SDL_FreeSurface(resourceImages[i]);
	}
	SDL_DestroyWindow(window);
	SDL_Quit();
}

string stepName(int step) {
	switch (step) {
	case 0:
		return "red";
	case 1:
		return "yellow";
	case 2:
		return "green";
	case 3:
		return "sky";
	case 4:
		return "blue";
	case 5:
		return "purple";
	case OPEN_BRACKET:
		return "{";
	case CLOSE_BRACKET:
		return "}";
	}
	return NULL;
}