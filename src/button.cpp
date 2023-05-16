#include "button.h"
#include "event.h"

Button::Button( int i, QObject *parent ) : QObject(parent) {
    index = i;
    isButtonPressed = false;
    isDown = false;
    rapidfire = false;
    hasLayout = false;
    toDefault();
    tick = 0;
}

Button::~Button() {
    release();
}

bool Button::read( QTextStream &stream ) {
//	at this point, toDefault() has just been called.

    //read in a line of text and break it into words
    QString input = stream.readLine();
    QRegExp regex("[\\s,]+");
    QStringList words = input.split(regex);

    //used to assure correct conversion of QStrings -> ints
    bool ok;
    //used to receive converted ints from QStrings.
    int val;

    //go through every word on the line describing this button.
    for ( QStringList::Iterator it = words.begin(); it != words.end(); ++it ) {
        if (QString::compare(*it, "mouse", Qt::CaseInsensitive) == 0) {
            ++it;
            if (it == words.end()) return false;
            val = (*it).toInt(&ok);
            if (ok && val >= 0 && val <= MAXKEY) {
                useMouse = true;
                keycode = val;
            }
            else return false;
        }
        else if (QString::compare(*it, "key", Qt::CaseInsensitive) == 0) {
            ++it;
            if (it == words.end()) return false;
            val = (*it).toInt(&ok);
            if (ok && val >= 0 && val <= MAXKEY) {
                useMouse = false;
                keycode = val;
            }
            else return false;
        }
        else if (QString::compare(*it, "layout", Qt::CaseInsensitive) == 0) {
            ++it;
            if (it == words.end()) return false;
            layout = (*it).replace("\\s", " ");
            hasLayout = true;
        }
        else if (QString::compare(*it, "rapidfire", Qt::CaseInsensitive) == 0) {
            rapidfire = true;
        }
        else if (QString::compare(*it, "sticky", Qt::CaseInsensitive) == 0) {
            sticky = true;
        }
    }
    return true;
}

void Button::write( QTextStream &stream ) {
    stream << "\tButton " << (index+1) << ": ";
    if (rapidfire) stream << "rapidfire, ";
    if (sticky) stream << "sticky, ";
    stream << (useMouse ? "mouse " : "key ") << keycode;
    if (hasLayout) stream << " layout " << layout.replace(" ", "\\s");
    stream << "\n";
}

void Button::release() {
    if (isDown) {
        click(false);
        isDown = true;
    }
}

void Button::jsevent( int value ) {
    if (hasLayout) {
        if (value == 1 && !isButtonPressed) {
            isButtonPressed = 1;
            //Change layout
            emit loadLayout(layout);
        }
        else if (value == 0) {
            isButtonPressed = 0;
        }
        return;
    }

    bool newval = (value == 1);
    if (sticky) {
        //the state of a sticky key only changes on button press, not button release.
        if (value == 1) {
            isButtonPressed = !isButtonPressed;
        }
        else return;
    }
    //if the received event indicates a change in state,
    else if (newval != isButtonPressed) {
        isButtonPressed = newval; //change state
        if (isButtonPressed && rapidfire) {
            tick = 0;
            connect(&timer, SIGNAL(timeout()), this, SLOT(timerCalled()));
            timer.start(MSEC);
        }
        if (!isButtonPressed && rapidfire) {
            timer.stop();
            disconnect(&timer, SIGNAL(timeout()), 0, 0);
            if(isDown) {
                click(false);
            }
            tick = 0;
        }
    }
    //otherwise... we don't care. This shouldn't happen.
    else return;
    //if rapidfire is on, then timer() will do its job. Otherwise we must
    //manually triger the key event.
    if (!rapidfire) {
        click(isButtonPressed);
    }
}

void Button::toDefault() {
    rapidfire = false;
    sticky = false;
    useMouse = false;
    keycode = 0;
    hasLayout = false;
    timer.stop();
}

bool Button::isDefault() {
    return	(rapidfire == false) &&
           (sticky == false) &&
           (useMouse == false) &&
           (keycode == 0) &&
           (hasLayout == false);
}

QString Button::getName() {
    return tr("Button %1").arg(index+1);
}

QString Button::status() {
    if (hasLayout) {
        return tr("%1 : %2").arg(getName(), layout);
    }
    else if (useMouse) {
        return tr("%1 : Mouse %2").arg(getName()).arg(keycode);
    }
    else {
        return tr("%1 : %2").arg(getName(), ktos(keycode));
    }
}

void Button::setKey( bool mouse, int value ) {
    useMouse = mouse;
    keycode = value;
}

void Button::timerTick( int tick ) {
    if (isButtonPressed) {
        //originally I just clicked true and then false right after, but this
        //was not recognized by some programs. I need a delay in between.
        if (tick % FREQ == 0) {
            click(true);
        }
        if (tick % FREQ == FREQ / 2) {
            click(false);
        }
    }
}

void Button::click( bool press ) {
    if (isDown == press) return;
    isDown = press;
    FakeEvent click;
    //determine which of the four possible events we're sending.
    if (press) click.type = useMouse ? FakeEvent::MouseDown : FakeEvent::KeyDown;
    else click.type = useMouse ? FakeEvent::MouseUp : FakeEvent::KeyUp;
    //set up the event,
    click.keycode = keycode;
    //and send it.
    sendevent(click);
}

void Button::timerCalled() {
    timerTick(++tick);
}
