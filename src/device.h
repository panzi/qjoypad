#ifndef JOY_DEVICE_H
#define JOY_DEVICE_H

#include <qintdict.h>
#include "joypad.h"

extern QIntDict<JoyPad> available;
extern QIntDict<JoyPad> joypads;

#endif
