#include "form.h"

extern Display* display;

JoyKeyPad::JoyKeyPad( QWidget* parent )
			 :QWidget( parent )
{
	LMain = new QGridLayout( this, (BUTTONS + 4) / 2, 2, 5, 5);
	
	Sense = new QSlider( 0, 32766, 100, 7000, Qt::Horizontal, this );
	Sense->setTickmarks( (QSlider::TickSetting) 2 );
	Sense->setTickInterval( 4096 );
	LMain->addMultiCellWidget( Sense, 0,0,0,1);
	
	for (int i = 0; i < ALLBUTTONS; i++)
	{
		Buttons[i] = new FlashButton( Name( i ), this, QString::number(i) );
		LMain->addWidget( Buttons[i], 1 + i / 2, i % 2);
		connect( Buttons[i], SIGNAL( clicked() ), this, SLOT( getKey() ));
	}
	
	BClear = new QPushButton( "Clear", this );
	BClear->setAutoDefault( false );
	LMain->addWidget( BClear, 1 + ALLBUTTONS / 2, 0 );
	connect( BClear, SIGNAL( clicked() ), this, SLOT( clear() ));
	
	BAll = new QPushButton( "Set all keys", this );
	BAll->setAutoDefault( false );
	LMain->addWidget( BAll, 1 + ALLBUTTONS / 2, 1 );
	connect( BAll, SIGNAL( clicked() ), this, SLOT( all() ));
	
	KeyDialog = 0;
}

JoyKeys JoyKeyPad::getState()
{
	JoyKeys result;
	for (int i = 0; i < ALLBUTTONS; i++)
	{
		result.Buttons[i] = KeyCodes[i];
	}
	result.Sensitivity = Sense->value();
	return result;
}

void JoyKeyPad::flash( int index )
{
	Buttons[index]->flash();
}

