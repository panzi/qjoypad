#define MAIN

//to create a qapplication
#include <QFile>
//for ouput when there is no GUI going
#include <stdio.h>
#include <unistd.h>
//to create and handle signals for various events
#include <signal.h>

//to load layouts
#include "layout.h"
//to give event.h the current X11 display
#include "event.h"
//to update the joystick device list
#include "device.h"
//to produce errors!
#include "error.h"
#include <QX11Info>
#include <QSystemTrayIcon>
#include <poll.h>
#include <cstdlib>

//for making universally available variables
QHash<int, JoyPad*> available;			//to device.h
QHash<int, JoyPad*> joypads;           //to device.h

//variables needed in various functions in this file
LayoutManager* lm = 0;
QString devdir = DEVDIR;

//signal handler for SIGIO
//SIGIO means that a new layout should be loaded. It is saved in
// ~/.qjoypad/layout, where the last used layout is put.
void catchSIGIO( int sig )
{
    lm->load();
    //remember to catch this signal again next time.
    signal( sig, catchSIGIO );
}



//signal handler for SIGUSR1
//SIGUSR1 means that we should update the available joystick device list.
void catchSIGUSR1( int sig ) {
    //buildJoyDevices();
    lm->updateJoyDevs();
    //remember to catch this signal again next time.
    signal( sig, catchSIGUSR1 );
}


/*  A new feature to add? We'll see.
void catchSIGUSR2( int sig ) {
	lm->trayClick();
	signal( sig, catchSIGUSR2 );
}
*/



int main( int argc, char **argv )
{
    //create a new event loop. This will be captured by the QApplication
    //when it gets created
    QApplication a( argc, argv );
    a.setQuitOnLastWindowClosed(false);


    //where to look for settings. If it does not exist, it will be created
    QDir dir(settingsDir);

    //if there is no new directory and we can't make it, complain
    if (!dir.exists() && !dir.mkdir(settingsDir)) {
        printf("Couldn't create the QJoyPad save directory (%s)!", settingsDir.toStdString().c_str());
        return 1;
    }


    //start out with no special layout.
    QString layout = "";
    //by default, we use a tray icon
    bool useTrayIcon = true;
    //this execution wasn't made to update the joystick device list.
    bool update = false;
    bool forceTrayIcon = false;

    //parse command-line options
    for (int i = 1; i < a.argc(); i++) {
        //if a device directory was specified,
        if (QRegExp("-{1,2}device").exactMatch(a.argv()[i])) {
            ++i;
            if (i < a.argc()) {
                if (QFile::exists(a.argv()[i])) {
                    devdir = a.argv()[i];
                }
                else {
                    error("Command Line Argument Problems", "No such directory: " + QString(a.argv()[i]));
                    continue;
                }
            }
        }
        //if no-tray mode was requested,
        else if (QRegExp("-{1,2}notray").exactMatch(a.argv()[i])) {
            useTrayIcon = false;
        }
        //if this execution is just meant to update the joystick devices,
        else if (QRegExp("-{1,2}update").exactMatch(a.argv()[i])) {
            update = true;
        }
        //if help was requested,
        else if (QRegExp("-{1,2}h(elp)?").exactMatch(a.argv()[i])) {
            printf("%s\n"
				"Usage: qjoypad [--device \"/device/path\"] [--notray] [\"layout name\"]\n"
				"\n"
				"Options:\n"
				"  --device path     Look for joystick devices in \"path\". This should\n"
				"                    be something like \"/dev/input\" if your game\n"
				"                    devices are in /dev/input/js0, /dev/input/js1, etc.\n"
				"  --notray          Do not use a system tray icon. This is useful for\n"
				"                    window managers that don't support this feature.\n"
				"  --update          Force a running instance of QJoyPad to update its\n"
				"                    list of devices and layouts.\n"
				"  \"layout name\"     Load the given layout in an already running\n"
				"                    instance of QJoyPad, or start QJoyPad using the\n"
				"                    given layout.\n", NAME);
            return 1;
        } else if(QRegExp("--force-tray").exactMatch(a.argv()[i])) {
            useTrayIcon = true;
            forceTrayIcon = true;
        }
        //in all other cases, an argument is assumed to be a layout name.
        //note: only the last layout name given will be used.
        else layout = a.argv()[i];
    }

    //if the user specified a layout to use,
    if (layout != "")
    {
        //then we try to store that layout in the last-used layout spot, to be
        //loaded by default.
        QFile file( settingsDir + "layout");
        if (file.open(QIODevice::WriteOnly))
        {
            QTextStream stream( &file );
            stream << layout;
            file.close();
        }
    }





    //create a pid lock file.
    QFile pidFile( "/tmp/qjoypad.pid" );
    //if that file already exists, then qjoypad is already running!
    if (pidFile.exists())
    {
        int pid = 0;
        if (pidFile.open( QIODevice::ReadOnly ))
        {
            //try to get that pid...
            QTextStream( &pidFile ) >> pid;
            pidFile.close();
            //if we can signal the pid (ie, if the process is active)
            if (kill(pid,0) == 0)
            {
                //then prevent two instances from running at once.
                //however, if we are setting the layout or updating the device
                //list, this is not an error and we shouldn't make one!
                if (layout == "" && update == false) error("Instance Error","There is already a running instance of QJoyPad; please close\nthe old instance before starting a new one.");
                else  {
                    //if one of these is the case, send the approrpriate signal!
                    if (update == true)
                        kill(pid,SIGUSR1);
                    if (layout != "")
                        kill(pid,SIGIO);
                }
                //and quit. We don't need two instances.
                return 0;
            }
        }
    }
    //now we can try to create and write our pid to the lock file.
    if (pidFile.open( QIODevice::WriteOnly ))
    {
        QTextStream( &pidFile ) << getpid();
        pidFile.close();
    }

    if(forceTrayIcon) {
        int sleepCounter = 0;
        while(!QSystemTrayIcon::isSystemTrayAvailable()) {
            sleep(1);
            sleepCounter++;
            if(sleepCounter > 20) {
                printf("Error, we've waited more than 20 seconds, your sys tray probably isn't loading\n");
                exit(1);
            }
        }
    }
    //capture the current display for event.h
    display = QX11Info::display();

    //create a new LayoutManager with a tray icon / floating icon, depending
    //on the user's request
    lm = new LayoutManager(useTrayIcon);

    //build the joystick device list for the first time,
    //buildJoyDevices();
    lm->updateJoyDevs();
    
    //load the last used layout (Or the one given as a command-line argument)
    lm->load();

    //prepare the signal handlers
    signal( SIGIO,   catchSIGIO );
    signal( SIGUSR1, catchSIGUSR1 );
//	signal( SIGUSR2, catchSIGUSR2 );

    //and run the program!
    int result = a.exec();

    //when everything is done, save the current layout for next time...
    lm->saveDefault();

    //remove the lock file...
    pidFile.remove();

    //and terminate!
    return result;
}
