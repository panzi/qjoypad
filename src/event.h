#ifndef JEVENT_H
#define JEVENT_H

#include <qapplication.h>

#include <X11/extensions/XTest.h>
#include <sys/timeb.h>

enum eventType {KREL, KPRESS, BREL, BPRESS, WARP};


//a simplified event structure that can handle buttons and mouse movements
struct xevent {
	eventType type;
	int value1;	//button, keycode, or x
	int value2; //y
};


void sendevent( xevent e );

#endif
