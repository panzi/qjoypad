#include "icon.h"

#include <qpopupmenu.h>

FloatingIcon::FloatingIcon( const QPixmap &icon, QPopupMenu *popup, QWidget *parent, const char *name)
	: QDialog( parent, name ) {
	setCaption(NAME);
	setPaletteBackgroundPixmap(icon);
	pop = popup;

	setFixedSize(64,64);
}

void FloatingIcon::mousePressEvent( QMouseEvent* e ) {
	if (e->button() == RightButton) {
		pop->popup( e->globalPos() );
		e->accept();
	}
	else {
		emit clicked();
	}
}

void FloatingIcon::closeEvent( QCloseEvent* e ) {
	emit closed();
	e->accept();
}
