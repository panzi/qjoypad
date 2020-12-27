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
    chkGradient = new QComboBox(this);
    chkGradient->insertItem((int) Axis::ZeroOne, tr("Use 0 or max always"), Qt::DisplayRole);
    chkGradient->insertItem((int) Axis::Gradient, tr("Relative movement (previously gradient)"), Qt::DisplayRole);
    chkGradient->insertItem((int) Axis::Absolute, tr("Absolute movement (direct position)"), Qt::DisplayRole);
    chkGradient->setCurrentIndex( axis->interpretation );
    connect(chkGradient, SIGNAL(activated(int)), this, SLOT( gradientChanged( int )));
    v2->addWidget(chkGradient);

    cmbMode = new QComboBox(this);
    cmbMode->insertItem((int) Axis::Keyboard, tr("Keyboard/Mouse Button"), Qt::DisplayRole);
    cmbMode->insertItem((int) Axis::MousePosVert, tr("Mouse (Vert.)"),Qt::DisplayRole);
    cmbMode->insertItem((int) Axis::MouseNegVert, tr("Mouse (Vert. Rev.)"), Qt::DisplayRole);
    cmbMode->insertItem((int) Axis::MousePosHor, tr("Mouse (Hor.)"), Qt::DisplayRole);
    cmbMode->insertItem((int) Axis::MouseNegHor, tr("Mouse (Hor. Rev.)"), Qt::DisplayRole);
    cmbMode->setCurrentIndex( axis->mode );
    connect(cmbMode, SIGNAL(activated(int)), this, SLOT( modeChanged( int )));
    v2->addWidget(cmbMode);
    cmbTransferCurve = new QComboBox(this);
    cmbTransferCurve->insertItem(Axis::Linear, tr("Linear"), Qt::DisplayRole);
    cmbTransferCurve->insertItem(Axis::Quadratic, tr("Quadratic"), Qt::DisplayRole );
    cmbTransferCurve->insertItem(Axis::Cubic, tr("Cubic"), Qt::DisplayRole );
    cmbTransferCurve->insertItem(Axis::QuadraticExtreme, tr("Quadratic Extreme"), Qt::DisplayRole);
    cmbTransferCurve->insertItem(Axis::PowerFunction, tr("Power Function"), Qt::DisplayRole);
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
    QLabel *mouseLabel = new QLabel(tr("&Mouse Speed"), mouseBox);
    v2->addWidget(mouseLabel);
    spinSpeed = new QSpinBox(mouseBox);
    spinSpeed->setRange(0,MAXMOUSESPEED);
    spinSpeed->setSingleStep(1);
    spinSpeed->setValue(axis->maxSpeed);
    v2->addWidget(spinSpeed);
    lblSensitivity = new QLabel(tr("&Sensitivity"), mouseBox);
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
    cmbThrottle->insertItem(0, tr("Neg. Throttle"), Qt::DisplayRole);
    cmbThrottle->insertItem(1, tr("No Throttle"), Qt::DisplayRole);
    cmbThrottle->insertItem(2, tr("Pos. Throttle"), Qt::DisplayRole);
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

    gradientChanged( axis->interpretation );
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
void AxisEdit::gradientChanged( int index ) {
    bool gradient = index != Axis::ZeroOne;
    cmbTransferCurve->setEnabled(gradient);
	if (gradient) {
        transferCurveChanged( axis->transferCurve );
	}
	else {
        lblSensitivity->setEnabled(false);
        spinSensitivity->setEnabled(false);
	}
}

void AxisEdit::modeChanged( int index ) {
    if (index == Axis::Keyboard) {
        mouseBox->setEnabled(false);
        keyBox->setEnabled(true);
    }
    else {
        mouseBox->setEnabled(true);
        keyBox->setEnabled(false);
        if ((Axis::Interpretation)chkGradient->currentIndex() != Axis::ZeroOne) {
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
    axis->interpretation = (Axis::Interpretation)chkGradient->currentIndex(); 
    axis->gradient = axis->interpretation != Axis::ZeroOne;
    axis->absolute = axis->interpretation == Axis::Absolute;
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
