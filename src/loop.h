#ifndef JOY_LOOP_H
#define JOY_LOOP_H

//this is an event loop
#include <qeventloop.h>

//for reading events from the joysticks
#include <unistd.h>
#include <linux/joystick.h>

//to pass events on to the joypads
#include "joypad.h"
//to know which devices need to be checked for joystick events
#include "device.h"

//A Component is either an Axis or a Button
class Component;

//the main event loop of QJoyPad; its main distinction from a regular event loop
//is that it reads joystick events on every iteration and also provides a timer
//to any Components that need it.
class JoyLoop : public QEventLoop {
	public:
		JoyLoop();
		bool processEvents( ProcessEventsFlags );
		//provide a timer to the requesting Component
		void takeTimer( Component* c );
		//stop providing a timer to the given Component
		void tossTimer( Component* c );
	protected:
		//happens ever MSEC miliseconds  :)
		virtual void timerEvent( QTimerEvent* );
		//just a counter, nothing more.
		int tick;
		//a list of the Components that need a timer
		QPtrList<Component> timerList;
};

#endif
