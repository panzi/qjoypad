#define MAIN
#include <qapplication.h>
#include <qstring.h>
#include <qdir.h>
#include <qfile.h>
#include <qstringlist.h>
#include <qregexp.h>

#include <stdio.h>
#include <signal.h>

#include "loop.h"
#include "layout.h"
#include "event.h"
#include "device.h"
#include "error.h"

extern Display* display;			//from event.h
QIntDict<JoyPad> available;			//to device.cpp
QIntDict<JoyPad> joypads;           //to device.cpp
LayoutManager* lm;
QString devdir = DEVDIR;
	
void buildJoyDevices() {
	//reset all joydevs to sentinal value
    QIntDictIterator<JoyPad> it( joypads );
    for ( ; it.current(); ++it ) {
		it.current()->unsetDev();
	}

	//clear available
	available.clear();

	//set all joydevs anew (create new JoyPad's if necesary)
	QDir DeviceDir(devdir);
	QStringList devices = DeviceDir.entryList("js*", QDir::System );
	QRegExp devicename(".*\\js(\\d+)");
	int joydev;
	int index;
	for (QStringList::Iterator it = devices.begin(); it != devices.end(); ++it) {
		joydev = open( devdir + "/" + (*it), O_RDONLY | O_NONBLOCK);
		if (joydev > 0) {
			devicename.match(*it);
			index = QString(devicename.cap(1)).toInt();
			JoyPad* joypad;
			//if we've never seen this device before...
			if (joypads[index] == 0) {
				joypad = new JoyPad( index, joydev );
				joypads.insert(index,joypad);
			}
			else {
				joypad = joypads[index];
				joypad->resetToDev(joydev);
			}
			available.insert(index,joypad);
		}
	}

	lm->fillPopup();
}

//signal handler for SIGIO
//checks to see if it should reload a layout
void catchSIGIO( int sig )
{
	lm->load();
	signal( sig, catchSIGIO );
}

void catchSIGUSR1( int sig ){
	buildJoyDevices();
	signal( sig, catchSIGUSR1 );
}


int main( int argc, char **argv )
{


	JoyLoop loop;
    QApplication a( argc, argv );


	QDir configDir(QDir::homeDirPath() + "/.qjoypad3");

	//if there is no new directory and we can't make it, complain
	if (!configDir.exists() && !configDir.mkdir(configDir.path())) {
		printf("Couldn't create the QJoyPad save directory (" + configDir.path() + ")!");
		return 1;
	}



	QString layout = "";
	bool useTrayIcon = true;
	bool update = false;
	//parse options
	for (int i = 1; i < a.argc(); i++) {
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
		else if (QRegExp("-{1,2}notray").exactMatch(a.argv()[i])) {
			useTrayIcon = false;
		}
		else if (QRegExp("-{1,2}update").exactMatch(a.argv()[i])) {
			update = true;
		}
		else if (QRegExp("-{1,2}h(elp)?").exactMatch(a.argv()[i])) {
		 	printf(NAME"\nUsage: qjoypad [--device \"/device/path\"] [--notray] [\"layout name\"]\n\nOptions:\n  --device path     Look for joystick devices in \"path\". This should\n                    be something like \"/dev/input\" if your game\n                    devices are in /dev/input/js0, /dev/input/js1, etc.\n  --notray          Do not use a system tray icon. This is useful for\n                    window managers that don't support this feature.\n  --update          Force a running instance of QJoyPad to update its\n                    list of devices and layouts.\n  \"layout name\"     Load the given layout in an already running\n                    instance of QJoyPad, or start QJoyPad using the\n                    given layout.\n");
			return 1;
		}
		else layout = a.argv()[i];
	}

	if (layout != "")
	{
		QFile file( settingsDir + "layout");
		if (file.open(IO_WriteOnly))
		{
			QTextStream stream( &file );
			stream << layout;
			file.close();
		}
	}

	
	
	
	
	//create a pid lock file.
	QFile pidFile( "/var/run/qjoypad.pid" );
	if (pidFile.exists())
	{
  		int pid;
  		if (pidFile.open( IO_ReadOnly ));
		{
			QTextStream( &pidFile ) >> pid;
			pidFile.close();
			if (kill(pid,0) == 0)
			{
				if (layout == "" && update == false) error("Instance Error","There is already a running instance of QJoyPad; please close\nthe old instance before starting a new one.");
				else  {
					if (update == true)
						kill(pid,SIGUSR1);
					if (layout != "")
						kill(pid,SIGIO);
				}
				return 0;
			}
		}
	}
	if (pidFile.open( IO_WriteOnly ))
	{
		QTextStream( &pidFile ) << getpid();
		pidFile.close();
	}

	
	
	
	
	signal( SIGIO,   catchSIGIO );
	signal( SIGUSR1, catchSIGUSR1 );
	
	
	
	
	
	display = QPaintDevice::x11AppDisplay();

	lm = new LayoutManager(useTrayIcon);

/*	
	QDir DeviceDir(devdir);
	QStringList devices = DeviceDir.entryList("js*", QDir::System );
	QRegExp devicename(".*\\js(\\d+)");
	int joydev;
	int index;
	for (QStringList::Iterator it = devices.begin(); it != devices.end(); ++it) {
		joydev = open( devdir + "/" + (*it), O_RDONLY | O_NONBLOCK);
		if (joydev > 0) {
			devicename.match(*it);
			index = QString(devicename.cap(1)).toInt();
			JoyPad* joypad = new JoyPad( index, joydev );
			available.insert(index,joypad);
			lm->addJoyPad(index, joypad);
		}
	}
*/
	buildJoyDevices();

/*
	if (available.count() == 0) {
		error("No Joysticks!","Couldn't find any joystick devices!");
		return 1;
	}
*/
	
	lm->load();
	
    int result = a.exec();
	
	lm->saveDefault();
	
	pidFile.remove();
	return result;
}
