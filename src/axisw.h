#ifndef AXIS_WIDGET_H
#define AXIS_WIDGET_H

#include "axis.h"
#include "axis_edit.h"
#include "flash.h"

class AxisWidget : public FlashButton {
	Q_OBJECT
	public:
		AxisWidget( Axis* a, QWidget* parent );
		void jsevent( int val );
		void update();
	private:
		void mouseReleaseEvent( QMouseEvent* e );
		bool on;
		Axis* axis;
		AxisEdit* ae;
};

#endif
