#include "button_edit.h"

ButtonEdit::ButtonEdit(Button* butt)
	:QDialog(0,0,true) {
	button = butt;
	setCaption("Set " + button->getName());
	
	QVBoxLayout* v = new QVBoxLayout(this, 5, 5);

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
	if (CRapid->isChecked()) {
		button->rapidfire = true;
		button->startTimer(MSEC);
	}
	else {
		button->rapidfire = false;
	}
	button->sticky = (CSticky->isChecked());
//	if (BMouse->isOn()) {
//		button->useMouse = true;
//		for (int i = 0; i < MAXBUTTON; i++) {
//			if (BMKey[i]->isOn()) {
//				button->mousecode = i+1;
//			}
//		}
//	}
//	else {
		if (BKKey->getMouse()) {
			button->useMouse = true;
			button->mousecode = BKKey->getValue();
		}
		else {
			button->useMouse = false;
			button->keycode = BKKey->getValue();
		}
//	}

	QDialog::accept();
}
