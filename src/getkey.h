#ifndef QJOYPAD_GETKEY_H
#define QJOYPAD_GETKEY_H

#include <QDialog>
#include <QPainter>
#include <QPixmap>
#include <QIcon>
#include <QCloseEvent>
#include "constant.h"

#include <X11/Xlib.h>
// undefined Xlib macros that collide with Qt types
#undef Bool
#undef Unsorted

//The KeySym for "x"
#define XK_x 0x078

//a keycode dialog box
class GetKey : public QDialog {
	Q_OBJECT
	public:
        GetKey(const QString& button, bool useMouse = false, QWidget *parent = 0);

        static int getKey(const QString& button, bool useMouse, bool *choseMouse, QWidget *parent = 0);

        int getValue() const { return value; }
        bool choseMouse() const { return wasMouse; }
	protected:
        //to filter through every event this thing sees, before Qt does.
		bool x11Event( XEvent* e );
		//to avoid focus issues, there is only the dialog widget, all the
		//rest is painted on. So, I need to know when to repaint.
        void paintEvent (QPaintEvent *);
	private:
		//the dialog's message
        QString text;
		//does this dialog accept mouse clicks?
        bool useMouse;
        bool wasMouse;
        int  value;
};

#endif
