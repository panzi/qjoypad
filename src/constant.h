#ifndef JCONST_H
#define JCONST_H

//How many cycles there are per click.
#define FREQ 10

//How many milliseconds per cycle.
#define MSEC 5

//events will be generated every FREQ * MSEC milliseconds. The duration of the
//event can be anywhere between 0 * MSEC and FREQ * MSEC. This means there will
//be FREQ + 1 levels of gradation.


//maximum range of values from joystick driver
#define JOYMAX 32767
#define JOYMIN -32767

//maximum number of defined keys
#define MAXKEY 122

//fastest the mouse can go. Completely arbitrary.
#define MAXMOUSESPEED 5000

#define NAME "QJoyPad 4.0-pre"

#define MOUSE_OFFSET 400
#ifdef _DEBUG
#define DEBUG(str) printf(str);
#else
#define DEBUG
#endif

#endif
