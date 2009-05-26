#ifndef JOY_DEVICE_H
#define JOY_DEVICE_H

#include <qintdict.h>
#include "joypad.h"

//the purpose of this file is to make device information available to what
//needs it.

//a collection of joysticks currently available on this computer
extern QIntDict<JoyPad> available;

//a collection of joypad objects representing all the available joysticks
//as well as the ones defined in a layout buy not currently plugged in.
extern QIntDict<JoyPad> joypads;

#endif
