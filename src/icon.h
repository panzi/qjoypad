#ifndef JOY_ICON_H
#define JOY_ICON_H

//for creating a floating icon in notray mode  :)
#include <qdialog.h>
//the pixmap to load
#include <qpixmap.h>

#include "constant.h"

//A quirk in QT forbids me from simply including qpopupmenu.h
class QPopupMenu;

class FloatingIcon : public QDialog {
	Q_OBJECT
	public:
		FloatingIcon( const QPixmap &icon, QPopupMenu *popup = 0, QWidget *parent = 0, const char *name = 0);
	signals:
		void closed();
		void clicked();
	protected:
		void mousePressEvent( QMouseEvent* e );
		void closeEvent( QCloseEvent* e );
		QPopupMenu* pop;
};

#endif
