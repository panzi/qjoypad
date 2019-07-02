#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <QFileDialog>

#include "layout.h"
#include "config.h"


//initialize things and set up an icon  :)
LayoutManager::LayoutManager( bool useTrayIcon, const QString &devdir, const QString &settingsDir )
    : devdir(devdir), settingsDir(settingsDir),
      layoutGroup(new QActionGroup(this)),
      updateDevicesAction(new QAction(QIcon::fromTheme("view-refresh"),tr("Update &Joystick Devices"),this)),
      updateLayoutsAction(new QAction(QIcon::fromTheme("view-refresh"),tr("Update &Layout List"),this)),
      quitAction(new QAction(QIcon::fromTheme("application-exit"),tr("&Quit"),this)),
      le(0) {

#ifdef WITH_LIBUDEV
    udevNotifier = 0;
    udev = 0;
    monitor = 0;

    if (!initUDev()) {
        errorBox(tr("UDev Error"), tr("Error creating UDev monitor. "
                 "QJoyPad will still work, but it won't automatically update the joypad device list."));
    }
#endif

    //prepare the popup first.
    fillPopup();

    //make a tray icon
    if (useTrayIcon) {
        QSystemTrayIcon *tray = new QSystemTrayIcon(this);
        tray->setContextMenu(&trayMenu);
        tray->setIcon(QIcon(QJOYPAD_ICON24));
        tray->show();
        connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayClick(QSystemTrayIcon::ActivationReason)));
    }
    //or make a floating icon
    else {
        FloatingIcon* icon = new FloatingIcon(QJOYPAD_ICON64,&trayMenu,0,"tray");
        connect(icon, SIGNAL(clicked()), this, SLOT(iconClick()));
        connect(icon, SIGNAL(rejected()), qApp, SLOT(quit()));
        connect(icon, SIGNAL(accepted()), qApp, SLOT(quit()));
        icon->show();
    }

    connect(updateLayoutsAction, SIGNAL(triggered()), this, SLOT(fillPopup()));
    connect(updateDevicesAction, SIGNAL(triggered()), this, SLOT(updateJoyDevs()));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    //no layout loaded at start.
    setLayoutName(QString());
}

LayoutManager::~LayoutManager() {
    if (le) {
        le->close();
        le = 0;
    }
#ifdef WITH_LIBUDEV
    if (udevNotifier) {
        udevNotifier->blockSignals(true);
    }
    if (monitor) {
        udev_monitor_unref(monitor);
        monitor = 0;
    }
    if (udev) {
        udev_unref(udev);
        udev = 0;
    }
#endif
}

#ifdef WITH_LIBUDEV
bool LayoutManager::initUDev() {
    udev = udev_new();
    debug_mesg("init udev\n");

    if (udev) {
        debug_mesg("udev ok\n");
        monitor = udev_monitor_new_from_netlink(udev, "udev");

        if (monitor) {
            debug_mesg("monitor ok\n");
            int errnum = udev_monitor_filter_add_match_subsystem_devtype(
                        monitor, "input", NULL);
            if (errnum != 0) {
                debug_mesg("udev_monitor_filter_add_match_subsystem_devtype: %s\n",
                           strerror(-errnum));
                udev_monitor_unref(monitor);
                udev_unref(udev);
                monitor = 0;
                udev = 0;
                return false;
            }

            errnum = udev_monitor_enable_receiving(monitor);
            if (errnum != 0) {
                debug_mesg("udev_monitor_enable_receiving: %s\n",
                           strerror(-errnum));
                udev_monitor_unref(monitor);
                udev_unref(udev);
                monitor = 0;
                udev = 0;
                return false;
            }

            udevNotifier = new QSocketNotifier(udev_monitor_get_fd(monitor), QSocketNotifier::Read, this);
            connect(udevNotifier, SIGNAL(activated(int)), this, SLOT(udevUpdate()));
            debug_mesg("notifier ok\n");
        }
        else {
            udev_unref(udev);
            udev = 0;
        }
    }

    return udev != 0;
}

void LayoutManager::udevUpdate() {
    struct udev_device *dev = udev_monitor_receive_device(monitor);
    if (dev) {
        QRegExp devicename("/js(\\d+)$");
        QString path = udev_device_get_devnode(dev);
        const char *action = udev_device_get_action(dev);

        if (devicename.indexIn(path) >= 0) {
            int index = devicename.cap(1).toInt();

            if (strcmp(action,"add") == 0 || strcmp(action,"online") == 0) {
                addJoyPad(index, path);
            }
            else if (strcmp(action,"remove") == 0 || strcmp(action,"offline") == 0) {
                removeJoyPad(index);
            }
            else if (strcmp(action,"change") == 0) {
                removeJoyPad(index);
                addJoyPad(index, path);
            }

            fillPopup();
            if (le) {
                le->updateJoypadWidgets();
            }
        }
        udev_device_unref(dev);
    }
}
#endif

