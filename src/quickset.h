#ifndef QUICKSET_H
#define QUICKSET_H

#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>

#include "keycode.h"
#include "joypad.h"

class JoyPad;

class QuickSet : public QDialog {
	public:
		QuickSet(JoyPad* jp);
		void jsevent( js_event msg );
	private:
		JoyPad* joypad;
		bool setting;
};

#endif
