#ifndef JOY_ICON_H
#define JOY_ICON_H

#include <qdialog.h>
#include <qpixmap.h>
//#include <qpopupmenu.h>
#include <qobject.h>

#include "constant.h"

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
