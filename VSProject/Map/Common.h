#pragma once

#include <iostream>
#include <stdlib.h>
#include <string>
#include <SDL.h>
#include "SDL_ttf.h"

using namespace std;

#include "Parameters.h"
#include "Object.h"
#include "Resource.h"
#include "Generator.h"
#include "Map.h"
#include "Tunnel.h"
#include "Chunk.h"
#include "BaseChunk.h"
#include "SuperChunk.h"
#include "Pathfinder.h"
#include "Graphics.h"

extern bool programExecutionQuit;

extern Object* mouseOverObject;
extern Object* selected;

extern char formated[256];

extern unsigned int frameDuration;

#ifdef DEBUG
extern int lastExecutedObject;
#endif

void format(const char *fmt, ...);

void error(string msg);

void pause();

int getX(const int& dir);
int getY(const int& dir);
