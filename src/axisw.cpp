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
	//create the edit dialog,
	ae = new AxisEdit(axis);
	//get its input
	ae->exec();
	//now that it's done, destroy it!
	delete ae;
	//and remember that it's gone.
	ae = NULL;
	update();
	//release the button. Waiting to do this until the very end has the nice
	//effect of keeping the button depressed while the dialog is shown.
	FlashButton::mouseReleaseEvent( e );
}
