#include "Common.h"

struct Image {
	SDL_Texture* t;
	SDL_Rect position;
	int wCenter;
	int hCenter;

	Image(SDL_Texture* _t, int _w, int _h, int _wCenter, int _hCenter) {
		t = _t;
		position = { 0, 0, _w, _h };
		wCenter = _wCenter;
		hCenter = _hCenter;
	}
};

int viewX = MAP_WIDTH / 2 - VIEW_WIDTH / 2;
int viewY = MAP_WIDTH / 2 - VIEW_WIDTH / 2;
int viewZ = MAP_HEIGHT / 2 - VIEW_HEIGHT / 2;

bool graphics_init();

void graphics_close();

void graphics_draw_minimap();

SDL_Window* window = NULL;

SDL_Renderer* gRenderer = NULL;

SDL_Surface* screenSurface = NULL;

Image* blackTopImage = NULL;
Image* blackLeftImage = NULL;
Image* blackLeftPlusImage = NULL;
Image* blackRightImage = NULL;
Image* blackRightPlusImage = NULL;
Image* groundImage = NULL;
Image* groundTopImage = NULL;
Image* grassImage = NULL;
Image* grassTopImage = NULL;
Image* waterTopImage = NULL;
Image* generatorImage = NULL;
Image* generatorTopImage = NULL;
Image* unitImage[RESOURCE_TYPES_IMAGES];
Image* resourceImages[RESOURCE_TYPES_IMAGES];

vector<SDL_Texture*> images;

int screenX[VIEW_WIDTH][VIEW_WIDTH];
int screenY[VIEW_WIDTH][VIEW_WIDTH][VIEW_HEIGHT];
int screenYfloor[VIEW_WIDTH][VIEW_WIDTH];

TTF_Font* font = NULL;
SDL_Color textColor = { 255, 255, 255 };

