#include "axis_edit.h"

AxisEdit::AxisEdit( Axis* ax )
	:QDialog() {
	axis = ax;
	setCaption("Set " + axis->getName());
	
	QVBoxLayout* v = new QVBoxLayout(this, 5, 5);
	
	QHBoxLayout* h = new QHBoxLayout();
		QVBoxLayout* v2 = new QVBoxLayout(0,5,5);
			CGradient = new QCheckBox("Gradient", this);
			CGradient->setChecked(axis->gradient);
			v2->addWidget(CGradient);
		
			CMode = new QComboBox(this);
			CMode->insertItem("Keyboard",keybd);
			CMode->insertItem("Mouse (Vert.)",mousepv);
			CMode->insertItem("Mouse (Vert. Rev.)", mousenv);
			CMode->insertItem("Mouse (Hor.)", mouseph);
			CMode->insertItem("Mouse (Hor. Rev.)", mousenh);
			CMode->setCurrentItem( axis->mode );
			connect(CMode, SIGNAL(activated(int)), this, SLOT( CModeChanged( int )));
			v2->addWidget(CMode);
		h->addLayout(v2);
	
		MouseBox = new QFrame(this);
		MouseBox->setFrameStyle( QFrame::Box | QFrame::Sunken );
		v2 = new QVBoxLayout(MouseBox,5,5);
		v2->setAutoAdd(true);
			new QLabel("Mouse Speed", MouseBox);
			SSpeed = new QSpinBox(0,MAXMOUSESPEED,1,MouseBox);
			SSpeed->setValue(axis->maxSpeed);
		h->addWidget(MouseBox);
	v->addLayout(h);

	Slider = new JoySlider(axis->dZone, axis->xZone, axis->state, this);
	v->addWidget(Slider);

	KeyBox = new QFrame(this);
	KeyBox->setFrameStyle( QFrame::Box | QFrame::Sunken );
	h = new QHBoxLayout(KeyBox, 5, 5);
	h->setAutoAdd(true);
		BNeg = new KeyButton(axis->getName(),axis->nkeycode,KeyBox);
		
		CThrottle = new QComboBox(KeyBox);
		CThrottle->insertItem("Neg. Throttle",0);
		CThrottle->insertItem("No Throttle",1);
		CThrottle->insertItem("Pos. Throttle",2);
		CThrottle->setCurrentItem(axis->throttle + 1);
		connect( CThrottle, SIGNAL( activated( int )), this, SLOT( CThrottleChanged( int )));
		
		BPos = new KeyButton(axis->getName(),axis->pkeycode,KeyBox);
	v->addWidget( KeyBox );
	
	h = new QHBoxLayout();
	BOkay = new QPushButton("Okay", this);
	connect(BOkay, SIGNAL( clicked() ), this, SLOT( accept()));
	h->addWidget(BOkay);
	BCancel = new QPushButton("Cancel", this);
	connect(BCancel, SIGNAL( clicked() ), this, SLOT( reject()));
	h->addWidget(BCancel);
	v->addLayout(h);
	
	CModeChanged( axis->mode );
	CThrottleChanged( axis->throttle + 1 );
}

void AxisEdit::show() {
	QDialog::show();
	setFixedSize(size());
}

void AxisEdit::setState( int val ) {
	Slider->setValue( val );
}

void AxisEdit::CModeChanged( int index ) {
	if (index == keybd) {
		MouseBox->setEnabled(false);
		KeyBox->setEnabled(true);
	}
	else {
		MouseBox->setEnabled(true);
		KeyBox->setEnabled(false);
	}
}

void AxisEdit::CThrottleChanged( int index ) {
	switch (index) {
		case 0:	BNeg->setEnabled(true);
				BPos->setEnabled(false);
				break;
		case 1: BNeg->setEnabled(true);
				BPos->setEnabled(true);
				break;
		case 2: BNeg->setEnabled(false);
				BPos->setEnabled(true);
				break;
	}
	Slider->setThrottle( index - 1 );
}

void AxisEdit::accept() {
	if (CGradient->isChecked()) {
		axis->gradient = true;
		axis->startTimer(MSEC);
	}
	else {
		axis->gradient = false;
	}
	axis->maxSpeed = SSpeed->value();
	axis->throttle = CThrottle->currentItem() - 1;
	axis->dZone = Slider->dZone();
	axis->xZone = Slider->xZone();
	axis->mode = (AxisMode) CMode->currentItem();
	axis->pkeycode = BPos->getValue();
	axis->nkeycode = BNeg->getValue();

	QDialog::accept();
}
