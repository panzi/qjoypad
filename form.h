#ifndef FORM_H
#define FORM_H



//Constants for you to edit!
#define DEVICE			"/dev/input/js"	//Where your joysticks are
#define BUTTONS		10						//Maximum number of buttons on a joystick
#define JOYSTICKS		4						//Maximum number of joysticks

//Also, if you don't like the default color for the flashing buttons,
//you can go into flash.h and change HIGHLIGHT to any color you wish.


//--- --- --- --- End of editable code.






//From the QT Libraries
#include <qdialog.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qptrlist.h>
#include <qinputdialog.h>
#include <qfile.h>
#include <qwidgetstack.h>
#include <qthread.h>
#include <qmessagebox.h>
#include <qslider.h>

//Used to grab the user's home directory
#include <stdlib.h>

//used for the joystick interface
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>

//Used to get an input key from the user
#include "keycode.h"

//Used to send keypress events to the active window
#include "sendevent.h"

//Used to have those nifty flashing buttons
#include "flash.h"

//Don't edit these constants ;)
#define UP				0
#define DOWN			1
#define LEFT			2
#define RIGHT			3
#define DIRECTIONS	4
#define ALLBUTTONS	BUTTONS + DIRECTIONS



// Joystick device constants
#define JS_VERTICAL     1
#define JS_HORIZONTAL   0


#define NAME			"QJoyPad 1.4"		//The title on the title bar!


//One keycode layout to represent one joystick.
struct JoyKeys
{
	int Buttons[ALLBUTTONS];
	int Sensitivity;
};

//One save state consisting of one layout for each joystick
typedef JoyKeys OneSave[JOYSTICKS];

//One set of buttons representing all the keys on a joystick
//This representation also holds all the keycodes and actually
//triggers the keypress events.
class JoyKeyPad : public QWidget
{
	Q_OBJECT
	public:
		JoyKeyPad( QWidget* parent );
		JoyKeys getState();
		void flash( int index );
		void dark( int count );
		void sendKey( int type, int index );
		int sensitivity();
	public slots:	
		void clear();
		void setState( JoyKeys keys );
	private slots:
		void getKey();
		void all();
	private:
		QString Name( int index );
		void getkey( int index );
		
		GetKey* KeyDialog;
		QGridLayout* LMain;
		QSlider* Sense;
		FlashButton* Buttons[ALLBUTTONS];
		int KeyCodes[ALLBUTTONS];
		QPushButton* BClear;
		QPushButton* BAll;
};

//A thread used to monitor a joystick device and produce proper results.
class JoyThread : public QThread
{
	public:
		JoyThread( int index, FlashRadioArray* sticks, JoyKeyPad* pad );
		virtual void run();
		void clean();
	private:
		int Index;
		int JoyDev;
		FlashRadioArray* Sticks;
		JoyKeyPad* Pad;
};


//isn't it sad that I have to define a new class just to avoid
//accidentally triggering the ComboBox with an accidental keypress?
class ModCombo : public QComboBox
{
	public:
		ModCombo( QWidget * parent );
	protected:
		void keyPressEvent( QKeyEvent* );
};

//The main dialog and the editable combo box
class DMain : public QDialog
{
	Q_OBJECT
	public:
		DMain();
	private slots:
		void CAdd();
		void CRem();
		void CUpdate();
		void CRevert();
	public slots:
		void done( int );
	private:	
		void Clear();
		QVBoxLayout* LMain;
		
		JoyThread* Thread[JOYSTICKS];
	
		QFrame* CFrame;
		QGridLayout* CLayout;
		ModCombo* CCombo;
		QPushButton* CBAdd;
		QPushButton* CBRem;
		QPushButton* CBUpdate;
		QPushButton* CBRevert;
		QPtrList<OneSave> Items;
		void CSave();
		void CLoad();
		
		FlashRadioArray* JoyStick;
		QWidgetStack* Stack;
};


#endif
