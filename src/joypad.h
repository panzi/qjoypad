#ifndef JOYPAD_H
#define JOYPAD_H

//parts of the joypad
#include "button.h"
#include "axis.h"

//the widget that will edit this
#include "joypadw.h"

//for raising errors
#include "error.h"

//for actually interacting with the joystick devices
#include <linux/joystick.h>
#include <fcntl.h>
#include <unistd.h>
#include <QTextStream>
#include <QHash>
#include <QSocketNotifier>


class JoyPadWidget;

//represents an actual joystick device
class JoyPad : public QObject{
	Q_OBJECT
    friend class JoyPadWidget;
	friend class QuickSet;
	public:
		void setupJoyDeviceListener(int dev);
        JoyPad( int i, int dev );
        //read from a stream
		bool readConfig( QTextStream* stream );
		//write to a stream
		void write( QTextStream* stream );
		//release any pushed buttons and return to a neutral state
		void release();
		//handle an event from the joystick device this is associated with
		void jsevent( js_event msg );
		//reset to default settings
		void toDefault();
		//true iff this is currently at default settings
		bool isDefault();
		//release the connection to the real joystick
		void unsetDev();
		//read the dimensions on the real joystick and use them
		void resetToDev( int dev );
		//generates a name ("Joystick 1")
		QString getName() { return "Joystick " + QString::number(index+1);};
		
		//it's just easier to have these publicly available.
		int joydev;  //the actual file descriptor to the joystick device
		int axes;    //the number of axes available on this device
		int buttons; //the number of buttons
		
		//request the joypad to make a JoyPadWidget. We create them this way
		//so that the joypad is always aware when it has a widget active.
		JoyPadWidget* widget(QWidget* parent, int i);
		//called when the joypad is no longer being edited.
		void releaseWidget();
	protected:
		//lookup axes and buttons. These are dictionaries to support
		//layouts with different numbers of axes/buttons than the current
		//devices. Note that with the current layout settings, the defined
		//buttons that don't actually exist on the device may not be contiguous.
		QHash<int, Axis*> Axes;
		QHash<int, Button*> Buttons;
		//the index of this device (devicenum)
		int index;
		
		//the widget that edits this. Mainly I keep track of this to know if
		//the joypad is currently being edited.
		JoyPadWidget* jpw;
        QSocketNotifier *joydevFileHandle;
        QSocketNotifier *joydevFileException;
    public slots:    
        void handleJoyEvents(int fd);
        void errorRead(int fd);
};

#endif
