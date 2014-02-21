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
    chkGradient = new QCheckBox("&Gradient", this);
    chkGradient->setChecked(axis->gradient);
    connect(chkGradient, SIGNAL(toggled(bool)), this, SLOT( gradientChanged( bool )));
    v2->addWidget(chkGradient);

    cmbMode = new QComboBox(this);
    cmbMode->insertItem((int) Axis::Keyboard, QString("Keyboard/Mouse Button"), Qt::DisplayRole);
    cmbMode->insertItem((int) Axis::MousePosVert,QString("Mouse (Vert.)"),Qt::DisplayRole);
    cmbMode->insertItem((int) Axis::MouseNegVert, QString("Mouse (Vert. Rev.)"), Qt::DisplayRole);
    cmbMode->insertItem((int) Axis::MousePosHor, "Mouse (Hor.)", Qt::DisplayRole);
    cmbMode->insertItem((int) Axis::MouseNegHor, QString("Mouse (Hor. Rev.)"), Qt::DisplayRole);
    cmbMode->setCurrentIndex( axis->mode );
    connect(cmbMode, SIGNAL(activated(int)), this, SLOT( modeChanged( int )));
    v2->addWidget(cmbMode);
    cmbTransferCurve = new QComboBox(this);
    cmbTransferCurve->insertItem(Axis::Linear, QString("Linear"), Qt::DisplayRole);
    cmbTransferCurve->insertItem(Axis::Quadratic, QString("Quadratic"),Qt::DisplayRole );
    cmbTransferCurve->insertItem(Axis::Cubic, QString("Cubic"),Qt::DisplayRole );
    cmbTransferCurve->insertItem(Axis::QuadraticExtreme, QString("Quadratic Extreme"), Qt::DisplayRole);
    cmbTransferCurve->insertItem(Axis::PowerFunction, QString("Power Function"), Qt::DisplayRole);
    cmbTransferCurve->setCurrentIndex( axis->transferCurve );
    cmbTransferCurve->setEnabled(axis->gradient);
    connect(cmbTransferCurve, SIGNAL(activated(int)), this, SLOT( transferCurveChanged( int )));
    v2->addWidget(cmbTransferCurve);
    h->addLayout(v2);

    mouseBox = new QFrame(this);
    mouseBox->setFrameStyle( QFrame::Box | QFrame::Sunken );
    v2 = new QVBoxLayout(mouseBox);
    v2->setSpacing(5);
    v2->setMargin(5);
    //v2->setAutoAdd(true);
    QLabel *mouseLabel = new QLabel("&Mouse Speed", mouseBox);
    v2->addWidget(mouseLabel);
    spinSpeed = new QSpinBox(mouseBox);
    spinSpeed->setRange(0,MAXMOUSESPEED);
    spinSpeed->setSingleStep(1);
    spinSpeed->setValue(axis->maxSpeed);
    v2->addWidget(spinSpeed);
    lblSensitivity = new QLabel("&Sensitivity", mouseBox);
    v2->addWidget(lblSensitivity);
    spinSensitivity = new QDoubleSpinBox(mouseBox);
    spinSensitivity->setRange(1e-3F, 1e+3F);
    spinSensitivity->setSingleStep(0.10);
    spinSensitivity->setValue(axis->sensitivity);
    v2->addWidget(spinSensitivity);
    h->addWidget(mouseBox);
    mouseLabel->setBuddy(spinSpeed);
    lblSensitivity->setBuddy(spinSensitivity);
    v->addLayout(h);

    slider = new JoySlider(axis->dZone, axis->xZone, axis->state, this);
    v->addWidget(slider);

    keyBox = new QFrame(this);
    keyBox->setFrameStyle( QFrame::Box | QFrame::Sunken );
    h = new QHBoxLayout(keyBox);
    h->setSpacing(5);
    h->setMargin(5);
    //h->setAutoAdd(true);
    btnNeg = new KeyButton(axis->getName(),axis->nkeycode,keyBox,true,axis->nuseMouse);

    cmbThrottle = new QComboBox(keyBox);
    cmbThrottle->insertItem(0,"Neg. Throttle",Qt::DisplayRole);
    cmbThrottle->insertItem(1,"No Throttle",Qt::DisplayRole);
    cmbThrottle->insertItem(2,"Pos. Throttle",Qt::DisplayRole);
    cmbThrottle->setCurrentIndex(axis->throttle + 1);
    connect( cmbThrottle, SIGNAL( activated( int )), this, SLOT( throttleChanged( int )));

    btnPos = new KeyButton(axis->getName(),axis->pkeycode,keyBox,true,axis->puseMouse);
    h->addWidget(btnNeg);
    h->addWidget(cmbThrottle);
    h->addWidget(btnPos);
    v->addWidget( keyBox );

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    v->addWidget(buttonBox);

    modeChanged( axis->mode );
    transferCurveChanged( axis->transferCurve );
    throttleChanged( axis->throttle + 1 );
}

void AxisEdit::show() {
    QDialog::show();
    setFixedSize(size());
}

void AxisEdit::setState( int val ) {
    slider->setValue( val );
}
void AxisEdit::gradientChanged( bool on ) {
    cmbTransferCurve->setEnabled(on);
	if (on) {
        transferCurveChanged( axis->transferCurve );
	}
	else {
        lblSensitivity->setEnabled(false);
        spinSensitivity->setEnabled(false);
	}
}

void AxisEdit::modeChanged( Axis::Mode mode ) {
    if (mode == Axis::Keyboard) {
        mouseBox->setEnabled(false);
        keyBox->setEnabled(true);
    }
    else {
        mouseBox->setEnabled(true);
        keyBox->setEnabled(false);
        if (chkGradient->isChecked()) {
            cmbTransferCurve->setEnabled(true);
            transferCurveChanged( axis->transferCurve );
		}
	}
}

void AxisEdit::transferCurveChanged( int index ) {
    if (index == Axis::PowerFunction) {
        lblSensitivity->setEnabled(true);
        spinSensitivity->setEnabled(true);
	}
	else {
        lblSensitivity->setEnabled(false);
        spinSensitivity->setEnabled(false);
    }
}

void AxisEdit::throttleChanged( int index ) {
    switch (index) {
    case 0:
        btnNeg->setEnabled(true);
        btnPos->setEnabled(false);
        break;
    case 1:
        btnNeg->setEnabled(true);
        btnPos->setEnabled(true);
        break;
    case 2:
        btnNeg->setEnabled(false);
        btnPos->setEnabled(true);
        break;
    }
    slider->setThrottle( index - 1 );
}

void AxisEdit::accept() {
    axis->gradient = chkGradient->isChecked();
    axis->maxSpeed = spinSpeed->value();
    axis->transferCurve = (Axis::TransferCurve)cmbTransferCurve->currentIndex();
    axis->sensitivity = spinSensitivity->value();
    axis->throttle = cmbThrottle->currentIndex() - 1;
    axis->dZone = slider->deadZone();
    axis->xZone = slider->xZone();
    axis->mode = (Axis::Mode) cmbMode->currentIndex();
    axis->pkeycode = btnPos->getValue();
    axis->nkeycode = btnNeg->getValue();
    axis->puseMouse = btnPos->choseMouse();
    axis->nuseMouse = btnNeg->choseMouse();
    axis->adjustGradient();

    QDialog::accept();
}
