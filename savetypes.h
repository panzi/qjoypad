#ifndef SAVETYPES_H
#define SAVETYPES_H

#include <qintdict.h>
#include <qstring.h>
#include <qfile.h>
#include <qstringlist.h>
#include <qregexp.h>
#include <qmessagebox.h>
#include <qdir.h>


#define UP				0
#define DOWN			1
#define LEFT			2
#define RIGHT			3
#define DIRECTIONS	4


//Converts a button index into a button name (ie 0 -> "Up", 4 -> "Button 1")
QString Name( int index );



//One keycode layout to represent one joystick.
class JoyKeys
{
	public:
		JoyKeys();
		
		int read( QTextStream* stream );
		void write( QTextStream* stream );
		
		int getButton( int index );
		void setButton( int index, int value );
		
		int getSensitivity();
		void setSensitivity( int value );
	private:
		QIntDict<int> Buttons;
		int Sensitivity;
};

//One save state consisting of one layout for each joystick
class OneSave
{
	public:
		OneSave();
		
		bool open( QString filename );
		void save();
		void remove();
		
		JoyKeys* getStick( int index );
		void setStick( int index, JoyKeys* keys );

		QString name();
		void setName( QString newname );
		
		bool hasChanged();
		void setChanged();
	private:
		QIntDict<JoyKeys> Sticks;
		QString Name;
		QFile* File;
		bool Changed;
};


#endif
