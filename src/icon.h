#ifndef QJOYPAD_ICON_H
#define QJOYPAD_ICON_H

#include <QDialog>
#include <QMenu>
#include <QPixmap>
#include <QMouseEvent>
#include <QCloseEvent>

#include "constant.h"

class FloatingIcon : public QDialog {
	Q_OBJECT
	public:
		FloatingIcon( const QPixmap &icon, QMenu *popup = 0, QWidget *parent = 0, const char *name = 0);
	signals:
		void closed();
		void clicked();
	protected:
		void mousePressEvent( QMouseEvent* e );
		void closeEvent( QCloseEvent* e );
		QMenu* pop;
};

#endif
