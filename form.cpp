#include "form.h"

//The current XWindows display
extern Display* display;				//from sendevent.h

//Converts a key index into that key's name ("Up", "Button 2", etc.)
extern QString Name( int index );	//from savetypes.h


QString Device = "";


JoyKeyPad::JoyKeyPad( int buttons, QWidget* parent )
			 :QWidget( parent )
{
	if (buttons == -1)
	{
		AllButtons = 0;
		return;
	}
	AllButtons = buttons + DIRECTIONS;

	Buttons = new FlashButton*[AllButtons];
	KeyCodes = new int[AllButtons];
	
	LMain = new QGridLayout( this, AllButtons / 2, 2, 5, 5);
	
	Sense = new QSlider( 0, 32766, 100, 7000, Qt::Horizontal, this );
	Sense->setTickmarks( (QSlider::TickSetting) 2 );
	Sense->setTickInterval( 4096 );//  32766 / 8 = 4096
	LMain->addMultiCellWidget( Sense, 0,0,0,1);
	
	for (int i = 0; i < AllButtons; i++)
	{
		Buttons[i] = new FlashButton( Name( i ), this, QString::number(i) );
		LMain->addWidget( Buttons[i], 1 + i / 2, i % 2);
		connect( Buttons[i], SIGNAL( clicked() ), this, SLOT( getKey() ));
	}
	
	BClear = new QPushButton( "Clear", this );
	BClear->setAutoDefault( false );
	LMain->addWidget( BClear, 1 + AllButtons / 2, 0 );
	connect( BClear, SIGNAL( clicked() ), this, SLOT( clear() ));
	
	BAll = new QPushButton( "Set all keys", this );
	BAll->setAutoDefault( false );
	LMain->addWidget( BAll, 1 + AllButtons / 2, 1 );
	connect( BAll, SIGNAL( clicked() ), this, SLOT( all() ));
	
	KeyDialog = 0;
}

void JoyKeyPad::getState( JoyKeys* key )
{
	for (int i = 0; i < AllButtons; i++)
	{
		key->setButton(i,KeyCodes[i]);
	}
	key->setSensitivity( Sense->value());
}

void JoyKeyPad::flash( int index )
{
	Buttons[index]->flash();
}

void JoyKeyPad::sendKey( int type, int index )
{
	//if the main dialog isn't the active window,
	//and either the getkey dialog is not running
	//or is not the active window, send a keypress.
	//This is to keep the user from causing interesting
	//effects by triggering QJoyPad with the events
	//produced BY QJoyPad.
	//!KeyDialog is needed because accessing isActiveWindow()
	//of an undefined KeyDialog is a surefire segfault.
//This is no longer a problem since no widget on the form reacts to keyboard
//input and the event loop is stalled by GetKey
//	if (!hasFocus() && (!KeyDialog || !KeyDialog->isActiveWindow()))
//	{
	sendevent(type, KeyCodes[index]); 
//	}
	flash( index );
}

int JoyKeyPad::sensitivity()
{
	return Sense->value();
}

//This is the signal that calls the real function
void JoyKeyPad::getKey()
{
	getkey( QString(sender()->name()).toInt() );
}

void JoyKeyPad::setState( JoyKeys* keys )
{
	for (int i = 0; i < AllButtons; i++)
	{
		KeyCodes[i] = keys->getButton(i);
		Buttons[i]->setText( Name(i) + " : " + ktos( KeyCodes[i] ) );
		Sense->setValue( keys->getSensitivity() );
	}
}

void JoyKeyPad::clear()
{
	for (int i = 0; i < AllButtons; i++)
	{
		KeyCodes[i] = 0;
		Buttons[i]->setText( Name(i) + " : " + ktos( 0 ));
	}
}

