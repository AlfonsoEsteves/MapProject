#include "Common.h"

int viewX = MAP_WIDTH / 2 - VIEW_WIDTH / 2;
int viewY = MAP_WIDTH / 2 - VIEW_WIDTH / 2;
int viewZ = MAP_HEIGHT / 2 - VIEW_HEIGHT / 2;

bool graphics_init();

void graphics_close();

void graphics_draw_minimap();

SDL_Window* window = NULL;

SDL_Renderer* gRenderer = NULL;

SDL_Surface* screenSurface = NULL;

SDL_Texture* blackTopImage = NULL;
SDL_Texture* blackLeftImage = NULL;
SDL_Texture* blackLeftPlusImage = NULL;
SDL_Texture* blackRightImage = NULL;
SDL_Texture* blackRightPlusImage = NULL;
SDL_Texture* groundImage = NULL;
SDL_Texture* groundTopImage = NULL;
SDL_Texture* grassImage = NULL;
SDL_Texture* grassTopImage = NULL;
SDL_Texture* waterTopImage = NULL;
SDL_Texture* generatorImage = NULL;
SDL_Texture* generatorTopImage = NULL;
SDL_Texture* unitImage[RESOURCE_TYPES_IMAGES];
SDL_Texture* resourceImages[RESOURCE_TYPES_IMAGES];

vector<SDL_Texture*> images;

int screenX[VIEW_WIDTH][VIEW_WIDTH];
int screenY[VIEW_WIDTH][VIEW_WIDTH][VIEW_WIDTH];
int screenYfloor[VIEW_WIDTH][VIEW_WIDTH];

SDL_Surface* message = NULL;
TTF_Font* font = NULL;
SDL_Color textColor = { 255, 255, 255 };
/*
SDL_Surface* loadSurface(std::string path)
{
	//The final optimized image
	SDL_Surface* optimizedSurface = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Convert surface to screen format
		optimizedSurface = SDL_ConvertSurface(loadedSurface, screenSurface->format, NULL);
		if (optimizedSurface == NULL)
		{
			printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return optimizedSurface;
}

SDL_Surface * loadSurfaceOld(std::string path) {
	SDL_Surface* optimizedSurface = NULL;
	SDL_Surface* loadedSurface = SDL_LoadBMP(path.c_str());
	if (loadedSurface == NULL) {
		error("Unable to load image");
	}
	else {

		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0xFF, 0, 0xFF));



		optimizedSurface = SDL_ConvertSurface(loadedSurface, screenSurface->format, NULL);
		if (optimizedSurface == NULL) {
			error("Unable to optimize image");
		}
		else {
			SDL_SetColorKey(optimizedSurface, SDL_TRUE, SDL_MapRGB(optimizedSurface->format, 0xFF, 0, 0xFF));
		}
		SDL_FreeSurface(loadedSurface);
	}
	return optimizedSurface;
}*/

SDL_Texture * loadTexture(std::string path) {//The final texture
	/*:SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = loadSurface(path.c_str());
	//Create texture from surface pixels
	newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
	if (newTexture == NULL)
	{
		printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
	}

	//Get rid of old loaded surface
	SDL_FreeSurface(loadedSurface);

	SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND);

	images.push_back(newTexture);

	return newTexture;*/

	SDL_Texture * texture = IMG_LoadTexture(gRenderer, path.c_str());
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	return texture;
}

