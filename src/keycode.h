#ifndef KEYCODE_H
#define KEYCODE_H

#include <qdialog.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qapplication.h>

//Defines constants needed for x11Event
#include <X11/Xlib.h>

#include "constant.h"

//Produce a string for any keycode
const char* ktos( int keycode );

extern Display* display;

//a keycode dialog box
class GetKey : public QDialog {
	Q_OBJECT
	public:
		GetKey( QString button, bool m = false );
	protected:
		bool x11Event( XEvent* e );
		void paintEvent ( QPaintEvent * );
	public slots:
		void finish( int );
	private:
		QString Text;
		bool mouse;
};

//a button that requests a keycode from the user
class KeyButton : public QPushButton {
	Q_OBJECT
	public:
		KeyButton(QString name, int val, QWidget* parent, bool m = false, bool nowMouse = false);
		int getValue() {return value;};
		int getMouse() {return mouseClicked;};
	protected slots:
		void onClick();
	private:
		QString buttonname;
		int value;
		bool mouse;
		bool mouseClicked;
};

#endif
