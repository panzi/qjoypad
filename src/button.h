#ifndef BUTTON_H
#define BUTTON_H

#include <qstringlist.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qobject.h>

#include "event.h"
#include "constant.h"
#include "keycode.h"

class Button : public QObject {
	friend class ButtonEdit;
	public:
		Button( int i );
		virtual ~Button() {};
		//read from stream
		virtual bool read( QTextStream* stream );
		//write to stream
		virtual void write( QTextStream* stream );
		//release any keys that have been pressed through events
		virtual void release();
		//process an event from the actual joystick device
		virtual void jsevent( int value );
		//reset default settings
		virtual void toDefault();
		//True iff is currently using default settings
		virtual bool isDefault();
		//returns a string representation of this button.
		QString getName() { return "Button " + QString::number(index+1);};
		QString status();
		void setKey(bool mouse, int value);
	protected:
		//true iff this button is physically depressed.
		bool isOn;
		//counter used in making rapidfire work
		int tick;
		//the index of this button on the joystick
		int index;
		//happens every MSEC (constant.h) milliseconds
		virtual void timerEvent( QTimerEvent* );
		//actually sends a key press/release
		virtual void click( bool press );

		//button settings
		bool rapidfire;
		bool sticky;
		bool useMouse;
		int keycode;
		int mousecode; //like keycode, only mousebutton  ;)
};

#endif
