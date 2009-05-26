#ifndef AXIS_H
#define AXIS_H

//abs()
#include <stdlib.h>

//parent of Axis
#include "component.h"

//to request a periodic tap on the shoulder for gradient mode
#include "timer.h"

//default and arbitrary values for dZone and xZone
#define DZONE 3000
#define XZONE 30000

//each axis can create a key press or move the mouse in one of four directions.
enum AxisMode {keybd, mousepv, mousenv, mouseph, mousenh};

//represents one joystick axis
class Axis : public Component {
	//so AxisEdit can manipulate fields directly.
	friend class AxisEdit;
	public:
		Axis( int i );
		~Axis();
		//read axis settings from a stream
		bool read( QTextStream* stream );
		//write axis settings to a stream
		void write( QTextStream* stream );
		//releases any pushed buttons and returns to a neutral state
		void release();
		//pass a message from the joystick device to this axis object
		void jsevent( int value );
		//revert to default settings
		void toDefault();
		//True iff currently at defaults
		bool isDefault();
		QString getName() { return "Axis " + QString::number(index+1);};
		//true iff the given value is in the dead zone for this axis.
		bool inDeadZone( int val );
		//a descriptive string used as a label for the button representing this axis
		QString status();
		//set the key code for this axis. Used by quickset.
		void setKey(bool positive, int value);
		//happens every MSEC milliseconds (constant.h)
		//uses tick to decide if key events should be generated
		void timer( int tick );
		//recalculates the gradient curve. This should be run every time
		//maxSpeed, xZone, or dZone are changed.
		void adjustGradient();
	protected:
		//This axis is logically depressed (positive or negative)
		//if the axis is gradient, this is true even if it is not
		//currently generating a keypress at the instant.
		bool isOn;
		//which joystick this actually is
		int index;
		//actually sends key events. Press is true iff the key
		//is to be depressed as opposed to released.
		virtual void move( bool press );
		//is a key currently depressed?
		bool isDown;

		//variables for calculating quadratic used for gradient mouse axes
		double a,b,c;

		//actual axis settings:
		bool gradient;
		int maxSpeed; //0..MAXMOUSESPEED
		int throttle; //-1 (nkey), 0 (no throttle), 1 (pkey)
		int dZone;//-32767 .. 32767
		int xZone;//-32767 .. 32767
		AxisMode mode;
		//positive keycode
		int pkeycode;
		//negative keycode
		int nkeycode;
		//the key that is currently pressed
		int downkey;
		//the position of the axis, as from jsevent
		int state;
		//how long a key should stay down when in gradient mode
		//note, the key is still clicked at the same pace no matter what,
		//this just decides how long it stays down each cycle.
		int duration;
};

#endif
