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
	
	Buttons[UP] = new FlashButton( "Up", this, QString::number(UP) );
	LMain->addWidget( Buttons[UP], 1, 0 );
	connect( Buttons[UP], SIGNAL( clicked() ), this, SLOT( getKey() ));
	Buttons[DOWN] = new FlashButton( "Down", this, QString::number(DOWN) );
	LMain->addWidget( Buttons[DOWN], 1, 1 );
	connect( Buttons[DOWN], SIGNAL( clicked() ), this, SLOT( getKey() ));
	Buttons[LEFT] = new FlashButton( "Left", this, QString::number(LEFT) );
	LMain->addWidget( Buttons[LEFT], 2, 0 );
	connect( Buttons[LEFT], SIGNAL( clicked() ), this, SLOT( getKey() ));
	Buttons[RIGHT] = new FlashButton( "Right", this, QString::number(RIGHT) );
	LMain->addWidget( Buttons[RIGHT], 2, 1 );
	connect( Buttons[RIGHT], SIGNAL( clicked() ), this, SLOT( getKey() ));
	
	for (int i = 0; i < BUTTONS; i++)
	{
		Buttons[i] = new FlashButton( "Buttons " + QString::number( i + 1 ), this, QString::number(i) );
		LMain->addWidget( Buttons[i], 3 + i / 2, i % 2);
		connect( Buttons[i], SIGNAL( clicked() ), this, SLOT( getKey() ));
	}
	
	BClear = new QPushButton( "Clear", this );
	BClear->setAutoDefault( false );
	LMain->addWidget( BClear, 3 + BUTTONS / 2, 0 );
	connect( BClear, SIGNAL( clicked() ), this, SLOT( clear() ));
	
	BAll = new QPushButton( "Set all keys", this );
	BAll->setAutoDefault( false );
	LMain->addWidget( BAll, 3 + BUTTONS / 2, 1 );
	connect( BAll, SIGNAL( clicked() ), this, SLOT( all() ));
	
	Key = 0;
}

JoyKeys JoyKeyPad::getState()
{
	JoyKeys result;
	for (int i = 0; i < BUTTONS + 4; i++)
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

void JoyKeyPad::key( int type, int index )
{
	//if the main dialog isn't the active window,
	//and either the getkey dialog is not running
	//or is not the active window, send a keypress.
	//This is to keep the user from causing interesting
	//effects by triggering QJoyPad with the events
	//produced BY QJoyPad.
	if (!isActiveWindow() && (!Key || !Key->isActiveWindow())) sendevent(type, KeyCodes[index]); 
	flash( index );
}

int JoyKeyPad::sensitivity()
{
	return Sense->value();
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
						fullname = "Button " + QString::number( index + 1);
	}
	return fullname;
}

void JoyKeyPad::getKey()
{
	getkey( QString(sender()->name()).toInt() );
}

void JoyKeyPad::setState( JoyKeys keys )
{
	for (int i = 0; i < BUTTONS + 4; i++)
	{
		KeyCodes[i] = keys.Buttons[i];
		Buttons[i]->setText( Name(i) + " : " + ktos( KeyCodes[i] ) );
		Sense->setValue( keys.Sensitivity );
	}
}

void JoyKeyPad::clear()
{
	for (int i = 0; i < BUTTONS + 4; i++)
	{
		KeyCodes[i] = 0;
		Buttons[i]->setText( Name(i) + " : " + ktos( 0 ));
	}
}

void JoyKeyPad::getkey( int index )
{
	QString fullname = Name(index);
	Key = new GetKey( fullname );
	Key->exec();
	
	int keycode = Key->result();
	QString keyname = ktos( keycode );

	delete Key;
	Key = 0;
	
	if (keyname)
	{
		KeyCodes[index] = keycode;
		Buttons[index]->setText( fullname + " : " + keyname );
	}
}