bool graphics_init() {
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

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		error("SDL_image could not initialize");
		return false;
	}

	screenSurface = SDL_GetWindowSurface(window);
	
	gRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (gRenderer == NULL)
	{
		error("Renderer could not be created");
		return false;
	}

	blackTopImage = loadTexture(RESOURCES_PATH + "Images/black_top.png");
	blackLeftImage = loadTexture(RESOURCES_PATH + "Images/black_left.png");
	blackLeftPlusImage = loadTexture(RESOURCES_PATH + "Images/black_left_plus.png");
	blackRightImage = loadTexture(RESOURCES_PATH + "Images/black_right.png");
	blackRightPlusImage = loadTexture(RESOURCES_PATH + "Images/black_right_plus.png");
	groundImage = loadTexture(RESOURCES_PATH + "Images/ground.png");
	groundTopImage = loadTexture(RESOURCES_PATH + "Images/ground_top.png");
	grassImage = loadTexture(RESOURCES_PATH + "Images/grass.png");
	grassTopImage = loadTexture(RESOURCES_PATH + "Images/grass_top.png");
	waterTopImage = loadTexture(RESOURCES_PATH + "Images/water_top.png");
	generatorImage = loadTexture(RESOURCES_PATH + "Images/generator.png");
	generatorTopImage = loadTexture(RESOURCES_PATH + "Images/generator_top.png");
	unitImage[0] = loadTexture(RESOURCES_PATH + "Images/unitRed.png");
	unitImage[1] = loadTexture(RESOURCES_PATH + "Images/unitYellow.png");
	unitImage[2] = loadTexture(RESOURCES_PATH + "Images/unitGreen.png");
	unitImage[3] = loadTexture(RESOURCES_PATH + "Images/unitSky.png");
	unitImage[4] = loadTexture(RESOURCES_PATH + "Images/unitBlue.png");
	unitImage[5] = loadTexture(RESOURCES_PATH + "Images/unitPurple.png");
	unitImage[6] = loadTexture(RESOURCES_PATH + "Images/unitBlack.png");
	unitImage[7] = loadTexture(RESOURCES_PATH + "Images/unitWhite.png");
	unitImage[8] = loadTexture(RESOURCES_PATH + "Images/unitOrange.png");
	unitImage[9] = loadTexture(RESOURCES_PATH + "Images/unitDirt.png");
	resourceImages[0] = loadTexture(RESOURCES_PATH + "Images/resourceRed.png");
	resourceImages[1] = loadTexture(RESOURCES_PATH + "Images/resourceYellow.png");
	resourceImages[2] = loadTexture(RESOURCES_PATH + "Images/resourceGreen.png");
	resourceImages[3] = loadTexture(RESOURCES_PATH + "Images/resourceSky.png");
	resourceImages[4] = loadTexture(RESOURCES_PATH + "Images/resourceBlue.png");
	resourceImages[5] = loadTexture(RESOURCES_PATH + "Images/resourcePurple.png");
	resourceImages[6] = loadTexture(RESOURCES_PATH + "Images/resourceBlack.png");
	resourceImages[7] = loadTexture(RESOURCES_PATH + "Images/resourceWhite.png");
	resourceImages[8] = loadTexture(RESOURCES_PATH + "Images/resourceOrange.png");
	resourceImages[9] = loadTexture(RESOURCES_PATH + "Images/resourceDirt.png");
	
	font = TTF_OpenFont((RESOURCES_PATH + "OpenSans-Regular.ttf").c_str(), 14);
	
	for (int i = 0; i < VIEW_WIDTH; i++) {
		for (int j = 0; j < VIEW_WIDTH; j++) {
			screenX[i][j] = i * 6 - j * 6 + SCREEN_WIDTH / 2 - 5;
			for (int k = 0; k < VIEW_WIDTH; k++) {
				screenY[i][j][k] = (i - VIEW_WIDTH / 2) * 4 + (j - VIEW_WIDTH / 2) * 4 - 6 * (k - VIEW_HEIGHT / 2) + SCREEN_HEIGHT / 2 - 5;
			}
			screenYfloor[i][j] = (i - VIEW_WIDTH / 2) * 4 + (j - VIEW_WIDTH / 2) * 4 - 6 * (0 - 1 - VIEW_HEIGHT / 2) + SCREEN_HEIGHT / 2 - 5;
		}
	}

	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);

	return true;
}