Image* loadTexture(std::string path, int wCenter, int hCenter) {
	SDL_Texture * texture = IMG_LoadTexture(gRenderer, path.c_str());

	int w, h;
	SDL_QueryTexture(texture, NULL, NULL, &w, &h);
	if (wCenter == -99) {
		wCenter = w / 2;
	}
	if (hCenter == -99) {
		hCenter = h / 2;
	}
	Image* image = new Image(texture, w, h, wCenter, hCenter);
	return image;
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
	if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
		error("SDL_image could not initialize");
		return false;
	}
	window = SDL_CreateWindow("MAP", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		error("Window could not be created");
		return false;
	}

	screenSurface = SDL_GetWindowSurface(window);

	//I am having better results with SDL_RENDERER_SOFTWARE than SDL_RENDERER_ACCELERATED
	gRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

	if (gRenderer == NULL)
	{
		error("Renderer could not be created");
		return false;
	}

	const int imageHeightTerrain = 4;
	blackTopImage = loadTexture(RESOURCES_PATH + "Images/black_top.png", -99, -99);
	blackLeftImage = loadTexture(RESOURCES_PATH + "Images/black_left.png", 5, -1);
	blackLeftPlusImage = loadTexture(RESOURCES_PATH + "Images/black_left_plus.png", 5, -1);
	blackRightImage = loadTexture(RESOURCES_PATH + "Images/black_right.png", 0, -1);
	blackRightPlusImage = loadTexture(RESOURCES_PATH + "Images/black_right_plus.png", 1, -1);
	groundImage = loadTexture(RESOURCES_PATH + "Images/ground.png", -99, imageHeightTerrain);
	groundTopImage = loadTexture(RESOURCES_PATH + "Images/ground_top.png", -99, -99);
	grassImage = loadTexture(RESOURCES_PATH + "Images/grass.png", -99, imageHeightTerrain);
	grassTopImage = loadTexture(RESOURCES_PATH + "Images/grass_top.png", -99, -99);
	waterTopImage = loadTexture(RESOURCES_PATH + "Images/water_top.png", -99, -99);
	generatorImage = loadTexture(RESOURCES_PATH + "Images/generator.png", -99, imageHeightTerrain);
	generatorTopImage = loadTexture(RESOURCES_PATH + "Images/generator_top.png", -99, -99);
	unitImage[0] = loadTexture(RESOURCES_PATH + "Images/unitRed.png", -99, -99);
	unitImage[1] = loadTexture(RESOURCES_PATH + "Images/unitYellow.png", -99, -99);
	unitImage[2] = loadTexture(RESOURCES_PATH + "Images/unitGreen.png", -99, -99);
	unitImage[3] = loadTexture(RESOURCES_PATH + "Images/unitSky.png", -99, -99);
	unitImage[4] = loadTexture(RESOURCES_PATH + "Images/unitBlue.png", -99, -99);
	unitImage[5] = loadTexture(RESOURCES_PATH + "Images/unitPurple.png", -99, -99);
	unitImage[6] = loadTexture(RESOURCES_PATH + "Images/unitBlack.png", -99, -99);
	unitImage[7] = loadTexture(RESOURCES_PATH + "Images/unitWhite.png", -99, -99);
	unitImage[8] = loadTexture(RESOURCES_PATH + "Images/unitOrange.png", -99, -99);
	unitImage[9] = loadTexture(RESOURCES_PATH + "Images/unitDirt.png", -99, -99);
	const int imageHightResource = 1;
	resourceImages[0] = loadTexture(RESOURCES_PATH + "Images/resourceRed.png", -99, imageHightResource);
	resourceImages[1] = loadTexture(RESOURCES_PATH + "Images/resourceYellow.png", -99, imageHightResource);
	resourceImages[2] = loadTexture(RESOURCES_PATH + "Images/resourceGreen.png", -99, imageHightResource);
	resourceImages[3] = loadTexture(RESOURCES_PATH + "Images/resourceSky.png", -99, imageHightResource);
	resourceImages[4] = loadTexture(RESOURCES_PATH + "Images/resourceBlue.png", -99, imageHightResource);
	resourceImages[5] = loadTexture(RESOURCES_PATH + "Images/resourcePurple.png", -99, imageHightResource);
	resourceImages[6] = loadTexture(RESOURCES_PATH + "Images/resourceBlack.png", -99, imageHightResource);
	resourceImages[7] = loadTexture(RESOURCES_PATH + "Images/resourceWhite.png", -99, imageHightResource);
	resourceImages[8] = loadTexture(RESOURCES_PATH + "Images/resourceOrange.png", -99, imageHightResource);
	resourceImages[9] = loadTexture(RESOURCES_PATH + "Images/resourceDirt.png", -99, imageHightResource);

	font = TTF_OpenFont((RESOURCES_PATH + "OpenSans-Regular.ttf").c_str(), 14);

	for (int i = 0; i < VIEW_WIDTH; i++) {
		for (int j = 0; j < VIEW_WIDTH; j++) {
			screenX[i][j] = i * 6 - j * 6 + SCREEN_WIDTH / 2;
			for (int k = 0; k < VIEW_HEIGHT; k++) {
				screenY[i][j][k] = (i - VIEW_WIDTH / 2) * 4 + (j - VIEW_WIDTH / 2) * 4 - 6 * (k - VIEW_HEIGHT / 2) + SCREEN_HEIGHT / 2 - 5;
			}
			screenYfloor[i][j] = (i - VIEW_WIDTH / 2) * 4 + (j - VIEW_WIDTH / 2) * 4 - 6 * (0 - 1 - VIEW_HEIGHT / 2) + SCREEN_HEIGHT / 2 - 5;
		}
	}

	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);

	return true;
}

void drawText(int x, int y, const char *text, ...) {
	va_list args;
	va_start(args, text);
	vsnprintf(formated, sizeof formated, text, args);
	va_end(args);

	SDL_Surface* messageSurface = TTF_RenderText_Solid(font, formated, textColor);
	SDL_Texture* messageTexture = SDL_CreateTextureFromSurface(gRenderer, messageSurface);

	SDL_Rect position = { x, y, messageSurface->w, messageSurface->h };
	SDL_RenderCopy(gRenderer, messageTexture, NULL, &position);

	SDL_FreeSurface(messageSurface);
	SDL_DestroyTexture(messageTexture);
}

