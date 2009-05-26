#include "button_edit.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

ButtonEdit::ButtonEdit(Button* butt)
        :QDialog(0) {
    setModal(true);
    //build the dialog!
    button = butt;
    setWindowTitle("Set " + button->getName());
    setWindowIcon(QPixmap(ICON24));

    QVBoxLayout* v = new QVBoxLayout(this);
    v->setMargin(5);
    v->setSpacing(5);

    BKKey = new KeyButton( button->getName(), button->useMouse?button->mousecode:button->keycode, this, true, button->useMouse);
    v->addWidget(BKKey);

    QHBoxLayout* h = new QHBoxLayout();
    CSticky = new QCheckBox("Sticky", this);
    CSticky->setChecked(button->sticky);
    h->addWidget(CSticky);
    CRapid = new QCheckBox("Rapid Fire", this);
    CRapid->setChecked(button->rapidfire);
    h->addWidget(CRapid);
    v->addLayout(h);

    h = new QHBoxLayout();
    BOkay = new QPushButton("Okay", this);
    connect(BOkay, SIGNAL( clicked() ), this, SLOT( accept()));
    h->addWidget(BOkay);
    BCancel = new QPushButton("Cancel", this);
    connect(BCancel, SIGNAL( clicked() ), this, SLOT( reject()));
    h->addWidget(BCancel);
    v->addLayout(h);
}

void ButtonEdit::show() {
    QDialog::show();
    setFixedSize(size());
}

void ButtonEdit::accept() {
//if the rapidfire status has changed, either request a timer or turn it down.
    /*if (button->rapidfire) {
    	if (!CRapid->isChecked()) tossTimer(button);
    }
    else {
    	if (CRapid->isChecked()) takeTimer(button);
    }*/
    button->rapidfire = CRapid->isChecked();
    button->sticky = (CSticky->isChecked());
    //if the user chose a mouse button...
    if (BKKey->choseMouse()) {
        button->useMouse = true;
        button->mousecode = BKKey->getValue();
    }
    else {
        button->useMouse = false;
        button->keycode = BKKey->getValue();
    }

    QDialog::accept();
}
