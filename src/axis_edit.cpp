#include "config.h"
#include "axis_edit.h"


AxisEdit::AxisEdit( Axis* ax )
        :QDialog() {
    //build the dialog, display current axis settings  :)
    axis = ax;
    setWindowTitle("Set " + axis->getName());
    setWindowIcon(QPixmap(QJOYPAD_ICON24));

    //h, v, and v2 are all references to layouts. They are used to refer to
    //various layouts as the dialog is built and are not pointing to the same
    //thing throughout. This is just because I don't care about the layouts
    //after I have placed the widgets within them and there's no reasno to
    //keep track of them.

    QVBoxLayout* v = new QVBoxLayout(this);
    v->setMargin(5);
    v->setSpacing(5);

    QHBoxLayout* h = new QHBoxLayout();
    QVBoxLayout* v2 = new QVBoxLayout();
    v2->setMargin(5);
    v2->setSpacing(5);
    CGradient = new QCheckBox("&Gradient", this);
    CGradient->setChecked(axis->gradient);
    connect(CGradient, SIGNAL(toggled(bool)), this, SLOT( CGradientChanged( bool )));
    v2->addWidget(CGradient);

    CMode = new QComboBox(this);
    CMode->insertItem((int) Axis::Keyboard, QString("Keyboard/Mouse Button"), Qt::DisplayRole);
    CMode->insertItem((int) Axis::MousePosVert,QString("Mouse (Vert.)"),Qt::DisplayRole);
    CMode->insertItem((int) Axis::MouseNegVert, QString("Mouse (Vert. Rev.)"), Qt::DisplayRole);
    CMode->insertItem((int) Axis::MousePosHor, "Mouse (Hor.)", Qt::DisplayRole);
    CMode->insertItem((int) Axis::MouseNegHor, QString("Mouse (Hor. Rev.)"), Qt::DisplayRole);
    CMode->setCurrentIndex( axis->mode );
    connect(CMode, SIGNAL(activated(int)), this, SLOT( CModeChanged( int )));
    v2->addWidget(CMode);
	CTransferCurve = new QComboBox(this);
    CTransferCurve->insertItem(Axis::Linear, QString("Linear"), Qt::DisplayRole);
    CTransferCurve->insertItem(Axis::Quadratic, QString("Quadratic"),Qt::DisplayRole );
    CTransferCurve->insertItem(Axis::Cubic, QString("Cubic"),Qt::DisplayRole );
    CTransferCurve->insertItem(Axis::QuadraticExtreme, QString("Quadratic Extreme"), Qt::DisplayRole);
    CTransferCurve->insertItem(Axis::PowerFunction, QString("Power Function"), Qt::DisplayRole);
    CTransferCurve->setCurrentIndex( axis->transferCurve );
    CTransferCurve->setEnabled(axis->gradient);
	connect(CTransferCurve, SIGNAL(activated(int)), this, SLOT( CTransferCurveChanged( int )));
	v2->addWidget(CTransferCurve);
    h->addLayout(v2);

    MouseBox = new QFrame(this);
    MouseBox->setFrameStyle( QFrame::Box | QFrame::Sunken );
    v2 = new QVBoxLayout(MouseBox);
    v2->setSpacing(5);
    v2->setMargin(5);
    //v2->setAutoAdd(true);
    QLabel *mouseLabel = new QLabel("&Mouse Speed", MouseBox);
    v2->addWidget(mouseLabel);
    SSpeed = new QSpinBox(MouseBox);
    SSpeed->setRange(0,MAXMOUSESPEED);
    SSpeed->setSingleStep(1);
    SSpeed->setValue(axis->maxSpeed);
    v2->addWidget(SSpeed);
    LSensitivity = new QLabel("&Sensitivity", MouseBox);
    v2->addWidget(LSensitivity);
	SSensitivity = new QDoubleSpinBox(MouseBox);
    SSensitivity->setRange(1e-3F, 1e+3F);
    SSensitivity->setSingleStep(0.10);
	SSensitivity->setValue(axis->sensitivity);
    v2->addWidget(SSensitivity);
    h->addWidget(MouseBox);
    mouseLabel->setBuddy(SSpeed);
    LSensitivity->setBuddy(SSensitivity);
    v->addLayout(h);

    Slider = new JoySlider(axis->dZone, axis->xZone, axis->state, this);
    v->addWidget(Slider);

    KeyBox = new QFrame(this);
    KeyBox->setFrameStyle( QFrame::Box | QFrame::Sunken );
    h = new QHBoxLayout(KeyBox);
    h->setSpacing(5);
    h->setMargin(5);
    //h->setAutoAdd(true);
    BNeg = new KeyButton(axis->getName(),axis->nkeycode,KeyBox,true,axis->nuseMouse);

    CThrottle = new QComboBox(KeyBox);
    CThrottle->insertItem(0,"Neg. Throttle",Qt::DisplayRole);
    CThrottle->insertItem(1,"No Throttle",Qt::DisplayRole);
    CThrottle->insertItem(2,"Pos. Throttle",Qt::DisplayRole);
    CThrottle->setCurrentIndex(axis->throttle + 1);
    connect( CThrottle, SIGNAL( activated( int )), this, SLOT( CThrottleChanged( int )));

    BPos = new KeyButton(axis->getName(),axis->pkeycode,KeyBox,true,axis->puseMouse);
    h->addWidget(BNeg);
    h->addWidget(CThrottle);
    h->addWidget(BPos);
    v->addWidget( KeyBox );

    h = new QHBoxLayout();
    BOkay = new QPushButton("&Okay", this);
    connect(BOkay, SIGNAL( clicked() ), this, SLOT( accept()));
    h->addWidget(BOkay);
    BCancel = new QPushButton("&Cancel", this);
    connect(BCancel, SIGNAL( clicked() ), this, SLOT( reject()));
    h->addWidget(BCancel);
    v->addLayout(h);

    CModeChanged( axis->mode );
	CTransferCurveChanged( axis->transferCurve );
    CThrottleChanged( axis->throttle + 1 );
}

