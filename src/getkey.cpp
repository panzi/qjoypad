#include <QX11Info>
#include "config.h"
#include "getkey.h"

int GetKey::getKey(const QString& button, bool useMouse, bool *isMouse, QWidget *parent) {
    GetKey getkey(button, useMouse, parent);
    if (getkey.exec() == QDialog::Accepted) {
        if (isMouse) {
            *isMouse = getkey.choseMouse();
        }
        return getkey.getValue();
    }
    return -1;
}


GetKey::GetKey(const QString &button, bool useMouse, QWidget *parent)
        : QDialog( parent ), useMouse(useMouse), wasMouse(false), value(-1)
{
    //prepare the dialog
    setWindowTitle( tr("Choose a key") );
    setWindowIcon(QIcon(QJOYPAD_ICON24));

    //I'd use a QLabel, but that steals x11Events!
    //So, I'll draw the text directly. That means
    //I need to resolve the size of the dialog by hand:
    text = (useMouse ? tr("Choose a new key or mouse button for %1") : tr("Choose a new key for %1")).arg(button);
    QRect rect = fontMetrics().boundingRect( text );
    //I calculate the size based on the first line of text, which is longer.
    //The fontMetrics function is dumb and would treat the string with a
    //newline in it as a continues flow of characters if I did the whole string
    //at once.
    text += tr("\n(Ctrl-X for no key)");
    //now I add 20 pixels of padding and double the height to make room for
    //two lines.
    setFixedSize(rect.width() + 20, rect.height()*2 + 20);
}

bool GetKey::x11Event( XEvent* e )
{
    //keep Qt from closing the dialog upon seeing Esc pressed.
    if (e->type == KeyPress) return true;
    //On a key press, return the key and quit
    //Ctrl+X == [No Key]
    if (e->type == KeyRelease) {
        wasMouse = false;
        if (XKeycodeToKeysym(QX11Info::display(),e->xkey.keycode,0) == XK_x) {
            if (e->xkey.state & ControlMask) {
                value = 0;
                accept();
            }
            else {
                value = e->xkey.keycode;
                accept();
            }
        }
        else {
            value = e->xkey.keycode;
            accept();
        }
        return true;
    }
    //if we're accepting mouse clicks and a mouse button was clicked...
    if (useMouse && e->type == ButtonRelease) {
        wasMouse = true;
        value = e->xbutton.button;
        accept();
        return true;
    }

    //any other events we will pass on to the dialog. This allows for closing
    //the window and easy redrawing  :)
    return false;
}

void GetKey::paintEvent ( QPaintEvent * ) {
    //whenever we need to repaint, draw in our text.
    QPainter paint( this );
    paint.drawText( rect(), Qt::AlignCenter, text );
}
