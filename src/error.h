#ifndef JOY_ERROR_H
#define JOY_ERROR_H

#include <qmessagebox.h>
#include <stdarg.h>
//a nice simple way of throwing up an error message if something goes wrong.

static void error(QString type, QString message ) {
	QMessageBox::warning(0,NAME" - " + type,
		message, QMessageBox::Ok, Qt::NoButton);
}

#ifdef _DEBUG
static void debug_mesg(const char *fmt, ...) {
    va_list ap;
    va_start(ap, NULL);
    vprintf(fmt, ap);
    va_end(ap);
}
#else
static void debug_mesg(...) {
    return;
}
#endif
#endif
