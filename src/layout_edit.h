#ifndef LAYOUT_EDIT_H
#define LAYOUT_EDIT_H

//needed to build the dialog
#include <qdialog.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qwidgetstack.h>

//for the tab list of joypads
#include "flash.h"
//this is a front end for the non-GUI LayoutManager
#include "layout.h"
//so we can see the available devices
#include "device.h"
//so we can use the LayoutEditer to edit key layouts  :)
#include "joypadw.h"

class LayoutManager;

class LayoutEdit : public QDialog {
	public:
		LayoutEdit( LayoutManager* l );
		//swap to a new layout
		void setLayout(QString layout);
		//update the list of available layouts
		void updateLayoutList();
	protected:
		//the layout manager this represents
		LayoutManager* lm;		
		
		//find out when the window is activated.
		virtual void windowActivationChange( bool oldActive );
		
		//parts of the dialog:
		QVBoxLayout *LMain;
		QWidgetStack *PadStack;
		FlashRadioArray *JoyButtons;
		
		QComboBox* CLayouts;
		QPushButton	*BAdd, *BRem, *BUpd, *BRev;
};


#endif