void JoyKeyPad::dark( int count )
{
	if (count == -1)
		count = ALLBUTTONS;
	else
		count += DIRECTIONS;
		
	for (int i = 0; i < count; i++)
	{
		Buttons[i]->dark();
	}
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
	if (!isActiveWindow() && (!KeyDialog || !KeyDialog->isActiveWindow())) sendevent(type, KeyCodes[index]); 
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

void JoyKeyPad::setState( JoyKeys keys )
{
	for (int i = 0; i < ALLBUTTONS; i++)
	{
		KeyCodes[i] = keys.Buttons[i];
		Buttons[i]->setText( Name(i) + " : " + ktos( KeyCodes[i] ) );
		Sense->setValue( keys.Sensitivity );
	}
}

void JoyKeyPad::clear()
{
	for (int i = 0; i < ALLBUTTONS; i++)
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
	for (int index = 0; index < ALLBUTTONS; index++)
	{
		getkey( index );
	}
}

QString JoyKeyPad::Name( int index )
{
	QString fullname;
	
	switch (index)
	{
		case UP: 	fullname = "Up";
						break;
		case DOWN: 	fullname = "Down";
						break;
		case LEFT:	fullname = "Left";
						break;
		case RIGHT:	fullname = "Right";
						break;
		default:
						fullname = "Button " + QString::number( index - DIRECTIONS + 1);
	}
	return fullname;
}











JoyThread::JoyThread( int index, FlashRadioArray* sticks, JoyKeyPad* pad )
{
	Index = index;
	Sticks = sticks;
	Pad = pad;
}

void JoyThread::run()
{
	//can't use qfile here as it doesn't allow reading of arbitrary data types.
	JoyDev = open(QString( DEVICE ) + QString::number(Index), O_RDONLY);
	if (JoyDev==-1) return; //if can't access the device, give up that device.
	
	Sticks->dark( Index );//show that the joystick is active

	//show which buttons the joystick api thinks are available.
	char buttons;
	ioctl(JoyDev, JSIOCGBUTTONS, &buttons);
	Pad->dark( buttons );
	
	unsigned int len;
	js_event msg;

	bool Pressed[ALLBUTTONS];
	for (int i = 0; i < ALLBUTTONS; i++)
	{
		Pressed[i] = false;
	}


	//used to decide whether or not to flash the joystick button
	bool oldjoy = false;
	bool joy = false;
	
	int minzero, maxzero;


	while (true)
	{
		//read in one event
		len = read( JoyDev, &msg, sizeof(js_event));
		if (len < sizeof(js_event)) //if it's... not right
		{
			close(JoyDev);
			break; //quit.
		}
		
		if (msg.type == JS_EVENT_AXIS) //directional pad / stick
		{
			maxzero = Pad->sensitivity();
			minzero = - maxzero;
			if (msg.number == JS_VERTICAL) //Vertical axis
			{
				if (msg.value > maxzero) //Down
				{
					if (Pressed[UP]) Pad->sendKey(KeyRelease, UP);
					if (!Pressed[DOWN]) Pad->sendKey(KeyPress, DOWN);
					Pressed[DOWN]=true; Pressed[UP]=false;
				}
				else if (msg.value < minzero) //Up
				{
					if (Pressed[DOWN]) Pad->sendKey(KeyRelease, DOWN);
					if (!Pressed[UP]) Pad->sendKey(KeyPress, UP);
					Pressed[DOWN]=false; Pressed[UP]=true;
				}
				else //Neutral
				{
					if (Pressed[UP]) Pad->sendKey(KeyRelease, UP);
					if (Pressed[DOWN]) Pad->sendKey(KeyRelease, DOWN);
					Pressed[DOWN]=false; Pressed[UP]=false;
				}
			}
			
			if (msg.number==JS_HORIZONTAL) //Horizontal axis
			{
				if (msg.value > maxzero) //Right
				{
					if (Pressed[LEFT]) Pad->sendKey(KeyRelease, LEFT);
					if (!Pressed[RIGHT]) Pad->sendKey(KeyPress, RIGHT);
					Pressed[RIGHT]=true; Pressed[LEFT]=false;
				}
				else if (msg.value < minzero) //Left
				{
					if (Pressed[RIGHT]) Pad->sendKey(KeyRelease, RIGHT);
					if (!Pressed[LEFT]) Pad->sendKey(KeyPress, LEFT);
					Pressed[RIGHT]=false; Pressed[LEFT]=true;
				}
				else //Neutral
				{
					if (Pressed[RIGHT]) Pad->sendKey(KeyRelease, RIGHT);
					if (Pressed[LEFT]) Pad->sendKey(KeyRelease, LEFT);
					Pressed[RIGHT]=false; Pressed[LEFT]=false;
				}
			}
		}
		
		if (msg.type==JS_EVENT_BUTTON) //Buttons
		{
			if (Pressed[DIRECTIONS + msg.number] != msg.value )
			{
				Pad->sendKey( msg.value?KeyPress:KeyRelease, DIRECTIONS + msg.number);
				Pressed[DIRECTIONS + msg.number] = msg.value;
			}
		}
		
		
		joy = false;
		for (int i = 0; i < ALLBUTTONS; i++)
		{
			if (Pressed[i])
			{
				joy = true;
				break;
			}
		}
		if (joy != oldjoy) Sticks->flash(Index);
		oldjoy = joy;
	}
}

void JoyThread::clean()
{
	//JoyDev->close();
	close(JoyDev);
}








ModCombo::ModCombo( QWidget* parent )
			:QComboBox( parent )
{
}

void ModCombo::keyPressEvent( QKeyEvent* )
{
}









DMain::DMain()
		:QDialog( 0 )
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
	
	int i;
	
	QString sticks[JOYSTICKS];
	for (i = 1; i <= JOYSTICKS; i++)
	{
		sticks[i - 1] = "Joystick " + QString::number(i);
	}
	JoyStick = new FlashRadioArray( JOYSTICKS, sticks, true, this );
	
	Stack = new QWidgetStack( this );
	Stack->setFrameStyle( QFrame::Box | QFrame::Raised );
	
	for (i = 0; i < JOYSTICKS; i++)
	{
		JoyStick->dark( i );
		Stack->addWidget( new JoyKeyPad( Stack ));
		((JoyKeyPad*)Stack->widget( i ))->dark( -1 );
		
		Thread[i] = new JoyThread( i, JoyStick, ((JoyKeyPad*)Stack->widget( i )));
		Thread[i]->start();
	}

	connect( JoyStick, SIGNAL( changed( int )), Stack, SLOT( raiseWidget( int )));
	CLoad();

	display=XOpenDisplay(NULL);
}