void graphics_draw_text() {
	/*Object* currentObject = NULL;
	if (selected != NULL) {
		currentObject = unitsMap[selected->x][selected->y][selected->z];
	}
	int xDisplacement = 0;
	while (currentObject != NULL) {
#		ifdef DEBUG
		format("Object's id: %d", currentObject->id);
		message = TTF_RenderText_Solid(font, formated, textColor);
		SDL_Rect position = { xDisplacement + 20, 20, 0, 0 };
		SDL_BlitSurface(message, NULL, screenSurface, &position);
		SDL_FreeSurface(message);

		if (currentObject->objectType == objectUnit) {
			Unit* unit = (Unit*)selected;
			if (unit->parent != NULL && unit->parent->alive) {
				int dist = abs(unit->x - unit->parent->x) + abs(unit->y - unit->parent->y);
				format("Parent distance: %d", dist);
				message = TTF_RenderText_Solid(font, formated, textColor);
				position = { xDisplacement + 20, 60, 0, 0 };
				SDL_BlitSurface(message, NULL, screenSurface, &position);
				SDL_FreeSurface(message);
			}
		}
#		endif
		if (currentObject->type() == objectUnit) {
			Unit* currentUnit = (Unit*)currentObject;
			format("Unit's life: %d", currentUnit->life);
			message = TTF_RenderText_Solid(font, formated, textColor);
			SDL_Rect position = { xDisplacement + 20, 40, 0, 0 };
			SDL_BlitSurface(message, NULL, screenSurface, &position);
			SDL_FreeSurface(message);

			for (int i = 0; i < currentUnit->cycleLength; i++) {
				if (i == currentUnit->cycleCurrentStep) {
					format("%s <<", stepName(currentUnit->cycle[i]).c_str());
				}
				else {
					format("%s", stepName(currentUnit->cycle[i]).c_str());
				}
				message = TTF_RenderText_Solid(font, formated, textColor);
				SDL_Rect position = { xDisplacement + 20, 100 + i * 15, 0, 0 };
				SDL_BlitSurface(message, NULL, screenSurface, &position);
				SDL_FreeSurface(message);
			}

			for (int i = 0; i < currentUnit->bag.size(); i++) {
				format("%s", stepName(currentUnit->bag[i]).c_str());
				message = TTF_RenderText_Solid(font, formated, textColor);
				SDL_Rect position = { xDisplacement + 20, 400 + i * 15, 0, 0 };
				SDL_BlitSurface(message, NULL, screenSurface, &position);
				SDL_FreeSurface(message);
			}
		}
		currentObject = currentObject->sharesTileWithObject;
		xDisplacement += 150;
	}
	format("Time: %d", time);
	message = TTF_RenderText_Solid(font, formated, textColor);
	SDL_Rect position = { SCREEN_WIDTH - 100, SCREEN_HEIGHT - 50, 0, 0 };
	SDL_BlitSurface(message, NULL, screenSurface, &position);
	SDL_FreeSurface(message);
#	ifdef DEBUG
	format("Units: %d", debug_unitCount);
	message = TTF_RenderText_Solid(font, formated, textColor);
	position = { 20, SCREEN_HEIGHT - 50, 0, 0 };
	SDL_BlitSurface(message, NULL, screenSurface, &position);
	SDL_FreeSurface(message);
#	endif*/

	/*if (selected != NULL) {
		if (selected->objectType == objectUnit) {
			Unit* selectedUnit = (Unit*)selected;
			Unit* parent = selectedUnit->parent;
			if (parent != NULL && parent->alive) {
				if (abs(parent->x - selected->x) < SCREEN_WIDTH / 2 && 
					abs(parent->y - selected->y) < SCREEN_WIDTH / 2 && 
					abs(parent->z - selected->z) < SCREEN_HEIGHT / 2) {
					
				}
			}
		}
	}


	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);
	SDL_RenderDrawLine(gRenderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2);*/
}

void graphics_draw_map() {
	SDL_Rect position;
	position.w = 10;
	position.h = 14;
	
	//This draws the floor, which is only tops instead of full blocks
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
							//SDL_BlitSurface(groundTopImage, NULL, screenSurface, &position);
							SDL_RenderCopy(gRenderer, groundTopImage, NULL, &position);
						}
						else {
							//SDL_BlitSurface(grassTopImage, NULL, screenSurface, &position);
							SDL_RenderCopy(gRenderer, grassTopImage, NULL, &position);
						}
					}
					else if (tilesMap[x][y][viewZ - 1] == tileGenerator) {
						//SDL_BlitSurface(generatorTopImage, NULL, screenSurface, &position);
						SDL_RenderCopy(gRenderer, generatorTopImage, NULL, &position);
					}
					else if (tilesMap[x][y][viewZ - 1] == tileWater) {
						//SDL_BlitSurface(waterTopImage, NULL, screenSurface, &position);
						SDL_RenderCopy(gRenderer, waterTopImage, NULL, &position);
					}
				}
				else {
					//SDL_BlitSurface(waterTopImage, NULL, screenSurface, &position);
					SDL_RenderCopy(gRenderer, waterTopImage, NULL, &position);
				}
			}
		}
	}

	//This draws the most of the visible content
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
									//SDL_BlitSurface(unitImage[unit->resourceType], NULL, screenSurface, &position);
									SDL_RenderCopy(gRenderer, unitImage[unit->resourceType], NULL, &position);
									if (unit->calculateWorth() > 100) {
										position.y = screenY[i][j][k] + 1;
										//SDL_BlitSurface(unitImage[unit->resourceType], NULL, screenSurface, &position);
										SDL_RenderCopy(gRenderer, unitImage[unit->resourceType], NULL, &position);
									}
									position.y = screenY[i][j][k];
								}
								else if (current->type() == objectResource) {
									Resource* resource = (Resource*)current;
									//SDL_BlitSurface(resourceImages[resource->resourceType], NULL, screenSurface, &position);
									SDL_RenderCopy(gRenderer, resourceImages[resource->resourceType], NULL, &position);
								}
								current = current->sharesTileWithObject;
							}
						}
						else if (tilesMap[x][y][z] == tileGround) {
							if ((x / BASE_CHUNK_SIZE + y / BASE_CHUNK_SIZE + z / BASE_CHUNK_SIZE) % 2 == 0) {
								//SDL_BlitSurface(groundImage, NULL, screenSurface, &position);
								SDL_RenderCopy(gRenderer, groundImage, NULL, &position);
							}
							else {
								//SDL_BlitSurface(grassImage, NULL, screenSurface, &position);
								SDL_RenderCopy(gRenderer, grassImage, NULL, &position);
							}
						}
						else if (tilesMap[x][y][z] == tileGenerator) {
							//SDL_BlitSurface(generatorImage, NULL, screenSurface, &position);
							SDL_RenderCopy(gRenderer, generatorImage, NULL, &position);
						}
						else if (tilesMap[x][y][z] == tileWater) {
							//SDL_BlitSurface(waterTopImage, NULL, screenSurface, &position);
							SDL_RenderCopy(gRenderer, waterTopImage, NULL, &position);
						}
					}
				}
			}
			else {
				if (viewZ <= 0) {
					position.y = screenY[i][j][0 - viewZ];
					//SDL_BlitSurface(waterTopImage, NULL, screenSurface, &position);
					SDL_RenderCopy(gRenderer, waterTopImage, NULL, &position);
				}
			}
		}
	}

	//This draws the blacked out sides
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
				if (safeTilesMap(leftSideX, leftSideY, z) != tileEmpty) {
					if (i + 1 < VIEW_WIDTH && safeTilesMap(leftSideX + 1, leftSideY, z) != tileEmpty) {
						//SDL_BlitSurface(blackLeftPlusImage, NULL, screenSurface, &positionLeft);
						SDL_RenderCopy(gRenderer, blackLeftPlusImage, NULL, &position);
					}
					else {
						//SDL_BlitSurface(blackLeftImage, NULL, screenSurface, &positionLeft);
						SDL_RenderCopy(gRenderer, blackLeftImage, NULL, &position);
					}
				}
				if (safeTilesMap(rightSideX, rightSideY, z) != tileEmpty) {
					if (i + 1 < VIEW_WIDTH && safeTilesMap(rightSideX, rightSideY + 1, z) != tileEmpty) {
						//SDL_BlitSurface(blackRightPlusImage, NULL, screenSurface, &positionRightPlus);
						SDL_RenderCopy(gRenderer, blackRightPlusImage, NULL, &position);
					}
					else {
						//SDL_BlitSurface(blackRightImage, NULL, screenSurface, &positionRight);
						SDL_RenderCopy(gRenderer, blackRightImage, NULL, &position);
					}
				}
			}
		}
	}

	//This draws the blacked out tops
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
								//SDL_BlitSurface(blackTopImage, NULL, screenSurface, &position);
								SDL_RenderCopy(gRenderer, blackTopImage, NULL, &position);
							}
						}
					}
				}
			}
		}
	}
}

