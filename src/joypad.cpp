#include "unistd.h"
#include "joypad.h"
#include <stdio.h>
#include <fcntl.h>
#include<stdint.h>
#include <poll.h>
#include <QApplication>

JoyPad::JoyPad( int i, int dev ) {
    debug_mesg("Constructing the joypad device with index %d and fd %d\n", i, dev);
    //remember the index,
    index = i;

    //load data from the joystick device, if available.
    joydevFileHandle = NULL;
    if(dev >= 0) {
        debug_mesg("Valid file handle, setting up handlers and reading axis configs...\n");
        resetToDev(dev);
        debug_mesg("done resetting and setting up device index %d\n", i);
        char id[256];
        memset(id, 0, sizeof(id));
        if (ioctl(joydev, JSIOCGNAME(sizeof(id)), id) < 0) {
            deviceId = "Unknown";
        }
        else {
            deviceId = id;
        }
    } else {
        debug_mesg("This joypad does not have a valid file handle, not setting up event listeners\n");
    }
    //there is no JoyPadWidget yet.
    jpw = NULL;
    debug_mesg("Done constructing the joypad device %d\n", i);
}

void JoyPad::resetToDev(int dev ) {
    debug_mesg("resetting to dev\n");
    //remember the device file descriptor
    joydev = dev;

    //read in the number of axes / buttons
    axes = 0;
    ioctl (joydev, JSIOCGAXES, &axes);
    buttons = 0;
    ioctl (joydev, JSIOCGBUTTONS, &buttons);
    //make sure that we have the axes we need.
    //if one that we need doesn't yet exist, add it in.
    //Note: if the current layout has a key assigned to an axis that did not
    //have a real joystick axis mapped to it, and this function suddenly brings
    //that axis into use, the key assignment will not be lost because the axis
    //will already exist and no new axis will be created.
    for (int i = 0; i < axes; i++) {
        if (Axes[i] == 0) Axes.insert(i, new Axis( i, this ));
    }
    for (int i = 0; i < buttons; i++) {
        if (Buttons[i] == 0) Buttons.insert(i, new Button( i, this ));
    }
    struct pollfd read_struct;
    read_struct.fd = joydev;
    read_struct.events = POLLIN;
    char buf[10];
    while(poll(&read_struct, 1, 5)!=0) {
        debug_mesg("reading junk data\n");
        if (read(joydev, buf, 10) <= 0) break;
    }
    setupJoyDeviceListener(dev);
    debug_mesg("done resetting to dev\n");
}

void JoyPad::setupJoyDeviceListener(int dev) {
    debug_mesg("Setting up joyDeviceListeners\n");
    joydevFileHandle = new QSocketNotifier(dev, QSocketNotifier::Read, this);
    connect(joydevFileHandle, SIGNAL(activated(int)), this, SLOT(handleJoyEvents(int)));
    joydevFileException = new QSocketNotifier(dev, QSocketNotifier::Exception, this);
    connect(joydevFileException, SIGNAL(activated(int)), this, SLOT(errorRead(int)));
    debug_mesg("Done setting up joyDeviceListeners\n");
}

void JoyPad::toDefault() {
    //to reset the whole, reset all the parts.
    foreach (Axis *axis, Axes) {
        axis->toDefault();
    }
    foreach (Button *button, Buttons) {
        button->toDefault();
    }
}

bool JoyPad::isDefault() {
    //if any of the parts are not at default, then the whole isn't either.
    foreach (Axis *axis, Axes) {
        if (!axis->isDefault()) return false;
    }
    foreach (Button *button, Buttons) {
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
                    error("Layout file error", QString("Expected ':', found '%1'.").arg(ch));
                    return false;
                }
                if (Buttons[num-1] == 0) {
                    Buttons.insert(num-1,new Button(num-1));
                }
                if (!Buttons[num-1]->read( stream )) {
                    error("Layout file error", QString("Error reading Button %1").arg(num));
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
                    error("Layout file error", QString("Expected ':', found '%1'.").arg(ch));
                    return false;
                }
                if (Axes[num-1] == 0) {
                    Axes.insert(num-1,new Axis(num-1));
                }
                if (!Axes[num-1]->read(stream)) {
                    error("Layout file error", QString("Error reading Axis %1").arg(num));
                    return false;
                }
            }
        }
        else {
            error( "Layout file error", QString("Error while reading layout. Unrecognized word: %1").arg(word) );
            return false;
        }
        stream >> word;
    }
    return true;
}

//only actually writes something if this JoyPad is NON DEFAULT.
void JoyPad::write( QTextStream &stream ) {
    if (!Axes.empty() || !Buttons.empty()) {
        stream << getName() << " {\n";
        foreach (Axis *axis, Axes) {
            axis->write(stream);
        }
        foreach (Button *button, Buttons) {
            button->write(stream);
        }
        stream << "}\n\n";
    }
}

void JoyPad::release() {
    foreach (Axis *axis, Axes) {
        axis->release();
    }
    foreach (Button *button, Buttons) {
        button->release();
    }
}

void JoyPad::jsevent( js_event msg ) {
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
    if (msg.type & JS_EVENT_AXIS) {
        debug_mesg("DEBUG: passing on an axis event\n");
        debug_mesg("DEBUG: %d %d\n", msg.number, msg.value);
        Axes[msg.number]->jsevent(msg.value);
    }
    else {
        debug_mesg("DEBUG: passing on a button event\n");
        debug_mesg("DEBUG: %d %d\n", msg.number, msg.value);
        Buttons[msg.number]->jsevent(msg.value);
    }
}

JoyPadWidget* JoyPad::widget( QWidget* parent, int i) {
    //create the widget and remember it.
    jpw = new JoyPadWidget(this, i, parent);
    return jpw;
}

void JoyPad::handleJoyEvents(int fd) {
    Q_UNUSED(fd);

    js_event msg;
    int len;

    len = read( joydev, &msg, sizeof(js_event));
    //if there was a real event waiting,
    if (len == (int) sizeof(js_event)) {
        //pass that event on to the joypad!
        jsevent(msg);
    }
}

void JoyPad::releaseWidget() {
    //this is how we know that there is no longer a JoyPadWidget around.
    jpw = NULL;
}

void JoyPad::unsetDev() {
    close(joydev);
    joydev = -1;
    if(joydevFileHandle != NULL) {
        delete joydevFileHandle;
    }
}

void JoyPad::errorRead(int fd) {
    debug_mesg("There was an error reading off of the device with fd %d, disabling\n", fd);
    joydevFileHandle->blockSignals(true);
    joydevFileHandle->setEnabled(false);
    close(joydev);
    if(disconnect(joydevFileHandle , 0, 0, 0)) {
        joydevFileHandle->deleteLater();
        joydevFileHandle = NULL;
    }
    if(disconnect(joydevFileException, 0, 0, 0)) {
        joydevFileException->setEnabled(false);
        joydevFileException->deleteLater();
        joydevFileException = NULL;
    }
    joydev = -1;
    debug_mesg("Done disabling device with fd %d\n", fd);
}

void JoyPad::focusChange(bool focusState) {
	hasFocus = !focusState;
}