void graphics_draw_text() {
	Object* currentObject = NULL;
	if (selected != NULL) {
		currentObject = unitsMap[selected->x][selected->y][selected->z];
	}
	int xDisplacement = 0;
	while (currentObject != NULL) {
#		ifdef DEBUG
		drawText(xDisplacement + 20, 20, "Object's id: %d", currentObject->id);
#		endif

		if (currentObject->type() == objectUnit) {
			Unit* currentUnit = (Unit*)selected;

			drawText(xDisplacement + 20, 40, "Unit's life: %d", currentUnit->life);


			if (currentUnit->parent != NULL && currentUnit->parent->alive) {
				int distParent = dist(currentUnit, currentUnit->parent);
				drawText(xDisplacement + 20, 60, "Parent distance: %d", distParent);
				int distRegion = dist(currentUnit, currentUnit->parent->regionCenter);
				drawText(xDisplacement + 20, 80, "Parent region distance: %d", distRegion);
			}
			else {
				int distRegion = dist(currentUnit, currentUnit->regionCenter);
				drawText(xDisplacement + 20, 80, "Own region distance: %d", distRegion);
			}

			drawText(xDisplacement + 20, 100, "Search timeout: %d", currentUnit->searchTime);

			for (int i = 0; i < RESOURCE_CATEGORIES; i++) {
				drawText(xDisplacement + 20, 130 + i * 15, "%s", stepName(currentUnit->desiredResources[i] + RESOURCE_TYPES * i).c_str());
			}

			if (currentUnit->consuming) {
				drawText(xDisplacement + 20, 370, "consume");
			}
			else {
				drawText(xDisplacement + 20, 370, "store");
			}

			if(currentUnit->carryingResource != NO_RESOURCE) {
				drawText(xDisplacement + 20, 390, "carrying %s", stepName(currentUnit->carryingResource).c_str());
			}

			if (currentUnit->searchingResource1 != NO_RESOURCE) {
				drawText(xDisplacement + 20, 410, "searching1 %s", stepName(currentUnit->searchingResource1).c_str());
			}

			if (currentUnit->searchingResource2 != NO_RESOURCE) {
				drawText(xDisplacement + 20, 430, "searching2 %s", stepName(currentUnit->searchingResource2).c_str());
			}

			if (currentUnit->storingResource != NO_RESOURCE) {
				drawText(xDisplacement + 20, 450, "storing %s", stepName(currentUnit->storingResource).c_str());
			}
		}
		currentObject = currentObject->sharesTileWithObject;
		xDisplacement += 150;
	}
	drawText(SCREEN_WIDTH - 100, SCREEN_HEIGHT - 50, "Time: %d", time);
#	ifdef DEBUG
	drawText(20, SCREEN_HEIGHT - 50, "Units: %d", debug_unitCount);
#	endif

	/*
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);
	SDL_RenderDrawLine(gRenderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2);*/
}

void drawImage(Image* image, int x, int y) {
	image->position.x = x - image->wCenter;
	image->position.y = y - image->hCenter;
	SDL_RenderCopy(gRenderer, image->t, NULL, &(image->position));
}

void drawUnit(Unit* unit, int i, int j, int k) {
	int sX = screenX[i][j];
	int sY = screenY[i][j][k] + 4;
	Image* image;
	if (unit->isOrphan()) {
		image = unitImage[7];
	}
	else {
		if (unit->parent == NULL) {
			image = unitImage[0];
		}
		else {
			image = unitImage[4];
		}
	}
	drawImage(image, sX, sY);
	if (unit->childs > 0) {
		drawImage(image, sX, sY - 2);
	}

	//Draw connection to the parent
	Unit* parent = unit->parent;
	if (parent != NULL && parent->alive) {
		if (parent->x - viewX >= 0 && parent->x - viewX < VIEW_WIDTH &&
				parent->y - viewY >= 0 && parent->y - viewY < VIEW_WIDTH &&
				parent->z - viewZ >= 0 && parent->z - viewZ < VIEW_HEIGHT) {
			int unitX = screenX[unit->x - viewX][unit->y - viewY];
			int unitY = screenY[unit->x - viewX][unit->y - viewY][unit->z - viewZ] + unitImage[0]->hCenter;
			int parentX = screenX[parent->x - viewX][parent->y - viewY];
			int parentY = screenY[parent->x - viewX][parent->y - viewY][parent->z - viewZ] + unitImage[0]->hCenter;
			SDL_SetRenderDrawColor(gRenderer, 0xEE, 0xEE, 0xEE, 0xFF);
			SDL_RenderDrawLine(gRenderer, unitX, unitY, parentX, parentY);
			SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
		}
	}
}