void graphics_draw() {
	//SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0, 0, 0));
	SDL_RenderClear(gRenderer);
	if (selected != NULL) {
		viewX = selected->x - VIEW_WIDTH / 2;
		viewY = selected->y - VIEW_WIDTH / 2;
		viewZ = selected->z - VIEW_HEIGHT / 2;
	}
	graphics_draw_map();
	//graphics_draw_text();
	//graphics_draw_minimap();
	//SDL_UpdateWindowSurface(window);
	SDL_RenderPresent(gRenderer);
}

void graphics_draw_minimap() {
	/*SDL_Rect rect = { SCREEN_WIDTH - MAP_WIDTH / 3, 0, MAP_WIDTH / 3, MAP_WIDTH / 3 };
	SDL_FillRect(screenSurface, &rect, 255);
	SDL_Rect rect2 = { SCREEN_WIDTH - (MAP_WIDTH - viewX) / 3, viewY / 3, VIEW_WIDTH / 3, VIEW_WIDTH / 3 };
	SDL_FillRect(screenSurface, &rect2, 10000);*/
}

void graphics_close()
{
	for (int i = 0; i < images.size(); i++) {
		SDL_DestroyTexture(images[i]);
	}
	SDL_DestroyWindow(window);
	SDL_Quit();
}

string stepName(int step) {
	if (step < RESOURCE_TYPES) {
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
		case 6:
			return "black";
		case 7:
			return "white";
		case 8:
			return "orange";
		case 9:
			return "dirt";
		}
	}
	else {
		switch (step % 2) {
		case INSTRUCTION_NEW_INSTRUCTION:
			return "NEW INSTRUCTION";
		case INSTRUCTION_GIVE_RESOURCE:
			return "SHIFT AND GIVE";
		}
	}
	return NULL;
}

unsigned char safeTilesMap(int x, int y, int z) {
	if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_WIDTH) {
		if (z <= 0) {
			return tileWater;
		}
		else {
			return tileEmpty;
		}
	}
	if (z < 0) {
		return tileGround;
	}
	if (z >= MAP_HEIGHT) {
		return tileEmpty;
	}
	return tilesMap[x][y][z];
}