void JoyKeyPad::getkey( int index )
{
	QString fullname = Name(index);
	KeyDialog = new GetKey( fullname );
	KeyDialog->exec();
	
	int keycode = KeyDialog->result();
	if (keycode == 9) keycode = 0; //Capture escape as NoKey
	QString keyname = ktos( keycode );

	delete KeyDialog;
	KeyDialog = 0; //So I know that it's no longer active in sendKey
	
	if (keyname) //If they gave an acceptible key...
	{            //I check this to disallow things like CapsLock
					 //which have interesting side effects
		KeyCodes[index] = keycode;
		Buttons[index]->setText( fullname + " : " + keyname );
	}
}

void JoyKeyPad::all()
{
	for (int index = 0; index < AllButtons; index++)
	{
		getkey( index );
	}
}














ModCombo::ModCombo( QWidget* parent )
			:QComboBox( parent )
{
}

void ModCombo::keyPressEvent( QKeyEvent* )
{
}









DMain::DMain( bool usegui )
		:QDialog( 0 )
{
	UseGui = usegui;
	if (UseGui)
	{
		setCaption( NAME );
		LMain = new QVBoxLayout( this, 5, 0 );
		LMain->setAutoAdd( true );

		CFrame = new QFrame( this );
		CFrame->setFrameStyle( QFrame::Box | QFrame::Raised );
		CLayout = new QGridLayout( CFrame, 2, 4, 5, 5 );

		CCombo = new ModCombo( CFrame );
		CLayout->addMultiCellWidget( CCombo, 0,0,0,3);
		connect( CCombo, SIGNAL( activated( int ) ), this, SLOT( CRevert() ));

		CBAdd = new QPushButton( "Add", CFrame );
		CBAdd->setAutoDefault( false );
		CLayout->addWidget( CBAdd, 1,0);
		connect( CBAdd, SIGNAL( clicked() ), this, SLOT( CAdd() ));

		CBRem = new QPushButton( "Remove", CFrame );
		CBRem->setAutoDefault( false );
		CLayout->addWidget( CBRem, 1, 1);
		connect( CBRem, SIGNAL( clicked() ), this, SLOT( CRem() ));

		CBUpdate = new QPushButton( "Update", CFrame );
		CBUpdate->setAutoDefault( false );
		CLayout->addWidget( CBUpdate, 1, 2);
		connect( CBUpdate, SIGNAL( clicked() ), this, SLOT( CUpdate() ));

		CBRevert = new QPushButton( "Revert", CFrame );
		CBRevert->setAutoDefault( false );
		CLayout->addWidget( CBRevert, 1, 3);
		connect( CBRevert, SIGNAL( clicked() ), this, SLOT( CRevert() ));
	}


	if (Device == "") Device = DEVICE;

	int i;
	JoyCount = 0;
        int joynum;

/*
Code added to speed up testing joystick devices on certain systems,
especially those that use devfs. Although opening a device file
shouldn't take any time, it does on some systems in the case that
the device doesn't exist. This code alleviates that problem by
checking only files that exist, limiting the checks to the system
maximum and in the case of devfs users checking only the devices
which are currently plugged in. Thanks to Patrik Johansson for the
idea and this code snippet ;)
 */
 	glob_t globbuf;
	glob(QString(DEVICE) + "*", GLOB_NOSORT, NULL, &globbuf);
   	for(i = 0; i < globbuf.gl_pathc; i++)
	{
	  char *number = strpbrk(globbuf.gl_pathv[i],"0123456789");
	  if (number != NULL)
          {
	    joynum = atol(number);
	    if (JoyCount < joynum)
	      JoyCount = joynum;
	  }
	}
//--- Thanks again Patrik ;)

	//find the highest-numbered joystick (in most cases, the number of joysticks available)
	for (i = JoyCount; i >= 0; i--)
	{
		QString filename = Device + QString::number( i );
  		int file = open( filename, O_RDONLY | O_NONBLOCK );

		if (file > 0)
		{
//			close( file );   For some reason, this causes segfaults...
			JoyCount = i + 1;
			break;
		}
	}
	if (JoyCount == 0)
	{
		QMessageBox mb( "No joystick devices found!",
			"QJoyPad couldn't find any joysticks plugged into your system. You\nmust plug in and, if necesary, load modules for every game device\nyou wish to use in QJoyPad before starting the program. If your\ndevices ARE plugged in and working properly, please make sure that\nQJoyPad is looking in the right directory by passing that directory\nin the \"--device\" argument.\n\nNo devices were found in: " + Device,
			QMessageBox::Critical,
			QMessageBox::Ok | QMessageBox::Default,
			QMessageBox::NoButton,
			QMessageBox::NoButton);
		mb.exec();
		return;
	}
	if (UseGui)
	{
		QString names[JoyCount];
		for (i = 1; i <= JoyCount; i++)
		{
			names[i - 1] = "Joystick " + QString::number(i);
		}
		JoyStick = new FlashRadioArray( JoyCount, names, true, this );

		Stack = new QWidgetStack( this );
		Stack->setFrameStyle( QFrame::Box | QFrame::Raised );
	}
	
	JoyDev = new int[JoyCount];
	AllButtons = new int[JoyCount];

	JoyKeyPad* pad;
	char buttons;
	for (i = 0; i < JoyCount; i++)
	{
		JoyDev[i] = open(QString( Device ) + QString::number(i), O_RDONLY | O_NONBLOCK);
		if (JoyDev[i]>-1) //device IS available
		{
			//find out which buttons the joystick api thinks are available.
			ioctl(JoyDev[i], JSIOCGBUTTONS, &buttons);
			AllButtons[i] = buttons + DIRECTIONS;
		}
		else
		{
			JoyStick->dark( i );
			buttons = -1;
		}

		if (UseGui)
		{
			pad = new JoyKeyPad( buttons, this );
			Stack->addWidget(pad);
		}
	}
	CLoad();

	display=XOpenDisplay(NULL);	

	if (UseGui)
	{
		connect( JoyStick, SIGNAL( changed( int )), Stack, SLOT( raiseWidget( int )));
		Items.setAutoDelete( true );
		if (CCombo->count() == 0)
		{
			Layout = -1;
			Clear();
			return;
		}
	}

	Layout = 0;
	if (UseGui) CCombo->setCurrentItem( 0 );
	LoadLayout();
	if (UseGui) CRevert();
}

