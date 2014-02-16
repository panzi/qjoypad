#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <QApplication>

#include "joypad.h"

JoyPad::JoyPad( int i, int dev, QObject *parent )
    : QObject(parent), joydev(-1), axisCount(0), buttonCount(0), jpw(0), readNotifier(0), errorNotifier(0) {
    debug_mesg("Constructing the joypad device with index %d and fd %d\n", i, dev);
    //remember the index,
    index = i;

    //load data from the joystick device, if available.
    if (dev >= 0) {
        debug_mesg("Valid file handle, setting up handlers and reading axis configs...\n");
        open(dev);
        debug_mesg("done resetting and setting up device index %d\n", i);
    } else {
        debug_mesg("This joypad does not have a valid file handle, not setting up event listeners\n");
    }
    debug_mesg("Done constructing the joypad device %d\n", i);
}

JoyPad::~JoyPad() {
    close();
}

void JoyPad::close() {
    if (readNotifier) {
        disconnect(readNotifier, 0, 0, 0);

        readNotifier->blockSignals(true);
        readNotifier->setEnabled(false);

        delete readNotifier;
        readNotifier = 0;
    }
    if (errorNotifier) {
        disconnect(errorNotifier, 0, 0, 0);

        errorNotifier->blockSignals(true);
        errorNotifier->setEnabled(false);

        delete errorNotifier;
        errorNotifier = 0;
    }
    if (joydev >= 0) {
        if (::close(joydev) != 0) {
            debug_mesg("close(js%d %d): %s\n", index, joydev, strerror(errno));
        }
        joydev = -1;
    }
}

void JoyPad::open(int dev) {
    debug_mesg("resetting to dev\n");
    //remember the device file descriptor
    close();
    joydev = dev;

    char id[256];
    memset(id, 0, sizeof(id));
    if (ioctl(joydev, JSIOCGNAME(sizeof(id)), id) < 0) {
        deviceId = "Unknown";
    }
    else {
        deviceId = id;
    }

    //read in the number of axes / buttons
    axisCount = 0;
    ioctl (joydev, JSIOCGAXES, &axisCount);
    buttonCount = 0;
    ioctl (joydev, JSIOCGBUTTONS, &buttonCount);
    //make sure that we have the axes we need.
    //if one that we need doesn't yet exist, add it in.
    //Note: if the current layout has a key assigned to an axis that did not
    //have a real joystick axis mapped to it, and this function suddenly brings
    //that axis into use, the key assignment will not be lost because the axis
    //will already exist and no new axis will be created.
    for (int i = 0; i < axisCount; i++) {
        if (axes[i] == 0) axes.insert(i, new Axis( i, this ));
    }
    for (int i = 0; i < buttonCount; i++) {
        if (buttons[i] == 0) buttons.insert(i, new Button( i, this ));
    }
    debug_mesg("Setting up joyDeviceListeners\n");
    readNotifier = new QSocketNotifier(joydev, QSocketNotifier::Read, this);
    connect(readNotifier, SIGNAL(activated(int)), this, SLOT(handleJoyEvents()));
    errorNotifier = new QSocketNotifier(joydev, QSocketNotifier::Exception, this);
    connect(errorNotifier, SIGNAL(activated(int)), this, SLOT(handleJoyEvents()));
    debug_mesg("Done setting up joyDeviceListeners\n");
    debug_mesg("done resetting to dev\n");
}

void JoyPad::toDefault() {
    //to reset the whole, reset all the parts.
    foreach (Axis *axis, axes) {
        axis->toDefault();
    }
    foreach (Button *button, buttons) {
        button->toDefault();
    }
}

bool JoyPad::isDefault() {
    //if any of the parts are not at default, then the whole isn't either.
    foreach (Axis *axis, axes) {
        if (!axis->isDefault()) return false;
    }
    foreach (Button *button, buttons) {
        if (!button->isDefault()) return false;
    }
    return true;
}

