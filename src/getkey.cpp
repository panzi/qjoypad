#include <QX11Info>
#include "config.h"
#include "getkey.h"

GetKey::GetKey( QString button, bool m )
        :QDialog( 0 )
{
    //prepare the dialog
    mouse = m;
    setWindowTitle( "Choose a key" );
    setWindowIcon(QIcon(QJOYPAD_ICON24));

    //I'd use a QLabel, but that steals x11Events!
    //So, I'll draw the text directly. That means
    //I need to resolve the size of the dialog by hand:
    Text = "Choose a new key ";
    if (mouse) Text += "or mouse button ";
    Text += "for " + button;
    QRect rect = fontMetrics().boundingRect( Text );
    //I calculate the size based on the first line of text, which is longer.
    //The fontMetrics function is dumb and would treat the string with a
    //newline in it as a continues flow of characters if I did the whole string
    //at once.
    Text += "\n(Ctrl-X for no key)";
    //now I add 20 pixels of padding and double the height to make room for
    //two lines.
    setFixedSize( QSize( rect.width() + 20, rect.height()*2 + 20 ) );
}

bool GetKey::x11Event( XEvent* e )
{
    //keep Qt from closing the dialog upon seeing Esc pressed.
    if (e->type == KeyPress) return true;
    //On a key press, return the key and quit
    //Ctrl+X == [No Key]
    if (e->type == KeyRelease) {
        if (XKeycodeToKeysym(QX11Info::display(),e->xkey.keycode,0) == XK_x ) {
            if (e->xkey.state & ControlMask) done( 0 );
            else done( e->xkey.keycode );
        }
        else done( e->xkey.keycode );
        return true;
    }
    //if we're accepting mouse clicks and a mouse button was clicked...
    if (mouse && e->type == ButtonRelease) {
        done ( e->xbutton.button + MOUSE_OFFSET);
        return true;
    }

    //any other events we will pass on to the dialog. This allows for closing
    //the window and easy redrawing  :)
    return false;
}

void GetKey::closeEvent(QCloseEvent *e) {
    e->ignore();
    done(-1);
}

void GetKey::paintEvent ( QPaintEvent * ) {
    //whenever we need to repaint, draw in our text.
    QPainter paint( this );
    paint.drawText( rect(), Qt::AlignCenter, Text );
}
