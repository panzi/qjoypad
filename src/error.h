#ifndef QJOYPAD_ERROR_H
#define QJOYPAD_ERROR_H

#include <qmessagebox.h>
#include <stdarg.h>
#include "config.h"

//a nice simple way of throwing up an error message if something goes wrong.

inline void errorBox(const QString &title, const QString &message, QWidget *parent = 0) {
    QMessageBox::warning(parent, QString("%1 - %2").arg(title, QJOYPAD_NAME),
		message, QMessageBox::Ok, Qt::NoButton);
}

inline void debug_mesg(const char *fmt, ...) __attribute__((format(printf,1,2)));

#ifdef _DEBUG
inline void debug_mesg(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}
#else
inline void debug_mesg(...) {}
#define debug_mesg(...) {}
#endif
#endif
