#ifndef LAYOUT_EDIT_H
#define LAYOUT_EDIT_H

#include <qdialog.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qwidgetstack.h>

#include "flash.h"
#include "layout.h"
#include "device.h"
#include "joypadw.h"

class LayoutManager;

class LayoutEdit : public QDialog {
	public:
		LayoutEdit( LayoutManager* l );
		void setLayout(QString layout);
		void updateLayoutList();
	protected:
		LayoutManager* lm;		

		QVBoxLayout *LMain;
		QWidgetStack *PadStack;
		FlashRadioArray *JoyButtons;
		
		QComboBox* CLayouts;
		QPushButton	*BAdd, *BRem, *BUpd, *BRev;
};


#endif