void graphics_draw_map() {
	int sX, sY;

	//This draws the floor, which is only tops instead of full blocks
	if (viewZ > 0) {
		for (int i = 0; i < VIEW_WIDTH; i++) {
			int x = viewX + i;
			for (int j = 0; j < VIEW_WIDTH; j++) {
				sX = screenX[i][j];
				sY = screenYfloor[i][j];
				int y = viewY + j;
				if (x >= 0 && y >= 0 && x < MAP_WIDTH && y < MAP_WIDTH) {
					if (tilesMap[x][y][viewZ - 1] == tileGround) {
						if ((x / BASE_CHUNK_SIZE + y / BASE_CHUNK_SIZE + (viewZ - 1) / BASE_CHUNK_SIZE) % 2 == 0) {
							drawImage(groundTopImage, sX, sY);
						}
						else {
							drawImage(grassTopImage, sX, sY);
						}
					}
					else if (tilesMap[x][y][viewZ - 1] == tileGenerator) {
						drawImage(generatorTopImage, sX, sY);
					}
					else if (tilesMap[x][y][viewZ - 1] == tileWater) {
						drawImage(waterTopImage, sX, sY);
					}
				}
				else {
					drawImage(waterTopImage, sX, sY);
				}
			}
		}
	}

	//This draws the most of the visible content
	for (int i = 0; i < VIEW_WIDTH; i++) {
		int x = viewX + i;
		for (int j = 0; j < VIEW_WIDTH; j++) {
			int y = viewY + j;
			sX = screenX[i][j];
			if (x >= 0 && y >= 0 && x < MAP_WIDTH && y < MAP_WIDTH) {
				for (int k = 0; k < VIEW_HEIGHT; k++) {
					int z = viewZ + k;
					if (z >= 0 && z < MAP_HEIGHT) {
						sY = screenY[i][j][k];
						if (tilesMap[x][y][z] == tileEmpty) {
							Object* current = unitsMap[x][y][z];
							while (current != NULL) {
								if (current->type() == objectUnit) {
									Unit* unit = (Unit*)current;
									drawUnit(unit, i, j, k);
								}
								else if (current->type() == objectResource) {
									Resource* resource = (Resource*)current;
									Image* image = resourceImages[resource->resourceType % RESOURCE_TYPES];
									drawImage(image, sX, sY);
									if (resource->resourceType > RESOURCE_TYPES) {
										drawImage(image, sX, sY - 2);
										if (resource->resourceType > RESOURCE_TYPES * 2) {
											drawImage(image, sX, sY - 4);
										}
									}
								}
								current = current->sharesTileWithObject;
							}
						}
						else if (tilesMap[x][y][z] == tileGround) {
							if ((x / BASE_CHUNK_SIZE + y / BASE_CHUNK_SIZE + z / BASE_CHUNK_SIZE) % 2 == 0) {
								drawImage(groundImage, sX, sY);
							}
							else {
								drawImage(grassImage, sX, sY);
							}
						}
						else if (tilesMap[x][y][z] == tileGenerator) {
							drawImage(generatorImage, sX, sY);
						}
						else if (tilesMap[x][y][z] == tileWater) {
							drawImage(waterTopImage, sX, sY);
						}
					}
				}
			}
			else {
				if (viewZ <= 0) {
					sY = screenY[i][j][0 - viewZ];
					drawImage(waterTopImage, sX, sY);
				}
			}
		}
	}

	//This draws the blacked out sides
	int sLX, sRX;
	int leftSideY = viewY + VIEW_WIDTH - 1;
	int rightSideX = viewX + VIEW_WIDTH - 1;
	for (int i = 0; i < VIEW_WIDTH; i++) {
		int leftSideX = i + viewX;
		int rightSideY = i + viewY;
		sLX = screenX[i][VIEW_WIDTH - 1];
		sRX = screenX[VIEW_WIDTH - 1][i];
		for (int k = 0; k < VIEW_HEIGHT; k++) {
			int z = viewZ + k;
			if (z < MAP_HEIGHT) {
				sY = screenY[i][VIEW_WIDTH - 1][k];
				if (safeTilesMap(leftSideX, leftSideY, z) != tileEmpty) {
					if (i + 1 < VIEW_WIDTH && safeTilesMap(leftSideX + 1, leftSideY, z) != tileEmpty) {
						drawImage(blackLeftPlusImage, sLX, sY);
					}
					else {
						drawImage(blackLeftImage, sLX, sY);
					}
				}
				if (safeTilesMap(rightSideX, rightSideY, z) != tileEmpty) {
					if (i + 1 < VIEW_WIDTH && safeTilesMap(rightSideX, rightSideY + 1, z) != tileEmpty) {
						drawImage(blackRightPlusImage, sRX, sY);
					}
					else {
						drawImage(blackRightImage, sRX, sY);
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
								sX = screenX[i][j];
								sY = screenY[i][j][VIEW_HEIGHT - 1];
								drawImage(blackTopImage, sX, sY);
							}
						}
					}
				}
			}
		}
	}

	//SDL_RenderDrawLine(gRenderer, SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT);
}

