#ifndef BUTTON_WIDGET_H
#define BUTTON_WIDGET_H

//this represents a Button
#include "button.h"
//this can set a button using a ButtonEdit
#include "button_edit.h"
//this IS a FlashButton
#include "flash.h"
#ifdef Bool
#undef Bool
#endif

#include <QMouseEvent>

class ButtonWidget : public FlashButton {
	public:
		ButtonWidget( Button* b, QWidget* parent);
		void jsevent( int val );
		//reset the label to match the respective Button's current state.
		void update();
	private:
		void mouseReleaseEvent( QMouseEvent* e );
		bool on;
		Button* button;
};

#endif
