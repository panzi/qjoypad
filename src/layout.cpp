#include "layout.h"

const char* NL = "[NO LAYOUT]";

LayoutManager::LayoutManager( bool useTrayIcon ) {
	le = NULL;

   	Popup = new QPopupMenu();
   	fillPopup();
   	connect(Popup,SIGNAL(activated(int)),this, SLOT(trayMenu(int)));

	if (useTrayIcon) {
    	TrayIcon* Tray = new TrayIcon(QPixmap(ICON24),NAME,Popup,0,"tray");
    	connect(Tray, SIGNAL( clicked(const QPoint&, int)), this, SLOT( trayClick()));
		Tray->show();
	}
	else {
		FloatingIcon* Icon = new FloatingIcon(QPixmap(ICON64),Popup,0,"tray");
		connect(Icon, SIGNAL( clicked()), this, SLOT( trayClick()));
		connect(Icon, SIGNAL( closed()), qApp, SLOT( quit()));
    	Icon->show();
	}
	
	setLayoutName(NL);
}


QString LayoutManager::getFileName( QString layoutname ) {
	return settingsDir + layoutname + ".lyt";
}

bool LayoutManager::load(const QString& name) {
	if (name.compare(NL) == 0) {
		clear();
		return true;
	}
	QFile file(getFileName(name));
	
	if (!file.exists()) {
		error("Load error","Failed to find a layout named " + name);
		return false;
	}
	
	if (!file.open(IO_ReadOnly)) {
		error("Load error","Error reading from file " + file.name());
		return false;
	}
	
    QIntDictIterator<JoyPad> it( joypads );
    for ( ; it.current(); ++it ) {
		it.current()->toDefault();
	}
	
	QTextStream stream( &file );
	QString input = stream.readLine().lower();
	QRegExp quoted("\"(.*)\"");
	bool okay;
	int num;

	while (input != QString::null) {
		QStringList words = QStringList::split(" ",input);
		if (words[0] == "joystick") {
			num = words[1].toInt(&okay);
			if (!okay) {
				error( "Load error", "Error reading joystick definition. Expected: Joysyick 1 {");
				if (name != CurrentLayout) reload();
				else clear();
				return false;
			}
			if (joypads[num-1] == 0) {
				joypads.insert(num-1, new JoyPad(num-1, 0));
			}
			if (!joypads[num-1]->read(&stream)) {
				error( "Load error", "Error reading definition for joystick " + QString::number(num-1));
				if (name != CurrentLayout) reload();
				else clear();
				return false;
			}
		}
		input = stream.readLine().lower();
	}
	setLayoutName(name);
	return true;
}

bool LayoutManager::load() {
	QFile file( settingsDir + "layout");
	QString name;
	if (file.open(IO_ReadOnly)) {
		QTextStream stream(&file);
		name = stream.readLine();
		file.close();
		if (name == "") {
			return false;
		}
		return load(name);
	}
	return false;
}

bool LayoutManager::reload() {
	if (CurrentLayout != NL)
		return load(CurrentLayout);
	else clear();
	return true;
}

void LayoutManager::clear() {
	for (QIntDictIterator<JoyPad> it(joypads); it.current(); ++it) {
		it.current()->toDefault();
	}
	setLayoutName(NL);
}

void LayoutManager::save() {
	if (CurrentLayout == NL) {
		saveAs();
		return;
	}
	
	QString filename = getFileName( CurrentLayout ); 
	QFile file(filename);
	if (file.open(IO_WriteOnly)) {
		QTextStream stream( &file );
        stream << "# "NAME" Layout File\n\n";
		for (QIntDictIterator<JoyPad> it(joypads); it.current(); ++it) {
			it.current()->write( &stream );
		}
		file.close();
	}
	else 
		error("Save error", "Could not open file " + filename + ", layout not saved.");
}

void LayoutManager::saveAs() {
	bool ok;
	QString name = QInputDialog::getText(NAME" - Name new layout","Enter a name for the new layout:", QLineEdit::Normal, QString::null, &ok );
	if (!ok) return;
	QFile file(settingsDir + name + ".lyt");
	if (file.exists()) {
		error("Save error", "That name's already taken!");
		return;
	}

	setLayoutName(name);

	save();

	fillPopup();
	if (le != 0) {
		le->updateLayoutList();
	}
}

void LayoutManager::saveDefault() {
	QFile file( settingsDir + "layout");
	if (file.open(IO_WriteOnly)) {
		QTextStream stream(&file);
		stream << CurrentLayout;
		file.close();
	}
}

void LayoutManager::remove() {
	if (CurrentLayout == NL) return;
	if (QMessageBox::warning( 0, NAME" - Delete layout?","Remove layout permanently from your hard drive?", "Yes", "No", 0, 0, 1 ) == 1) return;
	QString filename = getFileName( CurrentLayout );
	if (!QFile(filename).remove()) {
		error("Remove error", "Could not remove file " + filename);
	}
	fillPopup();
	
	if (le != 0) {
		le->updateLayoutList();
	}
	clear();
}

/*
void LayoutManager::addJoyPad(int index, JoyPad* pad ) {
	joypads.insert(index, pad);
	int id = Popup->idAt(0);
	Popup->changeItem(id,Popup->text(id) + "  " + QString::number(index + 1));
}
*/

QStringList LayoutManager::getLayoutNames() {
	QStringList result = QDir(settingsDir).entryList("*.lyt");
	
	for ( QStringList::Iterator it = result.begin(); it != result.end(); ++it ) {
		*it = (*it).left((*it).length() - 4);
	}
	result.prepend(NL);
	
	return result;
}

void LayoutManager::setLayoutName(QString name) {
	CurrentLayout = name;
//	int id = Popup->idAt(1);
	fillPopup();
	
	if (le != 0) {
		le->setLayout(name);
	}
}






void LayoutManager::trayClick() {
	if (available.count() == 0) {
		error("No joystick devices available","No joystick devices are currently available to configure.\nPlease plug in a gaming device and select\n\"Update Joystick Devices\" from the popup menu.");
		return;
	}
	le = new LayoutEdit(this);
	le->setLayout(CurrentLayout);
	le->exec();
	delete le;
	le = 0;
}

void LayoutManager::trayMenu(int id) {
	if (Popup->indexOf(id) > 4 && QString::compare(Popup->text(id),"Quit") != 0) {
		load(Popup->text(id));
	}
}

void LayoutManager::fillPopup() {
	Popup->clear();
	QString devs = "Joysticks: ";
	QIntDictIterator<JoyPad> it( available );
	for ( ; it.current(); ++it ) {
		devs += QString::number(it.currentKey() + 1) + " ";
	}
	Popup->insertItem(devs);
	Popup->insertSeparator();
	Popup->insertItem("Update layout list", this, SLOT(fillPopup()));
	Popup->insertItem("Update joystick devices", this, SLOT(updateJoyDevs()));
	Popup->insertSeparator();
	QStringList names = getLayoutNames();
	for ( QStringList::Iterator it = names.begin(); it != names.end(); ++it ) {
		int id = Popup->insertItem(*it);
		if (CurrentLayout == (*it)) Popup->setItemChecked(id,true);
	}
	Popup->insertSeparator();
	Popup->insertItem("Quit",qApp,SLOT(quit()));
}

void LayoutManager::updateJoyDevs() {
	raise(SIGUSR1);
}
