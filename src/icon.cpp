#include "icon.h"


FloatingIcon::FloatingIcon( const QPixmap &icon, QMenu *popup, QWidget *parent, const char *name)
        : QDialog( parent ) {
    this->setObjectName(name);
    setWindowTitle(NAME);
    QPalette palette;
    palette.setBrush(backgroundRole(), QBrush(icon));
    setPalette(palette);
    //setPaletteBackgroundPixmap(icon);
    pop = popup;

    setFixedSize(64,64);
}

void FloatingIcon::mousePressEvent( QMouseEvent* e ) {
    //if it was the right mouse button,
    if (e->button() == Qt::RightButton) {
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
