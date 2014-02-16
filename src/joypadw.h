#ifndef QJOYPAD_JOYPAD_WIDGET_H
#define QJOYPAD_JOYPAD_WIDGET_H

//parts for the widget
//Added by qt3to4:

#include <QList>
#include <linux/joystick.h>
#include "axisw.h"
//this all relates to a JoyPad
#include "joypad.h"
//and a JoyPadWidget is composed of AxisWidgets and ButtonWidgets
#include "buttonw.h"
//JoyPadWidget also is what initiates the whole QuickSet procedure  :)
#include "quickset.h"

//because of some circularity issues, I need to forward declare these.
class JoyPad;
class QuickSet;

//Widget for editing a JoyPad
class JoyPadWidget : public QWidget {
	Q_OBJECT
	public:
		JoyPadWidget( JoyPad* jp, int i, QWidget* parent);
		~JoyPadWidget();
		//takes in an event and decides whether or not to flash anything
        void jsevent(const js_event &msg );
	public slots:
		//called whenever one of the subwidgets flashes... used to determine
		//when to emit the flashed() signal.
		void flash( bool on );
		//update all the components, that is, get the proper labels up on the buttons
		void update();
		//reset everything
		void clear();
		//quickset!
		void setAll();
	signals:
		//happens whenever the tab that represents this joypadwidget should flash
		//(either on or off) The int is the index of this widget so that this
		//signal can be directly connected to FlashRadioArray's flash(int)
		void flashed(int);
    private:
		//the joypad this is linked to
		JoyPad* joypad;
		//the index of this widget IN THE WIDGET STACK! This is unique from
		//joypad->index and has nothing to do with device number.
		int index;

		//how many times one of the components has flashed on minus how many
		//times one of the components has flashed off. This is how we know when
		//none of the components are left lit.
		int flashcount;
		
		//the parts of the dialog
        QGridLayout *layoutMain;
        QList<AxisWidget*> axes;
        QList<ButtonWidget*> buttons;
        QPushButton *btnClear, *btnAll;
		
		//the quickset window, when we create it
		QuickSet* quickset;
};

#endif
