#ifndef BUTTON_WIDGET_H
#define BUTTON_WIDGET_H

#include "button.h"
#include "button_edit.h"
#include "flash.h"

class ButtonWidget : public FlashButton {
	public:
		ButtonWidget( Button* b, QWidget* parent);
		void jsevent( int val );
		void update();
	private:
		void mouseReleaseEvent( QMouseEvent* e );
		bool on;
		Button* button;
};

#endif