QString LayoutManager::getFileName(const QString& layoutname ) {
    return QString("%1%2.lyt").arg(settingsDir, layoutname);
}

bool LayoutManager::load(const QString& name) {
    //it's VERY easy to load NL  :)
    if (name.isNull()) {
        clear();
        return true;
    }
    QFile file(getFileName(name));

    //if the file isn't available,
    if (!file.exists()) {
        errorBox(tr("Load error"), tr("Failed to find a layout named %1.").arg(name), le);
        return false;
    }

    //if the file isn't readable,
    if (!file.open(QIODevice::ReadOnly)) {
        errorBox(tr("Load error"), tr("Error reading from file: %1").arg(file.fileName()), le);
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
                errorBox(tr("Load error"),
                         tr("Error reading joystick definition. Unexpected token \"%1\". Expected a positive number.").arg(word),
                         le);
                if (name != currentLayout) reload();
                else clear();
                return false;
            }
            stream.skipWhiteSpace();
            stream >> ch;
            if (ch != QChar('{')) {
                errorBox(tr("Load error"),
                         tr("Error reading joystick definition. Unexpected character \"%1\". Expected '{'.").arg(ch),
                         le);
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
                errorBox(tr("Load error"), tr("Error reading definition for joystick %1.").arg(index), le);
                //if this was attempting to change to a new layout and it failed,
                //revert back to the old layout.
                if (name != currentLayout) reload();
                //to keep from going into an infinite loop, if there is no good
                //layout to fall back on, go to NL.
                else clear();
                return false;
            }
        }
        else if (word.startsWith('#')) {
            // ignore comment
            stream.readLine();
        }
        else {
            errorBox(tr("Load error"),
                     tr("Error reading joystick definition. Unexpected token \"%1\". Expected \"Joystick\".").arg(word),
                     le);
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
    setLayoutName(QString());
}

void LayoutManager::save() {
    if (currentLayout.isNull()) {
        saveAs();
    }
    else {
        save(getFileName(currentLayout));
    }
}

void LayoutManager::save(const QString &filename) {
    QFile file(filename);
    save(file);
}

void LayoutManager::save(QFile &file) {
    //if it's good, start writing the file
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream( &file );
        stream << "# " QJOYPAD_NAME " Layout File\n\n";
        foreach (JoyPad *joypad, joypads) {
            joypad->write( stream );
        }
        file.close();
    }
    //if it's not, error.
    else {
        errorBox(tr("Save error"), tr("Could not open file %1, layout not saved.").arg(file.fileName()), le);
    }
}

void LayoutManager::saveAs() {
    bool ok = false;
    //request a new name!
    QString name = QInputDialog::getText(le,
                                         tr("Name new layout - %1").arg(QJOYPAD_NAME),
                                         tr("Enter a name for the new layout:"),
                                         QLineEdit::Normal, QString(), &ok);
    if (!ok) {
        return;
    }
    else if (name.isEmpty()) {
        errorBox(tr("Save error"), tr("Layout name cannot be empty."), le);
        return;
    }
    else if (name.contains('/')) {
        errorBox(tr("Save error"), tr("Layout name may not contain a '/' (slash)."), le);
        return;
    }
    QFile file(getFileName(name));
    //don't overwrite an existing layout.
    if (file.exists()) {
        errorBox(tr("Save error"), tr("That name's already taken!"), le);
        return;
    }

    //now that the new layout has a name, that is the name we should use.
    setLayoutName(name);

    //since we have a new name for this layout now, we can save it normally  :)
    save(file);

    //add the new name to our lists
    fillPopup();
    if (le) {
        le->updateLayoutList();
    }
}

void LayoutManager::importLayout() {
    QFileDialog dialog(le);
    dialog.setWindowTitle(tr("Import layout - %1").arg(QJOYPAD_NAME));
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    QStringList filters;
    filters.append(tr("QJoyPad layout files (*.lyt)"));
    filters.append(tr("Any files (*)"));
    dialog.setNameFilters(filters);
    dialog.setDefaultSuffix("lyt");
    if (dialog.exec() && !dialog.selectedFiles().isEmpty()) {
        QString sourceFile = dialog.selectedFiles()[0];
        QFileInfo info(sourceFile);
        QString layoutName = info.baseName();

        if (layoutName.endsWith(".lyt",Qt::CaseInsensitive)) {
           layoutName.truncate(layoutName.size() - 4);
        }

        QString filename = getFileName(layoutName);

        if (info == QFileInfo(filename)) {
            errorBox(tr("Import error"), tr("Cannot import file from QJoyPad settings directory."));
            return;
        }

        if (QFile::exists(filename)) {
            if (QMessageBox::warning(le,
                                     QString("%1 - %2").arg(tr("Layout exists"), QJOYPAD_NAME),
                                     tr("Layout %1 exists. Do you want to overwrite it?"),
                                     tr("Over&write"), tr("&Cancel"), QString(), 0, 1) == 1) {
                return;
            }
            QFile::remove(filename);
        }
        QFile::copy(sourceFile, filename);

        fillPopup();
        if (le) {
            le->updateLayoutList();
        }

        load(layoutName);
    }
}