void DMain::CAdd()
{
	QString newname = QInputDialog::getText( "Choose name for a new layout", "Please enter a name for the current layout:" );
	if (newname != "")
	{
		Layout = CCombo->count();
		CCombo->insertItem( newname );
		CCombo->setCurrentText( newname );

		OneSave* result = new OneSave();
		result->setName( newname );
		
		JoyKeys* key;
		for (int i = 0; i < JoyCount; i++)
		{
			if (JoyDev[i] == -1) continue;
			key = new JoyKeys();
			((JoyKeyPad*)Stack->widget( i ))->getState(key);
			result->setStick( i, key);
		}
		result->setChanged();
		Items.append( result );
	}
}

void DMain::CRem()
{
	QMessageBox mb( "Remove layout?",
		"If you remove this layout, it will be permanently erased\nfrom your hard drive. Are you sure you want to delete it?",
		QMessageBox::Warning,
		QMessageBox::Yes,
		QMessageBox::No,
		QMessageBox::NoButton);
	if (mb.exec() == QMessageBox::No) return;


	if (Layout != -1)
	{
		CCombo->removeItem( Layout );
		Items.at(Layout)->remove();//remove the file
		Items.remove( Layout );//remove the item
		if (Layout != 0)
		{
			CCombo->setCurrentItem( --Layout );
			CRevert();
		}
		else
		{
			Layout = -1;
			Clear();
		}
	}
}

void DMain::CUpdate()
{
	OneSave* newone = Items.at(Layout);
	JoyKeys* key;

	for (int i = 0; i < JoyCount; i++)
	{
		if (JoyDev[i] == -1) continue;
		key = newone->getStick(i);
		((JoyKeyPad*)Stack->widget( i ))->getState(key);
	}
	newone->setChanged();
}

