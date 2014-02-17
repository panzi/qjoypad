#include <QPainter>

#include "icon.h"
#include "config.h"

FloatingIcon::FloatingIcon( const QString &icon, QMenu *popup, QWidget *parent, const char *name)
        : QDialog( parent ), icon(icon) {
    this->setObjectName(name);
    setAttribute(Qt::WA_QuitOnClose);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);
    setWindowTitle(QJOYPAD_NAME);
    pop = popup;

    setFixedSize(64,64);
}

void FloatingIcon::mousePressEvent( QMouseEvent* event ) {
    //if it was the right mouse button,
    if (event->button() == Qt::RightButton) {
        //bring up the popup menu.
        pop->popup( event->globalPos() );
        event->accept();
    }
    else {
        //otherwise, treat it as a regular click.
        emit clicked();
    }
}

void FloatingIcon::paintEvent( QPaintEvent* event ) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.drawPixmap(0, 0, icon);
}
