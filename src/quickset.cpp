#include "quickset.h"
#include "keydialog.hpp"

//build the dialog
QuickSet::QuickSet( JoyPad* jp, QWidget *parent)
        : QDialog(parent), joypad(jp), setting(false) {
    setWindowTitle(tr("Set %1").arg(jp->getName()));
    QVBoxLayout* LMain = new QVBoxLayout(this);
    LMain->setMargin(5);
    LMain->setSpacing(5);
    //LMain->setAutoAdd(true);

    QLabel *temp = new QLabel(tr("Press any button or axis and\nyou will be prompted for a key."),this);
    LMain->addWidget(temp);
    QPushButton* button = new QPushButton(tr("Done"),this);
    LMain->addWidget(button);
    connect( button, SIGNAL(clicked()), this, SLOT(accept()));
}

void QuickSet::jsevent(const js_event &msg ) {
    //ignore any joystick events if we're waiting for a keypress
    if (setting) return;

    //if a button was pressed on the joystick
    unsigned int type = msg.type & ~JS_EVENT_INIT;
    if (type == JS_EVENT_BUTTON) {
        //capture that button.
        Button* button = joypad->buttons[msg.number];

        if (button) {
            //go into setting mode and request a key/mousebutton
            setting = true;
            bool isMouse = false;
            int code = KeyDialog::getKey(button->getName(), true, &isMouse, this);
            setting = false;

            if (code >= 0) {
                button->setKey(isMouse, code);
            }
        }
    }
    else if (type == JS_EVENT_AXIS) {
        //require a signal strength of at least 5000 to consider an axis moved.
        if (abs(msg.value) < 5000) return;

        //capture the axis that moved
        Axis* axis = joypad->axes[msg.number];

        //grab a keycode for that axis and that direction
        setting = true;
        bool isMouse = false;
        int code = KeyDialog::getKey((msg.value >= 0 ? tr("%1, positive") : tr("%1, negative")).arg(axis->getName()), true, &isMouse, this);
        setting = false;

        //assign the key to the axis.
        if (code >= 0) {
            axis->setKey(isMouse, (msg.value > 0), code);
        }
    }
}
