#ifndef BUTTON_H
#define BUTTON_H

#include <QTimer>
#include <QTextStream>


//for getting a key name in status()
#include "keycode.h"

//note that the Button class, unlike the axis class, does not need a release
//function because it releases the key as soon as it is pressed.
class Button : public QObject {
	Q_OBJECT
    friend class ButtonEdit;
	public:
		Button( int i );
		~Button();
		//read from stream
		bool read( QTextStream* stream );
		//write to stream
		void write( QTextStream* stream );
		//releases any pushed buttons and returns to a neutral state
		void release();
		//process an event from the actual joystick device
		void jsevent( int value );
		//reset default settings
		void toDefault();
		//True iff is currently using default settings
		bool isDefault();
		//returns a string representation of this button.
		QString getName() { return "Button " + QString::number(index+1);};
		//a descriptive string used as a label for the button representing this axis
		QString status();
		//set the key code for this axis. Used by quickset.
		void setKey(bool mouse, int value);
		//happens every MSEC (constant.h) milliseconds
		void timerTick( int tick );
	protected:
		//true iff this button is physically depressed.
		bool isButtonPressed;
		//the index of this button on the joystick
		int index;
		//actually sends a key press/release
		virtual void click( bool press );
		//is a simulated key currently depressed?
		bool isDown;
        int tick;
		//button settings
		bool rapidfire;
		bool sticky;
		bool useMouse;
		int keycode;
		int mousecode; //like keycode, only mousebutton  ;)
        QTimer *timer;
    public slots:
        void timerCalled();
};

#endif
