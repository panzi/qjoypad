#include "config.h"
#include "button_edit.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

ButtonEdit::ButtonEdit(Button* butt, const QStringList *layoutNames)
        : QDialog(0) {
    setModal(true);
    //build the dialog!
    button = butt;
    setWindowTitle(tr("Set %1").arg(button->getName()));
    setWindowIcon(QPixmap(QJOYPAD_ICON24));

    QVBoxLayout* v = new QVBoxLayout(this);
    v->setMargin(5);
    v->setSpacing(5);

    btnKey = new KeyButton( button->getName(), button->keycode, this, true, button->useMouse);
    v->addWidget(btnKey);

    QHBoxLayout* h = new QHBoxLayout();
    chkSticky = new QCheckBox(tr("&Sticky"), this);
    chkSticky->setChecked(button->sticky);
    h->addWidget(chkSticky);
    chkRapid = new QCheckBox(tr("&Rapid Fire"), this);
    chkRapid->setChecked(button->rapidfire);
    h->addWidget(chkRapid);
    v->addLayout(h);

    cmbLayout = new QComboBox(this);
    cmbLayout->addItem(tr("[UNSET]"), QVariant(QString()));
    cmbLayout->addItem(tr("[NO LAYOUT]"), QVariant(QString()));
    foreach (const QString& layout, *layoutNames) {
        cmbLayout->addItem(layout, layout);
    }
    //Keep selected layout (if any)
    if (button->hasLayout) {
        cmbLayout->setCurrentIndex(layoutNames->indexOf(button->layout) + 2);
    }
    else {
        cmbLayout->setCurrentIndex(0);
    }
    v->addWidget(cmbLayout);

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
    if (cmbLayout->currentIndex() != 0) {
        button->hasLayout = true;
        button->layout = cmbLayout->currentText();
    }
    else {
        button->hasLayout = false;
    }

    QDialog::accept();
}
