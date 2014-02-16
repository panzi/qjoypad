#ifndef QJOYPAD_ERROR_H
#define QJOYPAD_ERROR_H

#include <qmessagebox.h>
#include <stdarg.h>
#include "config.h"

//a nice simple way of throwing up an error message if something goes wrong.

inline void errorBox(const QString &type, const QString &message, QWidget *parent = 0) {
    QMessageBox::warning(parent, QString("%1 - %2").arg(QJOYPAD_DEVDIR, type),
		message, QMessageBox::Ok, Qt::NoButton);
}

#ifdef _DEBUG
inline void debug_mesg(const char *fmt, ...) {
    va_list ap;
    va_start(ap, NULL);
    vprintf(fmt, ap);
    va_end(ap);
}
#else
inline void debug_mesg(...) {}
#define debug_mesg(...)
#endif
#endif
