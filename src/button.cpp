#include "button.h"
#include "event.h"

Button::Button( int i, QObject *parent ) : QObject(parent) {
    index = i;
    isButtonPressed = false;
    isDown = false;
    rapidfire = false;
    toDefault();
    tick = 0;
}

Button::~Button() {
    release();
}

bool Button::read( QTextStream &stream ) {
//	at this point, toDefault() has just been called.

    //read in a line of text and break it into words
    QString input = stream.readLine().toLower();
    QRegExp regex("[\\s,]+");
    QStringList words = input.split(regex);

    //used to assure correct conversion of QStrings -> ints
    bool ok;
    //used to receive converted ints from QStrings.
    int val;

    //go through every word on the line describing this button.
    for ( QStringList::Iterator it = words.begin(); it != words.end(); ++it ) {
        if (*it == "mouse") {
            ++it;
            if (it == words.end()) return false;
            val = (*it).toInt(&ok);
            if (ok && val >= 0 && val <= MAXKEY) {
                useMouse = true;
                keycode = val;
            }
            else return false;
        }
        else if (*it == "key") {
            ++it;
            if (it == words.end()) return false;
            val = (*it).toInt(&ok);
            if (ok && val >= 0 && val <= MAXKEY) {
                useMouse = false;
                keycode = val;
            }
            else return false;
        }
        else if (*it == "rapidfire") {
            rapidfire = true;
        }
        else if (*it == "sticky") {
            sticky = true;
        }
    }
    return true;
}

void Button::write( QTextStream &stream ) {
    stream << "\t" << getName() << ": ";
    if (rapidfire) stream << "rapidfire, ";
    if (sticky) stream << "sticky, ";
    stream << (useMouse ? "mouse " : "key ") << keycode << "\n";
}

void Button::release() {
    if (isDown) {
        click(false);
        isDown = true;
    }
}

void Button::jsevent( int value ) {
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
    timer.stop();
}

bool Button::isDefault() {
    return	(rapidfire == false) &&
           (sticky == false) &&
           (useMouse == false) &&
           (keycode == 0);
}

QString Button::status() {
    if (useMouse) {
        return QString("%1 : Mouse %2").arg(getName()).arg(keycode);
    }
    else {
        return QString("%1 : %2").arg(getName(), ktos(keycode));
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
