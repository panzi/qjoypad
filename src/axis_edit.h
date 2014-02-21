#ifndef QJOYPAD_AXIS_EDIT_H
#define QJOYPAD_AXIS_EDIT_H
//to refer to the axis we're editing
//for building up the dialog we need
#include "axis.h"
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QDialogButtonBox>
//for my home-brewed widgets
#include "joyslider.h"
#include "keycode.h"

class AxisEdit : public QDialog {
	Q_OBJECT
	public:
		AxisEdit(Axis* ax);
		//show the dialog (modal)
		void show();
		//set the current state of the axis (adjusts the JoySlider for real time
		//notification of the state to the user)
		void setState( int val );
	protected slots:
		//slots for GUI events
		void gradientChanged( bool on );
		void modeChanged( Axis::Mode mode );
		void transferCurveChanged( int index );
		void throttleChanged( int index );
		void accept();
	protected:
		//the associated Axis that needs to be set.
		Axis *axis;
		//the important parts of the dialog:
		QCheckBox *chkGradient;
		QComboBox *cmbMode, *cmbThrottle, *cmbTransferCurve;
		QFrame *mouseBox, *keyBox;
		QSpinBox *spinSpeed;
		QLabel *lblSensitivity;
		QDoubleSpinBox *spinSensitivity;
		KeyButton *btnNeg, *btnPos;
		JoySlider *slider;
};

#endif
