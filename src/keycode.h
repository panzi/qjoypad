#ifndef KEYCODE_H
#define KEYCODE_H

//To create the "press a key" dialog:
#include <qdialog.h>
#include <qpainter.h>

//For the KeyButton widget
#include <qpushbutton.h>

//For converting X Windows key names to something nicer.
#include <qregexp.h>

//Defines constants needed for x11Event
#include <X11/Xlib.h>
//The KeySym for "x"
#define XK_x 0x078

#define MOUSE_OFFSET 400

#include "constant.h"

//Produce a string for any keycode
const QString ktos( int keycode );

//The X11 display, taken from main.cpp
extern Display* display;

//a keycode dialog box
class GetKey : public QDialog {
	Q_OBJECT
	public:
		GetKey( QString button, bool m = false );
	protected:
		//to filter through every event this thing sees, before QT does.
		bool x11Event( XEvent* e );
		//to avoid focus issues, there is only the dialog widget, all the
		//rest is painted on. So, I need to know when to repaint.
		void paintEvent ( QPaintEvent * );
	private:
		//the dialog's message
		QString Text;
		//does this dialog accept mouse clicks?
		bool mouse;
};

//a button that requests a keycode from the user when clicked.
class KeyButton : public QPushButton {
	Q_OBJECT
	public:
		KeyButton(QString name, int val, QWidget* parent, bool m = false, bool nowMouse = false);
		int getValue() {return value;};
		int choseMouse() {return mouseClicked;};
	protected slots:
		void onClick();
	private:
		QString buttonname;
		int value;
		bool mouse;
		bool mouseClicked;
};

#endif
