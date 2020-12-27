#include <QX11Info>
#include "event.h"

//actually creates an XWindows event  :)
void sendevent(const FakeEvent &e) {
    Display* display = QX11Info::display();

    switch (e.type) {
    case FakeEvent::MouseMove:
        if (e.move.x == 0 && e.move.y == 0) return;
        XTestFakeRelativeMotionEvent(display, e.move.x, e.move.y, 0);
        break;

    case FakeEvent::MouseMoveAbsolute:
      {
        Screen* screen = XDefaultScreenOfDisplay(display);
        static int rememberX = 0, rememberY = 0;
        if (e.move.x) rememberX = e.move.x;
        if (e.move.y) rememberY = e.move.y;
        const int scaledX100 = rememberX * (XWidthOfScreen(screen)/2) / 100;
	const int scaledY100 = rememberY * (XHeightOfScreen(screen)/2) / 100;
	XTestFakeMotionEvent(display, DefaultScreen(display),
                             XWidthOfScreen(screen)/2 + scaledX100, 
                             XHeightOfScreen(screen)/2 + scaledY100, 0);
        break;
      }
    case FakeEvent::KeyUp:
        if (e.keycode == 0) return;
        XTestFakeKeyEvent(display, e.keycode, false, 0);
        break;

    case FakeEvent::KeyDown:
        if (e.keycode == 0) return;
        XTestFakeKeyEvent(display, e.keycode, true, 0);
        break;

    case FakeEvent::MouseUp:
        if (e.keycode == 0) return;
        XTestFakeButtonEvent(display, e.keycode, false, 0);
        break;

    case FakeEvent::MouseDown:
        if (e.keycode == 0) return;
        XTestFakeButtonEvent(display, e.keycode, true, 0);
        break;
    }
    XFlush(display);
}