void LayoutManager::exportLayout() {
    QFileDialog dialog(le);
    dialog.setWindowTitle(tr("Export layout - %1").arg(QJOYPAD_NAME));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList filters;
    filters.append(tr("QJoyPad layout files (*.lyt)"));
    filters.append(tr("Any files (*)"));
    dialog.setNameFilters(filters);
    dialog.setDefaultSuffix("lyt");
    if (dialog.exec() && !dialog.selectedFiles().isEmpty()) {
        save(dialog.selectedFiles()[0]);
    }
}

void LayoutManager::saveDefault() {
    QFile file( settingsDir + "layout");
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream(&file) << currentLayout;
        file.close();
    }
}

void LayoutManager::remove() {
    if (currentLayout.isNull()) return;
    if (QMessageBox::warning(le, tr("Delete layout? - %1").arg(QJOYPAD_NAME),
        tr("Remove layout %1 permanently from your hard drive?").arg(currentLayout), tr("&Delete"), tr("&Cancel"), QString(), 0, 1 ) == 1) {
        return;
    }
    QString filename = getFileName( currentLayout );
    if (!QFile(filename).remove()) {
        errorBox(tr("Remove error"), tr("Could not remove file %1").arg(filename), le);
    }
    fillPopup();

    if (le) {
        le->updateLayoutList();
    }
    clear();
}

void LayoutManager::rename() {
    if (currentLayout.isNull()) return;
    bool ok = false;
    QString name = QInputDialog::getText(le,
                                         tr("Rename layout - %1").arg(QJOYPAD_NAME),
                                         tr("Enter a new name for the layout:"),
                                         QLineEdit::Normal, currentLayout, &ok);
    if (!ok) {
        return;
    }
    else if (name.isEmpty()) {
        errorBox(tr("Rename error"), tr("Layout name cannot be empty."), le);
        return;
    }
    else if (name.contains('/')) {
        errorBox(tr("Rename error"), tr("Layout name may not contain a '/' (slash)."), le);
        return;
    }

    QString filename = getFileName(name);

    if (QFile::exists(filename)) {
        errorBox(tr("Rename error"), tr("Layout with name %1 already exists.").arg(name), le);
        return;
    }

    if (!QFile::rename(getFileName(currentLayout), filename)) {
        errorBox(tr("Rename error"), tr("Error renaming layout."), le);
        return;
    }

    fillPopup();
    if (le) {
        le->updateLayoutList();
    }

    load(name);
}

QStringList LayoutManager::getLayoutNames() const {
    //goes through the list of .lyt files and removes the file extensions ;)
    QStringList result = QDir(settingsDir).entryList(QStringList("*.lyt"));

    for (int i = 0; i < result.size(); ++ i) {
        QString& name = result[i];
        name.truncate(name.length() - 4);
    }

    return result;
}

void LayoutManager::setLayoutName(const QString& name) {
    QList<QAction*> actions = layoutGroup->actions();
    for (int i = 0; i < actions.size(); ++ i) {
        QAction* action = actions[i];
        if (action->data().toString() == name) {
            action->setChecked(true);
            break;
        }
    }
    currentLayout = name;

    if (le) {
        le->setLayout(name);
    }
}

