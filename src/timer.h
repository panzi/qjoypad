#ifndef JTIMER_H
#define JTIMER_H

#include "component.h"

//the purpose of this file is to make a list of Components that need timer
//notification available to JoyLoop, Axis, and Buttons
extern void takeTimer( Component* ); //taken from main.cpp
extern void tossTimer( Component* ); //taken from main.cpp

#endif
