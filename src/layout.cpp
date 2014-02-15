#include "layout.h"
#include <errno.h>

//initialize things and set up an icon  :)
LayoutManager::LayoutManager( bool useTrayIcon ) {
    //no LayoutEdit yet.
    le = NULL;

    //prepare the popup first.
    Popup = new QMenu();
    fillPopup();
    connect(Popup,SIGNAL(triggered(QAction*)),this, SLOT(trayMenu(QAction*)));

    //make a tray icon
    if (useTrayIcon) {
        QSystemTrayIcon *Tray = new QSystemTrayIcon(this);
        Tray->setContextMenu(Popup);
        Tray->setIcon(QIcon(ICON24));
        Tray->show();
        connect(Tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayClick(QSystemTrayIcon::ActivationReason)));
    }
    //or make a floating icon
    else {
        FloatingIcon* Icon = new FloatingIcon(QPixmap(ICON64),Popup,0,"tray");
        connect(Icon, SIGNAL( clicked()), this, SLOT( iconClick()));
        connect(Icon, SIGNAL( closed()), qApp, SLOT( quit()));
        Icon->show();
    }

    //no layout loaded at start.
    setLayoutName(NL);
}

QString LayoutManager::getFileName( QString layoutname ) {
    return settingsDir + layoutname + ".lyt";
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
    QHashIterator<int, JoyPad*> it( joypads );
    while (it.hasNext())
    {
        it.next();
        it.value()->toDefault();
    }

    //start reading joypads!
    QTextStream stream( &file );
    QString input = stream.readLine().toLower();
    QRegExp quoted("\"(.*)\"");
    bool okay;
    int num;

    while (input != QString::null) {
        QStringList words = input.split(" ");
        //if this line is specifying a joystick
        if (words[0] == "joystick") {
            num = words[1].toInt(&okay);
            //make sure the number of the joystick is valid
            if (!okay || okay < 1) {
                error( "Load error", "Error reading joystick definition. Expected: Joysyick 1 {");
                if (name != CurrentLayout) reload();
                else clear();
                return false;
            }
            //if there was no joypad defined for this index before, make it now!
            if (joypads[num-1] == 0) {
                joypads.insert(num-1, new JoyPad(num-1, 0));
            }
            //try to read the joypad, report error on fail.
            if (!joypads[num-1]->readConfig(&stream)) {
                error( "Load error", "Error reading definition for joystick " + QString::number(num-1));
                //if this was attempting to change to a new layout and it failed,
                //revert back to the old layout.
                if (name != CurrentLayout) reload();
                //to keep from going into an infinite loop, if there is no good
                //layout to fall back on, go to NL.
                else clear();
                return false;
            }
        }
        //read a new line.
        input = stream.readLine().toLower();
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
        if (name == "") {
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
    return load(CurrentLayout);
}

void LayoutManager::clear() {
    //reset all the joypads...
    QHashIterator<int, JoyPad*> it (joypads);
    while (it.hasNext())
    {
        it.next();
        it.value()->toDefault();
    }
    //and call our layout NL
    setLayoutName(NL);
}

void LayoutManager::save() {
    if (CurrentLayout == NL) {
        saveAs();
        return;
    }

    //get a filename
    QString filename = getFileName( CurrentLayout );
    QFile file(filename);
    //if it's good, start writing the file
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream( &file );
        stream << "# " NAME " Layout File\n\n";
        QHashIterator<int, JoyPad*> it (joypads);
        while (it.hasNext())
        {
            it.next();
            it.value()->write( &stream );
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
    if (le != NULL) {
        le->updateLayoutList();
    }
}

void LayoutManager::saveDefault() {
    QFile file( settingsDir + "layout");
    if (file.open(QIODevice::WriteOnly)) {
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

    if (le != NULL) {
        le->updateLayoutList();
    }
    clear();
}

QStringList LayoutManager::getLayoutNames() {
    //goes through the list of .lyt files and removes the file extensions ;)
    QStringList result = QDir(settingsDir).entryList(QStringList("*.lyt"));

    for ( QStringList::Iterator it = result.begin(); it != result.end(); ++it ) {
        *it = (*it).left((*it).length() - 4);
    }
    //and, of course, there's always NL.
    result.prepend(NL);

    return result;
}

void LayoutManager::setLayoutName(QString name) {
    CurrentLayout = name;
    fillPopup();

    if (le != NULL) {
        le->setLayout(name);
    }
}

void LayoutManager::iconClick() {
    //don't show the dialog if there aren't any joystick devices plugged in
    if (available.count() == 0) {
        error("No joystick devices available","No joystick devices are currently available to configure.\nPlease plug in a gaming device and select\n\"Update Joystick Devices\" from the popup menu.");
        return;
    }
    if(le) {
        return;
    }
    //otherwise, make a new LayoutEdit dialog and show it.
    le = new LayoutEdit(this);
    le->setLayout(CurrentLayout);
}

void LayoutManager::trayClick(QSystemTrayIcon::ActivationReason reason) {
    if(reason == QSystemTrayIcon::Trigger) {
        iconClick();
    }
}

void LayoutManager::trayMenu(QAction *menuItemAction) {
    //if they clicked on a Layout name, load it!
    //note that the other options are handled with their own special functions
    if (Popup->actions().indexOf(menuItemAction) > 1 && menuItemAction->text() != "Quit" &&
        menuItemAction->text() != "Update lyaout list" &&
        menuItemAction->text() != "Update joystick devices") {
        load(menuItemAction->text());
    }
}

void LayoutManager::fillPopup() {
    //start with an empty slate
    Popup->clear();

    //make a list of joystick devices
    QString devs = "Joysticks: ";
    QHashIterator<int, JoyPad*> it( available );
    while (it.hasNext())
    {
        it.next();
        devs += QString::number(it.key() + 1) + " ";
    }
    QAction *temp = Popup->addAction(devs);
    Popup->addSeparator(/*temp*/);

    //add in the Update options
    QAction *tempAdd = new QAction("Update layout list", this);
    connect(tempAdd, SIGNAL(triggered(bool)), this, SLOT(fillPopup()));
    Popup->addAction(tempAdd);
    tempAdd = new QAction("Update joystick devices", this);
    connect(tempAdd, SIGNAL(triggered(bool)), this, SLOT(updateJoyDevs()));
    Popup->addAction(tempAdd);
    Popup->addSeparator(/*temp*/);

    //then add all the layout names
    QStringList names = getLayoutNames();
    for ( QStringList::Iterator it = names.begin(); it != names.end(); ++it ) {
        temp = Popup->addAction(*it);
        temp->setCheckable(true);
        //put a check by the current one  ;)
        if (CurrentLayout == (*it)) {
            temp->setChecked(true);
        }
    }
    Popup->addSeparator();

    //and, at the end, quit!
    Popup->addAction("Quit",qApp,SLOT(quit()));
}

void LayoutManager::updateJoyDevs() {
    debug_mesg("updating joydevs\n");
    QString devdir = DEVDIR;

    //reset all joydevs to sentinal value (-1)
    do {
        QHashIterator<int, JoyPad*> it( joypads );
        while (it.hasNext() ) {
            it.next();
            it.value()->unsetDev();
        }
    } while (0);

    //clear out the list of previously available joysticks
    available.clear();

    //set all joydevs anew (create new JoyPad's if necesary)
    QDir DeviceDir(devdir);
    QStringList devices = DeviceDir.entryList(QStringList("js*"), QDir::System );
    QRegExp devicename(".*\\js(\\d+)");
    int joydev;
    int index;
    //for every joystick device in the directory listing...
    //(note, with devfs, only available devices are listed)
    for (QStringList::Iterator it = devices.begin(); it != devices.end(); ++it) {
        debug_mesg("found a device file, %s\n", qPrintable(devdir + "/" + (*it)));
        //try opening the device.
        joydev = open( qPrintable(devdir + "/" + (*it)), O_RDONLY | O_NONBLOCK);
        //if it worked, then we have a live joystick! Make sure it's properly
        //setup.
        if (joydev > 0) {
            devicename.indexIn(*it);
            index = QString(devicename.cap(1)).toInt();
            JoyPad* joypad;
            //if we've never seen this device before, make a new one!
            if (joypads[index] == 0) {
                struct pollfd read_struct;
                read_struct.fd = joydev;
                read_struct.events = POLLIN;
                char buf[10];
                while(poll(&read_struct, 1, 5)!=0) {
                    debug_mesg("reading junk data\n");
                    if (read(joydev, buf, 10) <= 0) break;
                }
                joypad = new JoyPad( index, joydev );
                joypads.insert(index,joypad);
            }
            else {
                debug_mesg("found previously open joypad with index %d, ignoring", index);
                joypad = joypads[index];
                joypad->resetToDev(joydev);
            }
            //make this joystick device available.
            available.insert(index,joypad);
        }
        else {
            int errsv = errno;
            printf("error reading joypad dev device: %s\n", strerror(errsv));
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

void LayoutManager::leWindowClosed() {
    le=NULL;
}
