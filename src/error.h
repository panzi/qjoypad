#ifndef JOY_ERROR_H
#define JOY_ERROR_H

#include <qmessagebox.h>

static void error(QString type, QString message ) {
	QMessageBox::warning(0,NAME" - " + type,
		message, QMessageBox::Ok, QMessageBox::NoButton);
}

#endif
