#ifndef QJOYPAD_KEYCODE_H
#define QJOYPAD_KEYCODE_H

//To create the "press a key" dialog:
#include <QPushButton>
#include <QDialog>
#include <QPaintEvent>
#include <QPainter>


#include "constant.h"

//Produce a string for any keycode
const QString ktos( int keycode );

//The X11 display, taken from main.cpp
extern Display* display;


//a button that requests a keycode from the user when clicked.
class KeyButton : public QPushButton {
	Q_OBJECT
	public:
		KeyButton(QString name, int val, QWidget* parent, bool m = false, bool nowMouse = false);
        int getValue() {return value;}
        int choseMouse() {return mouseClicked;}
	protected slots:
		void onClick();
	private:
		QString buttonname;
		int value;
		bool mouse;
		bool mouseClicked;
};

#endif
