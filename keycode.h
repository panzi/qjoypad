#ifndef KEY_H
#define KEY_H

#include <qdialog.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <X11/Xlib.h>

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
