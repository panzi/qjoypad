#ifndef QJOYPAD_FLASH_H
#define QJOYPAD_FLASH_H


//The color the buttons flash! Feel free to change this.
//The three numbers are all you need to touch, and they
//represent the red, green, and blue values of the color,
//respectively.

#define HIGHLIGHT QColor( 0,0,255 )


//--- --- --- --- End of editable code.




#include <QPushButton>
#include <QPalette>
#include <QBoxLayout>
#include <QStringList>


//A QPushButton that can flash a color
//The color it flashes is defined by HIGHLIGHT
//A FlashButton can now also be set dark
//in the same way that it can flash, but
//a FlashButton can not be dark and flash at the same time.
class FlashButton : public QPushButton
{
	Q_OBJECT
	public:
        FlashButton(QString text, QString name = "" , QWidget* parent = 0);
	public slots:
		//make the button turn blue if it was gray, or visa versa.
		void flash();
	signals:
		//let the world know when flash() happens!
		void flashed( bool on );
	private:
		//is this currently "flashed" (ie, colored blue)?
		bool isflash;
		//the normal, unflashed palette
		QPalette Normal;
		//the colorful flashed palette
		QPalette Flash;
};

//An array of flashing mutually-exclusive toggle buttons.
class FlashRadioArray : public QWidget
{
	Q_OBJECT
	public:
        FlashRadioArray( const QStringList &names, bool horizontal, QWidget* parent);
		//returns an integer returning the currently selected button.
		//First button is 0.
		int getState();
	public slots: 
		//flash the button at the given index
		void flash( int );
	private slots:
		//this is what happens when one of the buttons in the array is clicked.
		void clicked();
	signals:
		//happens when the state changes. The int is the new state.
		void changed( int );
    private:
		//which is currently down
        int state;
		//the array of buttons
        QList<FlashButton*> buttons;
		//the main layout.
        QBoxLayout* mainLayout;
};

#endif