void AxisEdit::show() {
    QDialog::show();
    setFixedSize(size());
}

void AxisEdit::setState( int val ) {
    Slider->setValue( val );
}
void AxisEdit::CGradientChanged( bool on ) {
	CTransferCurve->setEnabled(on);
	if (on) {
		CTransferCurveChanged( axis->transferCurve );
	}
	else {
		LSensitivity->setEnabled(false);
		SSensitivity->setEnabled(false);
	}
}

void AxisEdit::CModeChanged( int index ) {
    if (index == Axis::Keyboard) {
        MouseBox->setEnabled(false);
        KeyBox->setEnabled(true);
    }
    else {
        MouseBox->setEnabled(true);
        KeyBox->setEnabled(false);
		if (CGradient->isChecked()) {
			CTransferCurve->setEnabled(true);
			CTransferCurveChanged( axis->transferCurve );
		}
	}
}

void AxisEdit::CTransferCurveChanged( int index ) {
    if (index == Axis::PowerFunction) {
		LSensitivity->setEnabled(true);
		SSensitivity->setEnabled(true);
	}
	else {
		LSensitivity->setEnabled(false);
		SSensitivity->setEnabled(false);
    }
}

void AxisEdit::CThrottleChanged( int index ) {
    switch (index) {
    case 0:
        BNeg->setEnabled(true);
        BPos->setEnabled(false);
        break;
    case 1:
        BNeg->setEnabled(true);
        BPos->setEnabled(true);
        break;
    case 2:
        BNeg->setEnabled(false);
        BPos->setEnabled(true);
        break;
    }
    Slider->setThrottle( index - 1 );
}

void AxisEdit::accept() {
    axis->gradient = CGradient->isChecked();
    axis->maxSpeed = SSpeed->value();
    axis->transferCurve = (Axis::TransferCurve)CTransferCurve->currentIndex();
	axis->sensitivity = SSensitivity->value();
    axis->throttle = CThrottle->currentIndex() - 1;
    axis->dZone = Slider->deadZone();
    axis->xZone = Slider->xZone();
    axis->mode = (Axis::Mode) CMode->currentIndex();
    axis->pkeycode = BPos->getValue();
    axis->nkeycode = BNeg->getValue();
	axis->puseMouse = BPos->choseMouse();
	axis->nuseMouse = BNeg->choseMouse();
    axis->adjustGradient();

    QDialog::accept();
}