void DMain::CRevert()
{
	Layout = CCombo->currentItem();
	OneSave* old = Items.at( Layout );
	for (int i = 0; i < JoyCount; i++)
	{
		((JoyKeyPad*)Stack->widget( i ))->setState( old->getStick( i ) );
	}
}

void DMain::CSave()
{
	OneSave* layout;
	for (layout = Items.first(); layout; layout = Items.next())
	{
		layout->save();
	}
}

void DMain::CLoad()
{
	QStringList files = QDir( QDir::homeDirPath() + "/.qjoypad", "*.lyt").entryList();

	OneSave* layout;
	for (unsigned int i = 0; i < files.count(); i++)
	{
		layout = new OneSave();
		if (layout->open( files[i] ))
		{
			if (UseGui) CCombo->insertItem( layout->name() );
			Items.append( layout );
		}
		else delete layout;
	}
}


void DMain::clean()
{
	if (JoyCount == 0) return;
	
	Finished = true; //end the ReadLoop
	
	XCloseDisplay( display );

	CSave();

	//save the current layout as default
	QFile file( QDir::homeDirPath() + "/.qjoypad/layout");
	if (file.open(IO_WriteOnly))
	{
		QTextStream stream( &file );
		stream << Items.at(Layout)->name();
		file.close();
	}
}

void DMain::done( int r )
{
	clean();
	QDialog::done( r );
}

void DMain::Clear()
{
	for (int i = 0; i < JoyCount; i++)
	{
		((JoyKeyPad*)Stack->widget( i ))->clear();
	}
}

void DMain::LoadLayout()
{
	QFile file( QDir::homeDirPath() + "/.qjoypad/layout" );

	QString name;
	if (file.exists() && file.open(IO_ReadOnly))
	{
		QTextStream stream( &file );
		name = stream.readLine();
		file.close();
	}

	if (name == "") return;

	for ( unsigned int i = 0; i < Items.count(); i++)
	{
		if (Items.at(i)->name() == name)
		{
			if (UseGui)
			{
				CCombo->setCurrentItem( i );
				CRevert();
			}
			Layout = i;
			return;
		}
	}

	QMessageBox mb( "Unknown layout",
		"QJoyPad couldn't load the layout you specified.\nThere is no layout named: " + name,
		QMessageBox::Critical,
		QMessageBox::Ok | QMessageBox::Default,
		QMessageBox::NoButton,
		QMessageBox::NoButton);
	mb.exec();
	return;
}