void LayoutManager::iconClick() {
    //don't show the dialog if there aren't any joystick devices plugged in
    if (available.isEmpty()) {
        errorBox(tr("No joystick devices available"),
                 tr("No joystick devices are currently available to configure.\nPlease plug in a gaming device and select\n\"Update Joystick Devices\" from the popup menu."),
                 le);
        return;
    }
    if (le) {
        if (le->isActiveWindow()) {
            le->close();
        }
        else {
            le->activateWindow();
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

void LayoutManager::layoutTriggered() {
    QAction *action = qobject_cast<QAction*>(sender());
    //if they clicked on a Layout name, load it!
    if (action) {
        load(action->data().toString());
    }
}

void LayoutManager::fillPopup() {
    //start with an empty slate
    trayMenu.clear();

    //add in the Update options
    trayMenu.addAction(updateLayoutsAction);
    trayMenu.addAction(updateDevicesAction);
    trayMenu.addSeparator();

    //add null layout
    QAction *action = trayMenu.addAction(tr("[NO LAYOUT]"));
    action->setCheckable(true);
    action->setActionGroup(layoutGroup);
    //put a check by the current one  ;)
    if (currentLayout.isNull()) {
        action->setChecked(true);
    }
    connect(action, SIGNAL(triggered()), this, SLOT(layoutTriggered()));

    //then add all the layout names
    foreach (const QString &name, getLayoutNames()) {
        QString title = name;
        title.replace('&',"&&");
        action = trayMenu.addAction(title);
        action->setData(name);
        action->setCheckable(true);
        action->setActionGroup(layoutGroup);
        //put a check by the current one  ;)
        if (currentLayout == name) {
            action->setChecked(true);
        }
        connect(action, SIGNAL(triggered()), this, SLOT(layoutTriggered()));
    }
    trayMenu.addSeparator();

    //and, at the end, quit!
    trayMenu.addAction(quitAction);
}

void LayoutManager::updateJoyDevs() {
    debug_mesg("updating joydevs\n");

    //reset all joydevs to sentinal value (-1)
    foreach (JoyPad *joypad, joypads) {
        joypad->close();
    }

    //clear out the list of previously available joysticks
    available.clear();

    QRegExp devicename("/js(\\d+)$");

#ifdef WITH_LIBUDEV
    // try to enumerate devices using udev, if compiled with udev support
    bool udev_ok = false;
    if (udev) {
        struct udev_enumerate *enumerate = udev_enumerate_new(udev);

        if (enumerate) {
            int errnum = udev_enumerate_add_match_subsystem(enumerate, "input");

            if (errnum == 0) {
                errnum = udev_enumerate_scan_devices(enumerate);

                if (errnum == 0) {
                    struct udev_list_entry *devices, *dev_list_entry;
                    devices = udev_enumerate_get_list_entry(enumerate);

                    udev_list_entry_foreach(dev_list_entry, devices) {
                        const char *path = udev_list_entry_get_name(dev_list_entry);
                        struct udev_device *dev = udev_device_new_from_syspath(udev, path);

                        if (dev) {
                            QString devpath = udev_device_get_devnode(dev);

                            if (devicename.indexIn(devpath) >= 0) {
                                int index = devicename.cap(1).toInt();
                                addJoyPad(index, devpath);
                            }

                            udev_device_unref(dev);
                        }
                    }

                    udev_ok = true;
                }
                else {
                    debug_mesg("udev_enumerate_scan_devices: %s\n",
                               strerror(-errnum));
                }
            }
            else {
                debug_mesg("udev_enumerate_add_match_subsystem: %s\n",
                           strerror(-errnum));
            }

            udev_enumerate_unref(enumerate);
        }
    }

    // but if udev failed still try "ls $devdir/js*"
    if (!udev_ok) {
        debug_mesg("udev enumeration failed. retry with \"ls $devdir/js*\"\n");
#endif

    //set all joydevs anew (create new JoyPad's if necesary)
    QDir deviceDir(devdir);
    QStringList devices = deviceDir.entryList(QStringList("js*"), QDir::System);
    //for every joystick device in the directory listing...
    //(note, with devfs, only available devices are listed)
    foreach (const QString &device, devices) {
        if (devicename.indexIn(device) >= 0) {
            int index = devicename.cap(1).toInt();
            QString devpath = QString("%1/%2").arg(devdir, device);
            addJoyPad(index, devpath);
        }
    }

#ifdef WITH_LIBUDEV
    }
#endif
    //when it's all done, rebuild the popup menu so it displays the correct
    //information.
    fillPopup();
    if (le) {
        le->updateJoypadWidgets();
    }
    debug_mesg("done updating joydevs\n");
}

void LayoutManager::addJoyPad(int index) {
    addJoyPad(index, QString("%1/js%2").arg(devdir, index));
}

void LayoutManager::addJoyPad(int index, const QString& devpath) {
    debug_mesg("opening %s\n", qPrintable(devpath));
    //try opening the device.
    int joydev = open(qPrintable(devpath), O_RDONLY | O_NONBLOCK);
    //if it worked, then we have a live joystick! Make sure it's properly
    //setup.
    if (joydev >= 0) {
        JoyPad* joypad = joypads[index];
        //if we've never seen this device before, make a new one!
        if (joypad == 0) {
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

void LayoutManager::removeJoyPad(int index) {
    JoyPad *joypad = available[index];
    if (joypad) {
        joypad->close();
        available.remove(index);
    }
}
