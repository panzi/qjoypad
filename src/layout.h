#ifndef JOY_LAYOUT_H
#define JOY_LAYOUT_H

#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include <qinputdialog.h>
#ifndef MAIN
	#include <qpopupmenu.h>
#endif

#include <signal.h>

#include "joypad.h"
#include "error.h"
#include "trayicon/trayicon.h"
#include "device.h"
#include "icon.h"
#include "layout_edit.h"

const QString settingsDir(QDir::homeDirPath() + "/.qjoypad3/");

class LayoutManager : public QObject {
	friend class LayoutEdit;
	Q_OBJECT
	public:
		LayoutManager( bool useTrayIcon);
//		void addJoyPad(int index, JoyPad* pad);
		QStringList getLayoutNames();
	public slots:
		bool load(const QString& name);
		bool load();
		bool reload();
		void clear();
		
		void save();
		void saveAs();
		void saveDefault();
		
		void remove();
		
		void trayClick();
		void trayMenu(int id);
		void fillPopup();
		void updateJoyDevs();
	private:
		void setLayoutName(QString name);
		QString getFileName( QString layoutname );
//		QIntDict<JoyPad> joypads;
		QString CurrentLayout;
		QPopupMenu* Popup;
		
		LayoutEdit* le;
};

#endif
