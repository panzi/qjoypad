#include "keydialog.hpp"

#include <QGuiApplication>
#include <QPointer>

KeyDialog::KeyDialog( const QString& button, bool m, QWidget* parent ) :
QDialog( parent ),
m_acceptMouse( m ),
m_text( this ),
m_isMouse( false ),
m_value( 0 )
{
    setWindowTitle( "Choose a key" );
    m_text.setText( QString( "Choose a new key %1for %2\n(Ctrl-X for no key)" ).
            arg( m ? "or mouse button " : "" ).arg( button ) );
    m_text.adjustSize();
    setFixedSize( m_text.width() + 20, m_text.height() + 20 );
    m_text.move( 10, 10 );
}

int KeyDialog::getKey( const QString& button, bool acceptMouse, bool* mouseOut, QWidget* parent ) {
    bool isMouse = false;
    quint32 value = 0;

    QPointer<KeyDialog> dialog = new KeyDialog( button, acceptMouse, parent );
    if ( dialog->exec() != QDialog::Accepted ) {
        delete dialog;
        return -1;
    }
    if ( !dialog ) {
        return -1;
    }

    isMouse = dialog->m_isMouse;
    value = dialog->m_value;
    delete dialog;

    if ( mouseOut ) {
        *mouseOut = isMouse;
    }
    return value;
}

void KeyDialog::keyReleaseEvent( QKeyEvent* event ) {
    if ( event->key() != Qt::Key_Control ) {
        return;
    }
    m_value = event->nativeScanCode();
    m_isMouse = false;
    accept();
}

void KeyDialog::keyPressEvent( QKeyEvent* event ) {
    if ( event->key() == Qt::Key_Control ) {
        return;
    }
    if ( event->key() == Qt::Key_X && QGuiApplication::keyboardModifiers() == Qt::ControlModifier ) {
        m_value = 0;
    } else {
        m_value = event->nativeScanCode();
    }
    m_isMouse = false;
    accept();
}

static quint32 qtMouseButtonToXButton( Qt::MouseButton button ) {
    switch ( button ) {
        case Qt::LeftButton: return 1;
        case Qt::MiddleButton: return 2;
        case Qt::RightButton: return 3;
        default: return 0;
    }
}

void KeyDialog::mouseReleaseEvent( QMouseEvent* event ) {
    if ( !m_acceptMouse ) {
        return;
    }
    m_value = qtMouseButtonToXButton( event->button() );
    m_isMouse = true;
    accept();
}

void KeyDialog::wheelEvent( QWheelEvent *event ) {
    if ( !m_acceptMouse ) {
        return;
    }

    const QPoint delta = event->angleDelta();
    const int x = delta.x();
    const int y = delta.y();

    if (y != 0) {
        if (y < 0) {
            m_value = 4;
        }
        else {
            m_value = 5;
        }
    }
    else if (x != 0) {
        if (x < 0) {
            m_value = 6;
        }
        else {
            m_value = 7;
        }
    }
    else {
        return;
    }

    m_isMouse = true;
    accept();
}
