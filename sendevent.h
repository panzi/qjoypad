#ifndef SENDEVENT_H
#define SENDEVENT_H

#include <X11/Xlib.h>
#include <sys/timeb.h>

void sendevent(int typ, int keycode);

#endif
