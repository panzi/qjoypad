#ifndef QJOYPAD_AXIS_WIDGET_H
#define QJOYPAD_AXIS_WIDGET_H

#include <QMouseEvent>
//so we can interact with the axis this refers to
#include "axis.h"
//for the FlashButton widget
#include "flash.h"

//so we can edit this axis when the user clicks the button
#include "axis_edit.h"

class AxisWidget : public FlashButton {
    Q_OBJECT

	public:
		AxisWidget( Axis* a, QWidget* parent );
		//this is notified on a jsevent so it can flash if necesary.
		void jsevent( int val );
		//change the text on this button to reflect the axis' current state.
		void update();
	private:
		//to deal with clicking (by creating an AxisEdit dialog)
		void mouseReleaseEvent( QMouseEvent* e );
		//is this button currently blue?
		bool on;
		//the axis this refers to
		Axis* axis;
		//the edit dialog that we pop up on request.
		AxisEdit* ae;
};

#endif
