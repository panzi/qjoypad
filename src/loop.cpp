#include "loop.h"

JoyLoop::JoyLoop() {
	//I like to start from zero, though, in all honesty, this doesn't matter.
	tick = 0;
}

bool JoyLoop::processEvents( ProcessEventsFlags ) {
	//standard event processing, except we get to throw in a few things of
	//our own!
	
	int joydev;
	JoyPad* pad;
	js_event msg;
	int len;
	
	//go through the currently plugged-in joysticks
	QIntDictIterator<JoyPad> it( available );
    for ( ; it.current(); ++it ) {
		pad = it.current();
		//get the file descriptor for the device
		joydev = pad->joydev;
		//and try reading an event, non-blocking
		len = read( joydev, &msg, sizeof(js_event));
		//if there was a real event waiting,
		if (len == (int) sizeof(js_event)) {
			//pass that event on to the joypad!
			pad->jsevent(msg);
		}
	}

	//sleep for a moment. This is just to keep us from throwing all the
	//available processer power into madly checking for new events.
	usleep(1);
	
	//now we can let QT process all of its events, like GUI events and timers.
	return QEventLoop::processEvents(AllEvents);
}

void JoyLoop::takeTimer( Component* c ) {
	//if this is the first Component asking for a timer, then we can start
	//up the timer! It isn't running unless someone needs it.
	if (timerList.isEmpty()) startTimer( MSEC );
	//remember this Component so we can signal it.
	timerList.append( c );
}

void JoyLoop::tossTimer( Component* c ) {
	//forget that Component, it doesn't NEED us any more  :(
	timerList.remove( c );
	//if that means there are no longer any Components listening for timer
	//events, then we can go ahead and stop timing.
	if (timerList.isEmpty()) killTimers();
}

void JoyLoop::timerEvent( QTimerEvent* ) {
	//ever MSEC miliseconds...
	
	//increment the counter (used for timing in axis.cpp and button.cpp)
	++tick;
	//and notify every Component in the list.
    for ( Component* c = timerList.first(); c; c = timerList.next() ) {
		c->timer(tick);
	}
}
