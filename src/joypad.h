#ifndef JOYPAD_H
#define JOYPAD_H

#include <qintdict.h>
#include <qobject.h>

#include <linux/joystick.h>
#include <fcntl.h>
#include <unistd.h>

#include "button.h"
#include "axis.h"
#include "joypadw.h"
#include "error.h"

class JoyPadWidget;

//represents an actual joystick device
class JoyPad : public QObject{
	friend class JoyPadWidget;
	friend class QuickSet;
	public:
		JoyPad( int i, int dev );
		//read from a stream
		bool read( QTextStream* stream );
		//write to a stream
		void write( QTextStream* stream );
		//handle an event from the joystick device this is associated with
		virtual void jsevent( js_event msg );
		//reset to default settings
		virtual void toDefault();
		//true iff this is currently at default settings
		virtual bool isDefault();
		//generates a name ("Joystick 1")
		void unsetDev() {close(joydev); joydev = -1;};
		void resetToDev( int dev );
		QString getName();
		
		int joydev;
		int axes;
		int buttons;
		
		JoyPadWidget* widget(QWidget* parent, int i);
		void releaseWidget();
	protected:
		//lookup axes and buttons. These are dictionaries to support
		//layouts with different numbers of axes/buttons than the current
		//devices.
		QIntDict<Axis> Axes;
		QIntDict<Button> Buttons;
		//the index of this device (devicenum)
		int index;
		
		JoyPadWidget* jpw;
};

#endif
