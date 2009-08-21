#include "unistd.h"
#include "joypad.h"
#include <stdio.h>
#include <fcntl.h>
#include<stdint.h>
#include <poll.h>
#include <QApplication>
JoyPad::JoyPad( int i, int dev ) {
    DEBUG("Constructing the joypad device\n");
    //remember the index,
    index = i;

    //load data from the joystick device, if available.
    joydevFileHandle = NULL;
    if(dev > 0) {
        DEBUG("Valid file handle, setting up handlers and reading axis configs...\n");
        resetToDev(dev);
        DEBUG("done resetting and setting up\n");
    }
    //there is no JoyPadWidget yet.
    jpw = NULL;
    DEBUG("Done constructing the joypad device\n");
}

void JoyPad::resetToDev(int dev ) {
    DEBUG("resetting to dev\n");
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
        if (Axes[i] == 0) Axes.insert(i, new Axis( i ));
    }
    for (int i = 0; i < buttons; i++) {
        if (Buttons[i] == 0) Buttons.insert(i, new Button( i ));
    }
    struct pollfd read_struct;
    read_struct.fd = joydev;
    read_struct.events = POLLIN;
    char buf[10];
    while(poll(&read_struct, 1, 5)!=0) {
        DEBUG("reading junk data\n");
        read(joydev, buf, 10);
    }
    setupJoyDeviceListener(dev);
    DEBUG("done resetting to dev\n");
}

void JoyPad::setupJoyDeviceListener(int dev) {
    DEBUG("Setting up joyDeviceListeners\n");
    joydevFileHandle = new QSocketNotifier(dev, QSocketNotifier::Read, this);
    connect(joydevFileHandle, SIGNAL(activated(int)), this, SLOT(handleJoyEvents(int)));
    joydevFileException = new QSocketNotifier(dev, QSocketNotifier::Exception, this);
    connect(joydevFileException, SIGNAL(activated(int)), this, SLOT(errorRead(int)));
    DEBUG("Done setting up joyDeviceListeners\n");
}

void JoyPad::toDefault() {
    //to reset the whole, reset all the parts.
    do
    {
        QHashIterator<int, Axis*> it( Axes );
        while (it.hasNext())
        {
            it.next();
            it.value()->toDefault();
        }
    } while (0);
    do
    {
        QHashIterator<int, Button*> it( Buttons );
        while (it.hasNext())
        {
            it.next();
            it.value()->toDefault();
        }
    } while (0);
}

bool JoyPad::isDefault() {
    //if any of the parts are not at default, then the whole isn't either.
    do {
        QHashIterator<int, Axis*> it( Axes );
        while (it.hasNext())
        {
            it.next();
            if (!it.value()->isDefault()) return false;
        }
    } while (0);
    do {
        QHashIterator<int, Button*> it( Buttons );
        while (it.hasNext())
        {
            it.next();
            if (!it.value()->isDefault()) return false;
        }
    } while (0);
    return true;
}

bool JoyPad::readConfig( QTextStream* stream ) {
    toDefault();

    QString word;
    QChar dump;
    int num;

    *stream >> word;
    while (word != NULL && word != "}") {
        word = word.toLower();
        if (word == "button") {
            *stream >> num;
            if (num > 0) {
                *stream >> dump;
                if (dump != ':') {
                    error("Layout file error", "Expected ':', found '" + QString(dump) + "'.");
                    return false;
                }
                if (Buttons[num-1] == 0) {
                    Buttons.insert(num-1,new Button(num-1));
                }
                if (!Buttons[num-1]->read( stream )) {
                    error("Layout file error", "Error reading Button " + QString::number(num));
                    return false;
                }
            }
            else {
                stream->readLine();
            }
        }
        else if (word == "axis") {
            *stream >> num;
            if (num > 0) {
                *stream >> dump;
                if (dump != ':') {
                    error("Layout file error", "Expected ':', found '" + QString(dump) + "'.");
                    return false;
                }
                if (Axes[num-1] == 0) {
                    Axes.insert(num-1,new Axis(num-1));
                }
                if (!Axes[num-1]->read(stream)) {
                    error("Layout file error", "Error reading Axis " + QString::number(num));
                    return false;
                }
            }
        }
        else {
            error( "Layout file error", "Error while reading layout. Unrecognized word: " + word );
            return false;
        }
        *stream >> word;
    }
    return true;
}

//only actually writes something if this JoyPad is NON DEFAULT.
void JoyPad::write( QTextStream* stream ) {
    int i = 0; //use to test if this is default or not.
    QString result;
    QTextStream* s = new QTextStream(&result, QIODevice::WriteOnly);
    *s << getName() << " {\n";
    do {
        QHashIterator<int, Axis*> it( Axes );
        while (it.hasNext()) {
            it.next();
            if (!it.value()->isDefault()) {
                it.value()->write( s );
                ++i;
            }
        }
    } while (0);
    QHashIterator<int, Button*> it( Buttons );
    while (it.hasNext()) {
        it.next();
        if (!it.value()->isDefault()) {
            it.value()->write( s );
            ++i;
        }
    }

    if (i > 0) {
        *stream << result << "}\n\n";
    }
}

void JoyPad::release() {
    do {
        QHashIterator<int, Axis*> it( Axes );
        while (it.hasNext()) {
            it.next();
            it.value()->release();
        }
    } while (0);
    do {
        QHashIterator<int, Button*> it( Buttons );
        while (it.hasNext()) {
            it.next();
            it.value()->release();
        }
    } while (0);
}

void JoyPad::jsevent( js_event msg ) {
    //if there is a JoyPadWidget around, ie, if the joypad is being edited
    if (jpw != NULL) {
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
        //printf("DEBUG: passing on an axis event\n");
        //printf("DEBUG: %d %d\n", msg.number, msg.value);
        Axes[msg.number]->jsevent(msg.value);
    }
    else {
        //printf("DEBUG: passing on a button event\n");
        //printf("DEBUG: %d %d\n", msg.number, msg.value);
        Buttons[msg.number]->jsevent(msg.value);
    }
}

JoyPadWidget* JoyPad::widget( QWidget* parent, int i) {
    //create the widget and remember it.
    jpw = new JoyPadWidget(this, i, parent);
    return jpw;
}

void JoyPad::handleJoyEvents(int fd) {
    js_event msg;
    int len;

    len = read( joydev, &msg, sizeof(js_event));
    //if there was a real event waiting,
    if (len == (int) sizeof(js_event)) {
        //pass that event on to the joypad!
        jsevent(msg);
    }

    //sleep for a moment. This is just to keep us from throwing all the
    //available processer power into madly checking for new events.
    //usleep(10000);
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
    DEBUG("There was an error reading off of the device, disabling\n");
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
    DEBUG("Done disabling\n");
}