void JoyKeyPad::all()
{
	for (int index = BUTTONS; index < BUTTONS + 4; index++)
	{
		getkey( index );
	}
	for (int index = 0; index < BUTTONS; index++)
	{
		getkey( index );
	}
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
	file = open(QString( DEVICE ) + QString::number(Index), O_RDONLY);
	if (file==-1) return;
	
	Sticks->setEnabled( Index, true );
	Pad->setEnabled( true );
	
	unsigned int len;
	int value;
	js_event msg;

	bool finished = false;
	bool Pressed[BUTTONS + 4];
	bool oldjoy = false;
	bool joy = false;
	
	int minzero, maxzero;
	
	for (int i = 0; i < BUTTONS + 4; i++)
	{
		Pressed[i] = false;
	}


	while (!finished)
	{
		len = read( file, &msg, sizeof(js_event));
		if (len < sizeof(js_event))
		{
			finished = true;
			continue;
		}
		if (msg.type == JS_EVENT_AXIS) //directional pad / stick
		{
			value = msg.value;

			maxzero = Pad->sensitivity();
			minzero = - maxzero;
			if (msg.number == JS_VERTICAL) //Vertical axis
			{
				if (value > maxzero) //Down
				{
					if (Pressed[UP]) Pad->key(KeyRelease, UP);
					if (!Pressed[DOWN]) Pad->key(KeyPress, DOWN);
					Pressed[DOWN]=true; Pressed[UP]=false;
				}
				else if (value < minzero) //Up
				{
					if (Pressed[DOWN]) Pad->key(KeyRelease, DOWN);
					if (!Pressed[UP]) Pad->key(KeyPress, UP);
					Pressed[DOWN]=false; Pressed[UP]=true;
				}
				else //Neutral
				{
					if (Pressed[UP]) Pad->key(KeyRelease, UP);
					if (Pressed[DOWN]) Pad->key(KeyRelease, DOWN);
					Pressed[DOWN]=false; Pressed[UP]=false;
				}
			}
			
			if (msg.number==JS_HORIZONTAL) //Horizontal axis
			{
				if (value > maxzero) //Right
				{
					if (Pressed[LEFT]) Pad->key(KeyRelease, LEFT);
					if (!Pressed[RIGHT]) Pad->key(KeyPress, RIGHT);
					Pressed[RIGHT]=true; Pressed[LEFT]=false;
				}
				else if (value < minzero) //Left
				{
					if (Pressed[RIGHT]) Pad->key(KeyRelease, RIGHT);
					if (!Pressed[LEFT]) Pad->key(KeyPress, LEFT);
					Pressed[RIGHT]=false; Pressed[LEFT]=true;
				}
				else //Neutral
				{
					if (Pressed[RIGHT]) Pad->key(KeyRelease, RIGHT);
					if (Pressed[LEFT]) Pad->key(KeyRelease, LEFT);
					Pressed[RIGHT]=false; Pressed[LEFT]=false;
				}
			}
		}
		
		if (msg.type==JS_EVENT_BUTTON) //Buttons
		{
			if (Pressed[msg.number] != msg.value )
			{
				Pad->key( msg.value?KeyPress:KeyRelease, msg.number);
				Pressed[msg.number] = msg.value;
			}
		}
		
		
		
		joy = false;
		for (int i = 0; i < BUTTONS + 4; i++)
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
	Sticks->setEnabled( Index, false );
	Pad->setEnabled( false );
}

void JoyThread::clean()
{
	//file->close();
	close(file);
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

	CCombo = new QComboBox( CFrame );
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
		JoyStick->setEnabled( i, false );
	
		Stack->addWidget( new JoyKeyPad( Stack ));
		Stack->widget( i )->setEnabled( false );
		
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
			for (button = BUTTONS; button < BUTTONS + 4; button++)
			{
				stream << (*Items.at(index))[joy].Buttons[button];
			}
			for (button = 0; button < BUTTONS; button++)
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
			for (button = BUTTONS; button < BUTTONS + 4; button++)
			{
				if (joy < joysticks)
					stream >> (*newitem)[joy].Buttons[button];
				else (*newitem)[joy].Buttons[button] = 0;
			}
			for (button = 0; button < BUTTONS; button++ )
			{
				if (button < buttons + 4 && joy < joysticks)
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
		Thread[i]->terminate();
		Thread[i]->clean();
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
