#ifndef FORM_H
#define FORM_H



//Constants for you to edit!
#define DEVICE			"/dev/input/js"	//Where your joysticks are

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
#include <qwidgetstack.h>
#include <qslider.h>
#include <qapplication.h>
#include <qmessagebox.h>

//used for the joystick interface
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>
#include <glob.h>
#include <stdlib.h>

//Used for the types used for layouts, the function Name(int),
//and the direction constants UP, DOWN, etc.
#include "savetypes.h"

//Used to get an input key from the user
#include "keycode.h"

//Used to send keypress events to the active window
#include "sendevent.h"

//Used to have those nifty flashing buttons
#include "flash.h"


// Joystick device constants
#define JS_VERTICAL     1
#define JS_HORIZONTAL   0





//One set of buttons representing all the keys on a joystick
//This representation also holds all the keycodes and actually
//triggers the keypress events.
class JoyKeyPad : public QWidget
{
	Q_OBJECT
	public:
		JoyKeyPad( int buttons, QWidget* parent );
		void getState( JoyKeys* key );
		void flash( int index );
		void sendKey( int type, int index );
		int sensitivity();
	public slots:	
		void clear();
		void setState( JoyKeys* keys );
	private slots:
		void getKey();
		void all();
	private:
		int AllButtons;
		
		void getkey( int index );
		GetKey* KeyDialog;
		
		QGridLayout* LMain;
		QSlider* Sense;
		FlashButton** Buttons;
		int* KeyCodes;
		QPushButton* BClear;
		QPushButton* BAll;
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
		DMain( bool usegui );
		void LoadLayout();
		void ReadLoop();
		void clean();
	private slots:
		void CAdd();
		void CRem();
		void CUpdate();
		void CRevert();
	protected slots:
		void done( int );
	private:	
		void sendKey( int type, int joystick, int index );
		void Clear();
		
		int* JoyDev;
		int* AllButtons;

		QVBoxLayout* LMain;
		
		int JoyCount;
		int Layout;
		bool UseGui; 
	
		bool Finished;
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
