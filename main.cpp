#include <qapplication.h>
#include <qdialog.h>
#include <qdir.h>
#include "form.h"

#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

DMain* form;
QFile* pidFile;
bool usegui;

//prefix of the device files
extern QString Device; //from form.h

void catchSIGIO( int sig )
{
	form->LoadLayout();
	signal( sig, catchSIGIO );
}

void catchEND( int )
{
	if (usegui) form->close();
	else form->clean();
	pidFile->remove();
}

int main( int argc, char **argv )
{

	QDir dir( QDir::homeDirPath());
	if (!dir.exists(".qjoypad") && !dir.mkdir( ".qjoypad" ))
	{
		qWarning( "Could not create save directory " + QDir::homeDirPath() + "/.qjoypad!\n");
	}


	usegui = true;
	QString layout = "";

	for (int i = 1; i < argc; i++)
	{
  		 if (QRegExp("-{1,2}nogui").exactMatch(argv[i]))
		 {
			 usegui = false;
		 }
		 else if (QRegExp("-{1,2}device").exactMatch(argv[i]))
		 {
		 	++i;
			if (i < argc)
			{
				Device = argv[i];
			}
		 }
		 else if (QRegExp("-{1,2}h(elp)?").exactMatch(argv[i]))
		 {
		 	qWarning("QJoyPad 2.0\nUsage: qjoypad [--nogui] [--device \"/device/path\"] [\"layout name\"]\n\nOptions:\n  --nogui           Don't use any graphical interface\n  --device path     Look for joystick devices in \"path\". This should\n                    be something like \"/dev/input/js\" if your game\n                    devices are in /dev/input/js0, /dev/input/js1, etc.\n  \"layout name\"     Loads the given layout in an already running\n                    instance of QJoyPad, or starts QJoyPad using the\n                    given layout.\n");
			return 0;
		 }
		 else layout = argv[i];
	}

	if (layout != "")
	{
		QFile file( QDir::homeDirPath() + "/.qjoypad/layout");
		if (file.open(IO_WriteOnly))
		{
			QTextStream stream( &file );
			stream << layout;
			file.close();
		}
	}
	

	pidFile = new QFile( "/var/run/qjoypad.pid" );
	if (pidFile->exists())
	{
  		int pid;
  		if (pidFile->open( IO_ReadOnly ));
		{
			QTextStream( pidFile ) >> pid;
			pidFile->close();
			if (kill(pid,0) == 0)
			{
				if (layout == "") qWarning("There is already a running instance of QJoyPad; please close\nthe old instance before starting a new one. If you can't see\nQJoyPad running, it might be running in the background; try\nto stop it manually with 'killall qjoypad'.");
				else  kill(pid,SIGIO);
				return 0;
			}
		}
	}
	if (pidFile->open( IO_WriteOnly ))
	{
		QTextStream( pidFile ) << getpid();
		pidFile->close();
	}

	signal( SIGIO, catchSIGIO );
	signal( SIGTERM, catchEND );
	signal( SIGQUIT, catchEND );
	signal( SIGINT, catchEND );


	QApplication a( argc, argv );
	form = new DMain( usegui );

	a.setMainWidget( form );
	if (usegui) form->show();
	form->ReadLoop();

//	a.exec();  This is ACTUALLY not needed, since ReadLoop handles all the events
	pidFile->remove();
}