bool JoyPad::readConfig( QTextStream &stream ) {
    toDefault();

    QString word;
    QChar ch = 0;
    int num = 0;

    stream >> word;
    while (!word.isNull() && word != "}") {
        word = word.toLower();
        if (word == "button") {
            stream >> num;
            if (num > 0) {
                stream >> ch;
                if (ch != ':') {
                    errorBox("Layout file error", QString("Expected ':', found '%1'.").arg(ch));
                    return false;
                }
                if (buttons[num-1] == 0) {
                    buttons.insert(num-1,new Button(num-1,this));
                }
                if (!buttons[num-1]->read( stream )) {
                    errorBox("Layout file error", QString("Error reading Button %1").arg(num));
                    return false;
                }
            }
            else {
                stream.readLine();
            }
        }
        else if (word == "axis") {
            stream >> num;
            if (num > 0) {
                stream >> ch;
                if (ch != ':') {
                    errorBox("Layout file error", QString("Expected ':', found '%1'.").arg(ch));
                    return false;
                }
                if (axes[num-1] == 0) {
                    axes.insert(num-1,new Axis(num-1,this));
                }
                if (!axes[num-1]->read(stream)) {
                    errorBox("Layout file error", QString("Error reading Axis %1").arg(num));
                    return false;
                }
            }
        }
        else {
            errorBox( "Layout file error", QString("Error while reading layout. Unrecognized word: %1").arg(word) );
            return false;
        }
        stream >> word;
    }
    return true;
}

//only actually writes something if this JoyPad is NON DEFAULT.
void JoyPad::write( QTextStream &stream ) {
    if (!axes.empty() || !buttons.empty()) {
        stream << "Joystick " << index << " {\n";
        foreach (Axis *axis, axes) {
            axis->write(stream);
        }
        foreach (Button *button, buttons) {
            button->write(stream);
        }
        stream << "}\n\n";
    }
}

void JoyPad::release() {
    foreach (Axis *axis, axes) {
        axis->release();
    }
    foreach (Button *button, buttons) {
        button->release();
    }
}

void JoyPad::jsevent(const js_event &msg) {
    //if there is a JoyPadWidget around, ie, if the joypad is being edited
    if (jpw != NULL && hasFocus) {
        //tell the dialog there was an event. It will use this to flash
        //the appropriate button, if necesary.
        jpw->jsevent(msg);
        return;
    }
    //if the dialog is open, stop here. We don't want to signal ourselves with
    //the input we generate.
    if (qApp->activeWindow() != 0 && qApp->activeModalWidget() != 0) return;

    //otherwise, lets create us a fake event! Pass on the event to whichever
    //Button or Axis was pressed and let them decide what to do with it.
    unsigned int type = msg.type & ~JS_EVENT_INIT;
    if (type == JS_EVENT_AXIS) {
        debug_mesg("DEBUG: passing on an axis event\n");
        debug_mesg("DEBUG: %d %d\n", msg.number, msg.value);
        Axis *axis = axes[msg.number];
        if (axis) axis->jsevent(msg.value);
    }
    else if (type == JS_EVENT_BUTTON) {
        debug_mesg("DEBUG: passing on a button event\n");
        debug_mesg("DEBUG: %d %d\n", msg.number, msg.value);
        Button *button = buttons[msg.number];
        if (button) button->jsevent(msg.value);
    }
}

JoyPadWidget* JoyPad::widget( QWidget* parent, int i) {
    //create the widget and remember it.
    jpw = new JoyPadWidget(this, i, parent);
    return jpw;
}

void JoyPad::handleJoyEvents() {
    js_event msg;
    ssize_t len = read(joydev, &msg, sizeof(js_event));
    //if there was a real event waiting,
    if (len == sizeof(js_event)) {
        //pass that event on to the joypad!
        jsevent(msg);
    }
}

void JoyPad::releaseWidget() {
    //this is how we know that there is no longer a JoyPadWidget around.
    jpw = NULL;
}

void JoyPad::errorRead() {
    debug_mesg("There was an error reading off of the device with fd %d, disabling\n", joydev);
    close();
    debug_mesg("Done disabling device with fd %d\n", joydev);
}

void JoyPad::focusChange(bool focusState) {
	hasFocus = !focusState;
}
