#ifndef JOYPAD_WIDGET_H
#define JOYPAD_WIDGET_H

#include <qpushbutton.h>
#include <qlayout.h>

#include "joypad.h"
#include "axisw.h"
#include "buttonw.h"
#include "quickset.h"

class JoyPad;
class QuickSet;

class JoyPadWidget : public QWidget {
	Q_OBJECT
	public:
		JoyPadWidget( JoyPad* jp, int i, QWidget* parent);
		~JoyPadWidget();
		void jsevent( js_event msg );
	public slots:
		void flash( bool on );
		void update();
		void clear();
		void setAll();
	signals:
		void flashed(int);
	protected:
		JoyPad* joypad;
		int index;
		
		int flashcount;
		QGridLayout *LMain;
		AxisWidget **Axes;
		ButtonWidget **Buttons;
		QPushButton *BClear, *BAll;	
		QuickSet* quickset;
};

#endif
