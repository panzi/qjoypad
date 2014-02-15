#include "quickset.h"
#include "getkey.h"

//build the dialog
QuickSet::QuickSet( JoyPad* jp, QWidget *parent)
        : QDialog(parent) {
    setting = false;
    joypad = jp;
    setWindowTitle("Set " + jp->getName());
    QVBoxLayout* LMain = new QVBoxLayout(this);
    LMain->setMargin(5);
    LMain->setSpacing(5);
    //LMain->setAutoAdd(true);

    QLabel *temp = new QLabel("Press any button or axis and\nyou will be prompted for a key.",this);
    LMain->addWidget(temp);
    QPushButton* button = new QPushButton("Done",this);
    LMain->addWidget(button);
    connect( button, SIGNAL(clicked()), this, SLOT(accept()));
}

void QuickSet::jsevent( js_event msg ) {
    //ignore any joystick events if we're waiting for a keypress
    if (setting) return;

    //if a button was pressed on the joystick
    if (msg.type == JS_EVENT_BUTTON) {
        //capture that button.
        Button* button = joypad->Buttons[msg.number];

        //go into setting mode and request a key/mousebutton
        setting = true;
        int code = GetKey(button->getName(), true).exec();
        setting = false;

        //if a mouse button was used,
        if (code > MOUSE_OFFSET)
            //then tell it to the Button a mouse button
            button->setKey(true, code - MOUSE_OFFSET);
        else
            //otherwise, tell it to use a keycode.
            button->setKey(false, code);
    }
    else {
        //require a signal strength of at least 5000 to consider an axis moved.
        if (abs(msg.value) < 5000) return;

        //capture the axis that moved
        Axis* axis = joypad->Axes[msg.number];

        //grab a keycode for that axis and that direction
        setting = true;
        int code = GetKey(axis->getName() + ", " + QString((msg.value > 0)?"positive":"negative"), false).exec();
        setting = false;

        //assign the key to the axis.
        axis->setKey((msg.value > 0),code);
    }
}