void DMain::CAdd()
{
	QString newname = QInputDialog::getText( "Choose name for a new layout", "Please enter a name for the current layout:" );
	if (newname != "")
	{
		CCombo->insertItem( newname );
		CCombo->setCurrentText( newname );
		OneSave* result = (OneSave*)(new OneSave);
		for (int i = 0; i < JOYSTICKS; i++)
		{
			(*result)[i] = ((JoyKeyPad*)Stack->widget( i ))->getState();
		}
		Items.append( result );
	}
}

void DMain::CRem()
{
	int index = CCombo->currentItem();
	if (index != -1)
	{
		CCombo->removeItem( index );
		Items.remove( index );
		if (index != 0)
		{
			CCombo->setCurrentItem( index - 1 );
			CRevert();
		}
		else Clear();
	}
}

void DMain::CUpdate()
{
	int index = CCombo->currentItem();
	OneSave* newone = (OneSave*)(new OneSave);
	for (int i = 0; i < JOYSTICKS; i++)
	{
		(*newone)[i] = ((JoyKeyPad*)Stack->widget( i ))->getState();
	}
	Items.replace( index, newone );
}

void DMain::CRevert()
{
	int index = CCombo->currentItem();
	OneSave* old = Items.at( index );
	for (int i = 0; i < JOYSTICKS; i++)
	{
		((JoyKeyPad*)Stack->widget( i ))->setState( (*old)[i] );
	}
}

void DMain::CSave()
{
	QString filename = QString(getenv( "HOME" )) + "/.qjoypadrc";
	QFile file( filename );
	if (!file.open( IO_WriteOnly ))
	{
		QMessageBox::about(this, "Error saving file", "Could not save " + filename);
		return;
	}
	QDataStream stream( &file );
	int index, joy, button;
	stream << BUTTONS << JOYSTICKS << CCombo->count();
	for (index = 0; index < CCombo->count(); index++)
	{
		stream << CCombo->text( index );
		for (joy = 0; joy < JOYSTICKS; joy++)
		{
			for (button = 0; button < ALLBUTTONS; button++)
			{
				stream << (*Items.at(index))[joy].Buttons[button];
			}
			stream << (*Items.at(index))[joy].Sensitivity;
		}
	}
	file.close();
}

void DMain::CLoad()
{
	QString filename = QString(getenv( "HOME" )) + "/.qjoypadrc";
	QFile file( filename );
	if (! file.open( IO_ReadOnly ))
	{
		Clear();
		return;
	}
	QDataStream stream( &file );
	int buttons, joysticks, count;
	stream >> buttons >> joysticks >> count;
	if (buttons != BUTTONS || joysticks != JOYSTICKS)
	{
		QMessageBox::about(this,
		"Error reading save file",
		"The current save file, " + filename + ", was saved by a different\nversion of this program with different maximum numbers of joysticks or\nbuttons. I will try to load it now, but it's likely that some data will be lost.");
	}
		
	QString newname;
	OneSave* newitem;
	
	int joy;
	int button;
	
	for (int i = 0; i < count; i++)
	{
		newitem = (OneSave*)(new OneSave);
		stream >> newname;
		for (joy = 0; joy < JOYSTICKS; joy++)
		{
			for (button = 0; button < ALLBUTTONS; button++)
			{
				if (button < buttons + DIRECTIONS && joy < joysticks)
					stream >> (*newitem)[joy].Buttons[button];
				else (*newitem)[joy].Buttons[button] = 0;
			}
			if (buttons > BUTTONS)
			{
				int sink;
				for (int j = BUTTONS; j < buttons; j++)
				{
					stream >> sink;
				}
			}
			if (joy < joysticks) stream >> (*newitem)[joy].Sensitivity;
		}
		CCombo->insertItem( newname );
		Items.append( newitem);
	}
	if (CCombo->count() != 0)
	{
		CCombo->setCurrentItem( 0 );
		CRevert();
	}
	else Clear();
	file.close();
}

void DMain::done( int r )
{
	for (int i = 0; i < JOYSTICKS; i++)
	{
		if (Thread[i]->running())
		{
			Thread[i]->terminate();
			Thread[i]->clean();
		}
	}
	XCloseDisplay( display );
	CSave();
	QDialog::done( r );
}

void DMain::Clear()
{
	for (int i = 0; i < JOYSTICKS; i++)
	{
		((JoyKeyPad*)Stack->widget( i ))->clear();
	}
}
