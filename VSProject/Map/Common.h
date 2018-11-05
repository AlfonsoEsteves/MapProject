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

extern int frameDuration;

#ifdef DEBUG
extern int lastExecutedObject;
/*extern int debug_areasCreated;
extern int debug_areasDestroyed;
extern int debug_chunksCreated;
extern int debug_chunksDestroyed;
extern int debug_unitsCreated;
extern int debug_unitsDestroyed;
extern int debug_generatorsCreated;
extern int debug_generatorsDestroyed;*/
#endif

void format(const char *fmt, ...);

void error(string msg);

void pause();

int getX(const int& dir);
int getY(const int& dir);
