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
	//if it was the right mouse button,
	if (e->button() == RightButton) {
		//bring up the popup menu.
		pop->popup( e->globalPos() );
		e->accept();
	}
	else {
		//otherwise, treat it as a regular click.
		emit clicked();
	}
}

void FloatingIcon::closeEvent( QCloseEvent* e ) {
	emit closed();
	e->accept();
}
