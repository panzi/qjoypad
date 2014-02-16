#include "layout.h"
#include <errno.h>

//initialize things and set up an icon  :)
LayoutManager::LayoutManager( bool useTrayIcon, const QString &devdir, const QString &settingsDir ) : devdir(devdir), settingsDir(settingsDir), le(0) {
    //prepare the popup first.
    fillPopup();
    connect(&trayMenuPopup,SIGNAL(triggered(QAction*)),this, SLOT(trayMenu(QAction*)));

    //make a tray icon
    if (useTrayIcon) {
        QSystemTrayIcon *tray = new QSystemTrayIcon(this);
        tray->setContextMenu(&trayMenuPopup);
        tray->setIcon(QIcon(ICON24));
        tray->show();
        connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayClick(QSystemTrayIcon::ActivationReason)));
    }
    //or make a floating icon
    else {
        FloatingIcon* icon = new FloatingIcon(QPixmap(ICON64),&trayMenuPopup,0,"tray");
        connect(icon, SIGNAL( clicked()), this, SLOT( iconClick()));
        connect(icon, SIGNAL( closed()), qApp, SLOT( quit()));
        icon->show();
    }

    //no layout loaded at start.
    setLayoutName(NL);
}

LayoutManager::~LayoutManager() {
    if (le) {
        le->close();
        le = 0;
    }
}

QString LayoutManager::getFileName( QString layoutname ) {
    return QString("%1%2.lyt").arg(settingsDir, layoutname);
}

bool LayoutManager::load(const QString& name) {
    //it's VERY easy to load NL  :)
    if (name == NL) {
        clear();
        return true;
    }
    QFile file(getFileName(name));

    //if the file isn't available,
    if (!file.exists()) {
        error("Load error","Failed to find a layout named " + name);
        return false;
    }

    //if the file isn't readable,
    if (!file.open(QIODevice::ReadOnly)) {
        error("Load error","Error reading from file " + file.fileName());
        return false;
    }

    //reset all the joypads.
    //note that we don't use available here, but joypads instead. This is so
    //if one layout has more joypads than this one does, this won't have the
    //extra settings left over after things are supposed to be "cleared"
    foreach (JoyPad *joypad, joypads) {
        joypad->toDefault();
    }

    //start reading joypads!
    QTextStream stream( &file );
    bool okay = false;
    int num = 0;
    QChar ch = 0;
    QString word;

    while (!stream.atEnd()) {
        stream >> word;

        if (word.isNull())
            break;

        //if this line is specifying a joystick
        if (word.compare(QLatin1String("joystick"), Qt::CaseInsensitive) == 0) {
            stream >> word;
            num = word.toInt(&okay);
            //make sure the number of the joystick is valid
            if (!okay || num < 1) {
                error( "Load error", QString("Error reading joystick definition. Unexpected token \"%1\". Expected a positive number.").arg(word));
                if (name != currentLayout) reload();
                else clear();
                return false;
            }
            stream.skipWhiteSpace();
            stream >> ch;
            if (ch != QChar('{')) {
                error( "Load error", QString("Error reading joystick definition. Unexpected character \"%1\". Expected '{'.").arg(ch));
                if (name != currentLayout) reload();
                else clear();
                return false;
            }
            int index = num - 1;
            //if there was no joypad defined for this index before, make it now!
            if (joypads[index] == 0) {
                joypads.insert(index, new JoyPad(index, -1, this));
            }
            //try to read the joypad, report error on fail.
            if (!joypads[index]->readConfig(stream)) {
                error( "Load error", QString("Error reading definition for joystick %1.").arg(index));
                //if this was attempting to change to a new layout and it failed,
                //revert back to the old layout.
                if (name != currentLayout) reload();
                //to keep from going into an infinite loop, if there is no good
                //layout to fall back on, go to NL.
                else clear();
                return false;
            }
        }
        else if (word == QLatin1String("#")) {
            // ignore comment
            stream.readLine();
        }
        else {
            error("Load error", QString("Error reading joystick definition. Unexpected token \"%1\". Expected \"Joystick\".").arg(word));
            if (name != currentLayout) reload();
            else clear();
            return false;
        }
    }

    //if loading succeeded, this is our new layout.
    setLayoutName(name);
    return true;
}

