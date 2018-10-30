#pragma once

bool graphics_init();

void graphics_draw();

void graphics_close();

bool unitHasBrackets(Unit* unit);

string stepName(int step);

extern int screenX[VIEW_WIDTH][VIEW_WIDTH];
extern int screenY[VIEW_WIDTH][VIEW_WIDTH][VIEW_WIDTH];