void graphics_draw() {
	SDL_RenderClear(gRenderer);
	if (selected != NULL) {
		viewX = selected->x - VIEW_WIDTH / 2;
		viewY = selected->y - VIEW_WIDTH / 2;
		viewZ = selected->z - VIEW_HEIGHT / 2;
	}
	graphics_draw_map();
	graphics_draw_text();
	graphics_draw_minimap();
	SDL_RenderPresent(gRenderer);
}

void graphics_draw_minimap() {
	int ratio = 4;
	SDL_Rect rect = { SCREEN_WIDTH - MAP_WIDTH / ratio, 0, MAP_WIDTH / ratio, MAP_WIDTH / ratio };
	SDL_FillRect(screenSurface, &rect, 120 * 256 * 256 + 120 * 256 + 120);
	SDL_Rect rect2 = { SCREEN_WIDTH - (MAP_WIDTH - viewX) / ratio, viewY / ratio, VIEW_WIDTH / ratio, VIEW_WIDTH / ratio };
	SDL_FillRect(screenSurface, &rect2, 136 * 256 * 256 + 136 * 256 + 136);

	for (int i = 0; i < 10; i++) {
		for (std::list<Object*>::iterator it = objects[(time + i) % BUCKETS].begin(); it != objects[(time + i) % BUCKETS].end(); it++) {
			Object* object = *it;
			if (object->objectType == objectUnit) {
				Unit* unit = (Unit*)object;
				if (unit->parent != NULL) {
					SDL_SetRenderDrawColor(gRenderer, 180, 180, 180, 0);
					SDL_RenderDrawLine(gRenderer, SCREEN_WIDTH - (MAP_WIDTH - unit->x) / ratio, unit->y / ratio, SCREEN_WIDTH - (MAP_WIDTH - unit->parent->x) / ratio, unit->parent->y / ratio);
				}
			}
		}
	}
	for (int i = 0; i < 10; i++) {
		for (std::list<Object*>::iterator it = objects[(time + i) % BUCKETS].begin(); it != objects[(time + i) % BUCKETS].end(); it++) {
			Object* object = *it;
			if (object->objectType == objectUnit) {
				Unit* unit = (Unit*)object;
				SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 0);
				SDL_RenderDrawPoint(gRenderer, SCREEN_WIDTH - (MAP_WIDTH - unit->x) / ratio, unit->y / ratio);
			}
		}
	}
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
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
	string str;
	switch (step % RESOURCE_TYPES) {
	case 0:
		str = "red";
		break;
	case 1:
		str = "yellow";
		break;
	case 2:
		str = "green";
		break;
	case 3:
		str = "sky";
		break;
	case 4:
		str = "blue";
		break;
	case 5:
		str = "purple";
		break;
	case 6:
		str = "black";
		break;
	case 7:
		str = "white";
		break;
	case 8:
		str = "orange";
		break;
	case 9:
		str = "dirt";
		break;
	}
	return str + std::to_string(step);
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