bool LayoutManager::load() {
    //try to load the file named "layout" to retrieve the last used layout name
    QFile file( settingsDir + "layout");
    QString name;
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        name = stream.readLine();
        file.close();
        //if there was no name, don't load.
        if (name.isEmpty()) {
            return false;
        }
        //if there was a name, try to load it! Note, this will still return
        //false if the name is invalid ( see load() )
        return load(name);
    }
    //if the file isn't available to open, don't load.
    return false;
}

bool LayoutManager::reload() {
    return load(currentLayout);
}

void LayoutManager::clear() {
    //reset all the joypads...
    foreach (JoyPad *joypad, joypads) {
        joypad->toDefault();
    }
    //and call our layout NL
    setLayoutName(NL);
}

void LayoutManager::save() {
    if (currentLayout == NL) {
        saveAs();
        return;
    }

    //get a filename
    QString filename = getFileName( currentLayout );
    QFile file(filename);
    //if it's good, start writing the file
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream( &file );
        stream << "# " NAME " Layout File\n\n";
        foreach (JoyPad *joypad, joypads) {
            joypad->write( stream );
        }
        file.close();
    }
    //if it's not, error.
    else
        error("Save error", "Could not open file " + filename + ", layout not saved.");
}


void LayoutManager::saveAs() {
    bool ok;
    //request a new name!
    QString name = QInputDialog::getText(0, NAME" - Name new layout","Enter a name for the new layout:", QLineEdit::Normal, QString::null, &ok );
    if (!ok) {
        return;
    }
    QFile file(settingsDir + name + ".lyt");
    //don't overwrite an existing layout.
    if (file.exists()) {
        error("Save error", "That name's already taken!");
        return;
    }

    //now that the new layout has a name, that is the name we should use.
    setLayoutName(name);

    //since we have a new name for this layout now, we can save it normally  :)
    save();

    //add the new name to our lists
    fillPopup();
    if (le) {
        le->updateLayoutList();
    }
}

void LayoutManager::saveDefault() {
    QFile file( settingsDir + "layout");
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream << currentLayout;
        file.close();
    }
}

void LayoutManager::remove() {
    if (currentLayout == NL) return;
    if (QMessageBox::warning( 0, NAME" - Delete layout?","Remove layout permanently from your hard drive?", "Yes", "No", 0, 0, 1 ) == 1) return;
    QString filename = getFileName( currentLayout );
    if (!QFile(filename).remove()) {
        error("Remove error", "Could not remove file " + filename);
    }
    fillPopup();

    if (le) {
        le->updateLayoutList();
    }
    clear();
}

QStringList LayoutManager::getLayoutNames() const {
    //goes through the list of .lyt files and removes the file extensions ;)
    QStringList result = QDir(settingsDir).entryList(QStringList("*.lyt"));

    for (int i = 0; i < result.size(); ++ i) {
        QString& name = result[i];
        name.truncate(name.length() - 4);
    }
    //and, of course, there's always NL.
    result.prepend(NL);

    return result;
}

void LayoutManager::setLayoutName(QString name) {
    currentLayout = name;
    fillPopup();

    if (le) {
        le->setLayout(name);
    }
}

void LayoutManager::iconClick() {
    //don't show the dialog if there aren't any joystick devices plugged in
    if (available.isEmpty()) {
        error("No joystick devices available","No joystick devices are currently available to configure.\nPlease plug in a gaming device and select\n\"Update Joystick Devices\" from the popup menu.");
        return;
    }
    if (le) {
        if (le->hasFocus()) {
            le->close();
        }
        else {
            le->raise();
        }
        return;
    }
    //otherwise, make a new LayoutEdit dialog and show it.
    le = new LayoutEdit(this);
    le->setLayout(currentLayout);
}

