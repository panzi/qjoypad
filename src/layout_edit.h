#ifndef QJOYPAD_LAYOUT_EDIT_H
#define QJOYPAD_LAYOUT_EDIT_H

#include <QScrollArea>
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
        void setLayout(const QString& layout);
		//update the list of available layouts
		void updateLayoutList();
        void updateJoypadWidgets();
    signals:
		void focusStateChanged(bool);
    public slots:
        void appFocusChanged(QWidget *old, QWidget *now);
    private slots:
        void load(int index);
	protected:
		//the layout manager this represents
        LayoutManager* lm;
		//parts of the dialog:
        QVBoxLayout *mainLayout;
        QScrollArea *padScroll;
        QStackedWidget *padStack;
        FlashRadioArray *joyButtons;
        QComboBox* cmbLayouts;
        QPushButton *btnAdd, *btnRem, *btnUpd, *btnRev, *btnExport, *btnImport, *btnRename;
};

#endif
