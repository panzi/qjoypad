#include "loop.h"


bool JoyLoop::processEvents( ProcessEventsFlags ) {
	QIntDictIterator<JoyPad> it( available );
	int joydev;
	JoyPad* pad;
	js_event msg;
	int len;
    for ( ; it.current(); ++it ) {
		pad = it.current();
		joydev = pad->joydev;
		len = read( joydev, &msg, sizeof(js_event));
		if (len == (int) sizeof(js_event)) {
			pad->jsevent(msg);
		}
	}

	usleep(1);
	
	return QEventLoop::processEvents(AllEvents);
}