void LayoutManager::trayClick(QSystemTrayIcon::ActivationReason reason) {
    if(reason == QSystemTrayIcon::Trigger) {
        iconClick();
    }
}

void LayoutManager::trayMenu(QAction *menuItemAction) {
    //if they clicked on a Layout name, load it!
    //note that the other options are handled with their own special functions
    if (trayMenuPopup.actions().indexOf(menuItemAction) > 1 && menuItemAction->text() != "Quit" &&
        menuItemAction->text() != "Update lyaout list" &&
        menuItemAction->text() != "Update joystick devices") {
        load(menuItemAction->text());
    }
}

void LayoutManager::fillPopup() {
    //start with an empty slate
    trayMenuPopup.clear();

    //make a list of joystick devices
    QString devs = "Joysticks: ";
    foreach (JoyPad *joypad, available) {
        devs += QString("%1 ").arg(joypad->getIndex() + 1);
    }

    QAction *temp = trayMenuPopup.addAction(devs);
    trayMenuPopup.addSeparator(/*temp*/);

    //add in the Update options
    QAction *tempAdd = new QAction("Update layout list", this);
    connect(tempAdd, SIGNAL(triggered(bool)), this, SLOT(fillPopup()));
    trayMenuPopup.addAction(tempAdd);
    tempAdd = new QAction("Update joystick devices", this);
    connect(tempAdd, SIGNAL(triggered(bool)), this, SLOT(updateJoyDevs()));
    trayMenuPopup.addAction(tempAdd);
    trayMenuPopup.addSeparator(/*temp*/);

    //then add all the layout names
    QStringList names = getLayoutNames();
    foreach (const QString &name, names) {
        temp = trayMenuPopup.addAction(name);
        temp->setCheckable(true);
        //put a check by the current one  ;)
        if (currentLayout == name) {
            temp->setChecked(true);
        }
    }
    trayMenuPopup.addSeparator();

    //and, at the end, quit!
    trayMenuPopup.addAction("Quit",qApp,SLOT(quit()));
}

void LayoutManager::updateJoyDevs() {
    debug_mesg("updating joydevs\n");

    //reset all joydevs to sentinal value (-1)
    foreach (JoyPad *joypad, joypads) {
        joypad->close();
    }

    //clear out the list of previously available joysticks
    available.clear();

    //set all joydevs anew (create new JoyPad's if necesary)
    QDir deviceDir(devdir);
    QStringList devices = deviceDir.entryList(QStringList("js*"), QDir::System);
    QRegExp devicename(".*\\js(\\d+)");
    int joydev = -1;
    int index = -1;
    //for every joystick device in the directory listing...
    //(note, with devfs, only available devices are listed)
    foreach (const QString &device, devices) {
        QString devpath = QString("%1/%2").arg(devdir, device);
        debug_mesg("found a device file, %s\n", qPrintable(devpath));
        //try opening the device.
        joydev = open( qPrintable(devpath), O_RDONLY | O_NONBLOCK);
        //if it worked, then we have a live joystick! Make sure it's properly
        //setup.
        if (joydev >= 0) {
            devicename.indexIn(device);
            index = devicename.cap(1).toInt();
            JoyPad* joypad = joypads[index];
            //if we've never seen this device before, make a new one!
            if (joypad == 0) {
                struct pollfd read_struct;
                read_struct.fd = joydev;
                read_struct.events = POLLIN;
                char buf[10];
                while (poll(&read_struct, 1, 5) != 0) {
                    debug_mesg("reading junk data\n");
                    if (read(joydev, buf, 10) <= 0) break;
                }
                joypad = new JoyPad( index, joydev, this );
                joypads.insert(index,joypad);
            }
            else {
                debug_mesg("found previously open joypad with index %d, ignoring", index);
                joypad->open(joydev);
            }
            //make this joystick device available.
            available.insert(index,joypad);
        }
        else {
            perror(qPrintable(devpath));
        }
    }
    //when it's all done, rebuild the popup menu so it displays the correct
    //information.
    fillPopup();
    if(le) {
        le->updateJoypadWidgets();
    }
    debug_mesg("done updating joydevs\n");
}