void DMain::ReadLoop()
{
	if (JoyCount == 0) return;

	if (! UseGui)
	{
		QMessageBox mb( "QJoyPad --nogui",
			"QJoyPad has started successfully and is now running in the background!",
			QMessageBox::Information,
			QMessageBox::Ok | QMessageBox::Default,
			QMessageBox::NoButton,
			QMessageBox::NoButton);
		mb.exec();
	}


	bool* Pressed[JoyCount];
	int stick = 0;
	unsigned int len;
	js_event msg;
	bool joy[JoyCount];
	bool oldjoy[JoyCount];
	int minzero, maxzero;
	int i;
	
	//set variables to begin
	for (i = 0; i < JoyCount; i++)
	{
		Pressed[i] = new bool[AllButtons[i]];
		for (int j = 0; j < AllButtons[i]; j++)
		{
			Pressed[i][j] = false;
		}
		
		joy[i] = false; oldjoy[i] = false;
	}

	Finished = false;
	while (!Finished)
	{
		i = stick + 1; //skip the stick we last did. (if no stick, ie -1, then go to 0)
		stick = -1;
		//For every joystick device we're watching...
                //Note that select won't work here since we need to capture
                //XWindows events...
		for (; i < JoyCount; i++)
		{
			if (JoyDev[i] == -1) continue;
			len = read( JoyDev[i], &msg, sizeof(js_event));
			if (len == sizeof(js_event))
			{
				stick = i;
				break; //if we have an input event to parse, stop reading and
			}         //work with the event message!
		}
		if (stick == -1) //if there's no input for us to get,
		{
			usleep(1); //sleep so we don't use up all those cycles!
	        	qApp->processEvents();
			continue;
		}
		if (msg.type == JS_EVENT_AXIS) //directional pad / stick
		{
			if (UseGui)
				maxzero = ((JoyKeyPad*)Stack->widget( stick ))->sensitivity();
			else
				maxzero = Items.at(Layout)->getStick(stick)->getSensitivity();
			minzero = - maxzero;
			if (msg.number == JS_VERTICAL) //Vertical axis
			{
				if (msg.value > maxzero) //Down
				{
					if (Pressed[stick][UP]) sendKey( stick, KeyRelease, UP );
					if (!Pressed[stick][DOWN]) sendKey( stick, KeyPress, DOWN );
					Pressed[stick][DOWN]=true; Pressed[stick][UP]=false;
				}
				else if (msg.value < minzero) //Up
				{
					if (Pressed[stick][DOWN]) sendKey( stick, KeyRelease, DOWN );
					if (!Pressed[stick][UP]) sendKey( stick, KeyPress, UP );
					Pressed[stick][DOWN]=false; Pressed[stick][UP]=true;
				}
				else //Neutral
				{
					if (Pressed[stick][UP]) sendKey( stick, KeyRelease, UP );
					if (Pressed[stick][DOWN]) sendKey( stick, KeyRelease, DOWN );
					Pressed[stick][DOWN]=false; Pressed[stick][UP]=false;
				}
			}
			else if (msg.number==JS_HORIZONTAL) //Horizontal axis
			{
				if (msg.value > maxzero) //Right
				{
					if (Pressed[stick][LEFT]) sendKey( stick, KeyRelease, LEFT );
					if (!Pressed[stick][RIGHT]) sendKey( stick, KeyPress, RIGHT );
					Pressed[stick][RIGHT]=true; Pressed[stick][LEFT]=false;
				}
				else if (msg.value < minzero) //Left
				{
					if (Pressed[stick][RIGHT]) sendKey( stick, KeyRelease, RIGHT );
					if (!Pressed[stick][LEFT]) sendKey( stick, KeyPress, LEFT );
					Pressed[stick][RIGHT]=false; Pressed[stick][LEFT]=true;
				}
				else //Neutral
				{
					if (Pressed[stick][RIGHT]) sendKey( stick, KeyRelease, RIGHT );
					if (Pressed[stick][LEFT]) sendKey( stick, KeyRelease, LEFT );
					Pressed[stick][RIGHT]=false; Pressed[stick][LEFT]=false;
				}
			}
		}
		else if (msg.type==JS_EVENT_BUTTON) //Buttons
		{
			if (Pressed[stick][DIRECTIONS + msg.number] != msg.value )
			{
				sendKey( stick, msg.value?KeyPress:KeyRelease, DIRECTIONS + msg.number );
				Pressed[stick][DIRECTIONS + msg.number] = msg.value;
			}
		}

		if (UseGui)
		{
			//check if any buttons on the joystick are pressed...
			joy[stick] = false;
			for (i = 0; i < AllButtons[stick]; i++)
			{
				if (Pressed[stick][i])
				{
					joy[stick] = true;
					break;
				}
			}
			//if there are buttons pressed, that means the joystick button should
			//be highlighted. So, if it isn't already highlighted, flash it!
			//Also, if it was highlighted and now isn't, flash it back to gray.
			if (joy[stick] != oldjoy[stick]) JoyStick->flash(stick);
			oldjoy[stick] = joy[stick];
		}
	}
	for (i = 0; i < JoyCount; i++)
	{
		//if (JoyDev[i]!=-1) close( JoyDev[i] ); segfaults after I close the first file! I can't imagine why  >.<
	}
}

void DMain::sendKey( int joystick, int type, int index )
{
	if (UseGui)
		((JoyKeyPad*)Stack->widget( joystick ))->sendKey( type, index );
	else
	{
		if (Layout == -1) return;
		else sendevent( type, Items.at(Layout)->getStick( joystick )->getButton( index ));
	}
}
