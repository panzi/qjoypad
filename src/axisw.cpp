#include "axisw.h"

AxisWidget::AxisWidget( Axis* a, QWidget* parent )
	: FlashButton( "",parent) {
	axis = a;
	ae = NULL;
	update();
	on = false;
}

void AxisWidget::jsevent( int val ) {
	bool newOn = !axis->inDeadZone(val);
	if (on != newOn) {
		on = newOn;
		flash();
	}
	if (ae != NULL) ae->setState(val);
}

void AxisWidget::update() {
	setText( axis->status());
}

void AxisWidget::mouseReleaseEvent( QMouseEvent* e ) {
	ae = new AxisEdit(axis);
	ae->exec();
	delete ae;
	ae = NULL;
	update();
	FlashButton::mouseReleaseEvent( e );
}
