#ifndef QUICKSET_H
#define QUICKSET_H

//for building the dialog
#include <QLayout>
#include <QLabel>
#include <QPushButton>

#include <linux/joystick.h>

//to request new keycodes
//#include "keycode.h"
//to actually set the joypad
#include "joypad.h"

//because of some circularity issues  ;)
class JoyPad;

//a dialog to quickly set a key to a button
class QuickSet : public QDialog {
	public:
        QuickSet(JoyPad* jp, QWidget *parent = 0);
		//this needs to see js_events so it can capture them directly
		void jsevent( js_event msg );
	private:
		//the joypad that is being configured
		JoyPad* joypad;
		//when a joystick event has trigered QuickSet, setting is true and it
		//is waiting for a keypress or mouse click. Otherwise, setting is false.
		bool setting;
};

#endif
