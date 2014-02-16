#ifndef QJOYPAD_GETKEY_H
#define QJOYPAD_GETKEY_H

#include <QDialog>
#include <QPainter>
#include <QPixmap>
#include <QIcon>
#include "constant.h"
#include <X11/Xlib.h>
//The KeySym for "x"
#define XK_x 0x078

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

#endif
