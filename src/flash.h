#ifndef FLASH_H
#define FLASH_H


//The color the buttons flash! Feel free to change this.
//The three numbers are all you need to touch, and they
//represent the red, green, and blue values of the color,
//respectively.

#define HIGHLIGHT QColor( 0,0,255 )


//--- --- --- --- End of editable code.




#include <qpushbutton.h>
#include <qlayout.h>


//A QPushButton that can flash a color
//The color it flashes is defined by HIGHLIGHT
//A FlashButton can now also be set dark
//in the same way that it can flash, but
//a FlashButton can not be dark and flash at the same time.
class FlashButton : public QPushButton
{
	Q_OBJECT
	public:
		FlashButton( QString text, QWidget* parent, QString name = "" );
	public slots:
		void flash();
	signals:
		void flashed( bool on );
	private:
		bool isflash;
		QPalette Normal;
		QPalette Flash;
};

//An array of flashing toggle buttons which can be toggled in a mutually exclusive fashion
class FlashRadioArray : public QWidget
{
	Q_OBJECT
	public:
		FlashRadioArray( int count, QString names[], bool horizontal, QWidget* parent);
		int getState();
	public slots: 
		void flash( int );
	private slots:
		void clicked();
	signals:
		void changed( int );
	private:
		int Count;
		int State;
		FlashButton** Buttons;
		QBoxLayout* LMain;
};

#endif
