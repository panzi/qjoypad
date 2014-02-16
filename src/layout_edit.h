#ifndef QJOYPAD_LAYOUT_EDIT_H
#define QJOYPAD_LAYOUT_EDIT_H

#include <QStackedWidget>

//for the tab list of joypads
#include "flash.h"
//this is a front end for the non-GUI LayoutManager
class LayoutEdit;
#include "layout.h"
//so we can use the LayoutEditer to edit key layouts  :)
#include "joypadw.h"

class LayoutManager;

class LayoutEdit : public QWidget {
	Q_OBJECT
	public:
        LayoutEdit( LayoutManager* l );
		//swap to a new layout
		void setLayout(QString layout);
		//update the list of available layouts
		void updateLayoutList();
        void updateJoypadWidgets();
    signals:
		void focusStateChanged(bool);
    public slots:
        void appFocusChanged(QWidget *old, QWidget *now);
	protected:
		//the layout manager this represents
        LayoutManager* lm;
		//parts of the dialog:
        QVBoxLayout *mainLayout;
        QStackedWidget *padStack;
        FlashRadioArray *joyButtons;
        QComboBox* cmbLayouts;
        QPushButton	*btnAdd, *btnRem, *btnUpd, *btnRev;
};

#endif
