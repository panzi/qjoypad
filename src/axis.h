#ifndef AXIS_H
#define AXIS_H

//abs()
#include <stdlib.h>

#include <qstringlist.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qobject.h>

#include "event.h"
#include "constant.h"

enum AxisMode {keybd, mousepv, mousenv, mouseph, mousenh};

//represents one joystick axis
class Axis : public QObject {
	friend class AxisEdit;
	Q_OBJECT
	public:
		Axis( int i );
		~Axis();
		//read axis settings from a stream
		virtual bool read( QTextStream* stream );
		//write axis settings to a stream
		virtual void write( QTextStream* stream );
		//releases any pushed buttons and returns to a neutral state
		virtual void release();
		//pass a message from the joystick device to this axis object
		virtual void jsevent( int value );
		//revert to default settings
		virtual void toDefault();
		//True iff currently at defaults
		virtual bool isDefault();
		QString getName() { return "Axis " + QString::number(index+1);};
		bool inDeadZone( int val );
		QString status();
		void setKey(bool positive, int value);
	protected:
		//This axis is logically depressed (positive or negative)
		//if the axis is gradient, this is true even if it is not
		//currently generating a keypress
		bool isOn;
		//used for timing how frequently a gradient axis should fire.
		//just a counter variable.
		int tick;
		//which joystick this actually is
		int index;
		//happens every MSEC milliseconds (constant.h)
		//uses tick to decide if key events should be generated
		virtual void timerEvent( QTimerEvent* );
		//actually sends key events. Press is true iff the key
		//is to be depressed as opposed to released1.
		virtual void move( bool press );
		//is a key currently depressed?
		bool isDown;

		//actuall axis settings:
		bool gradient;
		int maxSpeed; //0..100
		int throttle; //-1 (nkey), 0 (no throttle), 1 (pkey)
		int dZone;//-32767 .. 32767
		int xZone;
		AxisMode mode;
		//positive keycode
		int pkeycode;
		//negative keycode
		int nkeycode;
		//the key that is currently pressed
		int downkey;
		int state; //as from jsevent
		//how long a key should stay down when in gradient mode
		int duration;
};

#endif
