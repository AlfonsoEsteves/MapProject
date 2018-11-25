#pragma once

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 750

#define RESOURCES_PATH string("C:/Users/Usuario/Documents/Projects/MapProject/Resources/")
//#define RESOURCES_PATH string("C:/Users/alfonso.esteves/Documents/Otros/MapProject/Resources/")

#define VIEW_WIDTH 80
#define VIEW_HEIGHT 45

#define DRAW_GRAPHICS_EVERY_X_FRAMES 6

bool graphics_init();

void graphics_draw();

void graphics_close();

string stepName(int step);

unsigned char safeTilesMap(int x, int y, int z);

extern int screenX[VIEW_WIDTH][VIEW_WIDTH];
extern int screenY[VIEW_WIDTH][VIEW_WIDTH][VIEW_WIDTH];

