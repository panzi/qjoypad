#ifndef AXIS_EDIT_H
#define AXIS_EDIT_H

#include <qdialog.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qlabel.h>

#include "axis.h"
#include "joyslider.h"
#include "keycode.h"

class AxisEdit : public QDialog {
	Q_OBJECT
	public:
		AxisEdit(Axis* ax);
		//show the dialog (modal)
		void show();
		void setState( int val );
	protected slots:
		//slots for GUI events
		void CModeChanged( int index );
		void CThrottleChanged( int index );
		void accept();
	protected:
		//the associated Axis that needs to be set. This isn't a QAxis
		//because of complexities with inheritance and friend classes
		Axis *axis;
		QCheckBox *CGradient;
		QComboBox *CMode, *CThrottle;
		QFrame *MouseBox, *KeyBox;
		QSpinBox *SSpeed;
		KeyButton *BNeg, *BPos;
		JoySlider *Slider;
		QPushButton *BOkay, *BCancel;
};

#endif
