#ifndef BUTTON_EDIT_H
#define BUTTON_EDIT_H

#include <qdialog.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qwidgetstack.h>

#include "button.h"
#include "keycode.h"

class ButtonEdit : public QDialog {
	Q_OBJECT
	public:
		ButtonEdit(Button* butt);
		void show();
	protected slots:
		void accept();
	protected:
		Button *button;
		KeyButton *BKKey;
		QPushButton *BKey, *BMouse, *BOkay, *BCancel;
		QPushButton **BMKey;
		QCheckBox *CSticky, *CRapid;
};


#endif
