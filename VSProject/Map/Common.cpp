#include "Common.h"

bool programExecutionQuit = false;

Object* mouseOverObject = NULL;
Object* selected = NULL;

char formated[256];

#ifdef DEBUG
int lastExecutedObject = -999999;

void bp(int id) {
	if (id == DEBUG_OBJECT && time >= FAST_FOWARD) {
		printf("");
	}
}
#endif

void error(string msg) {
#	ifdef DEBUG
	cout << msg << "   time: " << time << "   objectId: " << lastExecutedObject;
#	endif
	programExecutionQuit = true;
	pause();
}

void pause() {
	int var;
	cin >> var;
}

int getX(const int& dir) {
	if (dir == 0)
		return 1;
	if (dir == 2)
		return -1;
	return 0;
}

int getY(const int& dir) {
	if (dir == 1)
		return 1;
	if (dir == 3)
		return -1;
	return 0;
}

void format(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vsnprintf(formated, sizeof formated, fmt, args);
	va_end(args);
}