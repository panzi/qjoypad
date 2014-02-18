#ifndef QJOYPAD_EVENT_H
#define QJOYPAD_EVENT_H

//for the functions we need to generate keypresses / mouse actions
#include <X11/extensions/XTest.h>

//a simplified event structure that can handle buttons and mouse movements
struct FakeEvent {
    //types of events QJoyPad can create.
    //KeyRelease, KeyPress, ButtonRelease, ButtonPress, and MouseMove
    enum EventType {KeyUp, KeyDown, MouseUp, MouseDown, MouseMove};

    EventType type;
    union {
        int keycode;

        struct {
            int x;
            int y;
        } move;
    };
};

void sendevent(const FakeEvent& e);

#endif
