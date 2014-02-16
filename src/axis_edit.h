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
		void CGradientChanged( bool on );
 		void CModeChanged( int index );
		void CTransferCurveChanged( int index );
		void CThrottleChanged( int index );
		void accept();
	protected:
		//the associated Axis that needs to be set.
		Axis *axis;
		//the important parts of the dialog:
		QCheckBox *CGradient;
		QComboBox *CMode, *CThrottle, *CTransferCurve;
		QFrame *MouseBox, *KeyBox;
		QSpinBox *SSpeed;
		QLabel *LSensitivity;
		QDoubleSpinBox *SSensitivity;
		KeyButton *BNeg, *BPos;
		JoySlider *Slider;
		QPushButton *BOkay, *BCancel;
};

#endif
