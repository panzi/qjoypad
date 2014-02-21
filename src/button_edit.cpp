#include "config.h"
#include "button_edit.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

ButtonEdit::ButtonEdit(Button* butt)
        : QDialog(0) {
    setModal(true);
    //build the dialog!
    button = butt;
    setWindowTitle("Set " + button->getName());
    setWindowIcon(QPixmap(QJOYPAD_ICON24));

    QVBoxLayout* v = new QVBoxLayout(this);
    v->setMargin(5);
    v->setSpacing(5);

    btnKey = new KeyButton( button->getName(), button->keycode, this, true, button->useMouse);
    v->addWidget(btnKey);

    QHBoxLayout* h = new QHBoxLayout();
    chkSticky = new QCheckBox("&Sticky", this);
    chkSticky->setChecked(button->sticky);
    h->addWidget(chkSticky);
    chkRapid = new QCheckBox("&Rapid Fire", this);
    chkRapid->setChecked(button->rapidfire);
    h->addWidget(chkRapid);
    v->addLayout(h);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    v->addWidget(buttonBox);
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
    button->rapidfire = chkRapid->isChecked();
    button->sticky = chkSticky->isChecked();
    //if the user chose a mouse button...
    button->useMouse = btnKey->choseMouse();
    button->keycode = btnKey->getValue();

    QDialog::accept();
}
