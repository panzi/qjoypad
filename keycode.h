#ifndef KEYCODE_H
#define KEYCODE_H

#include <qdialog.h>
#include <qpainter.h>

//Defines constants needed for x11Event
#include <X11/Xlib.h>

//Produce a string for any keycode
const char* ktos( int keycode );

class GetKey : public QDialog
{
	Q_OBJECT
	public:
		GetKey( QString button );
	protected:
		bool x11Event( XEvent* e );
	public slots:
		void finish( int );
	private:
		QString Text;
};
#endif
