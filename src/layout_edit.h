#ifndef LAYOUT_EDIT_H
#define LAYOUT_EDIT_H

#include <QStackedWidget>

//for the tab list of joypads
#include "flash.h"
//this is a front end for the non-GUI LayoutManager
class LayoutEdit;
#include "layout.h"
//so we can see the available devices
#include "device.h"
//so we can use the LayoutEditer to edit key layouts  :)
#include "joypadw.h"

class LayoutManager;

class LayoutEdit : public QWidget {
	public:
		LayoutEdit( LayoutManager* l );
		//swap to a new layout
		void setLayout(QString layout);
		//update the list of available layouts
		void updateLayoutList();
        void updateJoypadWidgets();
	protected:
		//the layout manager this represents
		LayoutManager* lm;		
		virtual void closeEvent(QCloseEvent *event);
		//find out when the window is activated.
		virtual void windowActivationChange( bool oldActive );
		//parts of the dialog:
		QVBoxLayout *LMain;
		QStackedWidget *PadStack;
		FlashRadioArray *JoyButtons;
		QComboBox* CLayouts;
		QPushButton	*BAdd, *BRem, *BUpd, *BRev;
};

#endif
