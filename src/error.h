#ifndef QJOYPAD_ERROR_H
#define QJOYPAD_ERROR_H

#include <qmessagebox.h>
#include <stdarg.h>
//a nice simple way of throwing up an error message if something goes wrong.

inline void error(const QString &type, const QString &message ) {
    QMessageBox::warning(0, QString("%1 - %2").arg(NAME, type),
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
#endif
#endif
