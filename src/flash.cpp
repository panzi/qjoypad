#include "flash.h"

//Modified here (and in .h) to not have default arguments for 2 and 3.
//This caused an error with a development version of g++ on a Mandrake system
//in Sweden.
FlashButton::FlashButton( QString text, QWidget* parent, QString name )
				:QPushButton( text, parent, name )
{
	//record the base palette for posterity.
	Normal = palette();

	//define the palette the button will have when it flashes.
	QColorGroup cg = this->palette().inactive();
	cg.setColor(QColorGroup::Button, HIGHLIGHT);
	cg.setColor(QColorGroup::Light, HIGHLIGHT.light(150));
	cg.setColor(QColorGroup::Midlight, HIGHLIGHT.light(125));
	cg.setColor(QColorGroup::Dark, HIGHLIGHT.dark(200));
	cg.setColor(QColorGroup::Mid, HIGHLIGHT.dark(150));
	Flash = QPalette(cg,cg,cg);
	isflash=false;

	setAutoDefault( false );
	setFocusPolicy(QWidget::NoFocus);
}

void FlashButton::flash()
{
	emit( flashed( !isflash ) );
	if (isflash)
	{
		setPalette( Normal );
		isflash = false;
	}
	else
	{
		setPalette( Flash );
		isflash = true;
	}
}









FlashRadioArray::FlashRadioArray( int count, QString names[], bool horizontal, QWidget* parent)
					 :QWidget( parent )
{
	if (horizontal)
		LMain = new QHBoxLayout( this, 5, 5 );
	else
		LMain = new QVBoxLayout( this, 5, 5 );
	LMain->setAutoAdd( true );
	
	Buttons = new FlashButton*[count];
	for (int i = 0; i < count; i++)
	{
		Buttons[i] = new FlashButton( names[i], this, "" );
		//when any of the buttons is clicked, it calls the same function on this.
		connect( Buttons[i], SIGNAL( clicked() ), this, SLOT( clicked() ));
	}
	
	Count = count;
	State = 0;
	Buttons[0]->setDown( true );
}

int FlashRadioArray::getState()
{
	return State;
}

void FlashRadioArray::flash( int index )
{
	Buttons[index]->flash();
}

void FlashRadioArray::clicked()
{
	//go through each button. If it wasn't the button that was just clicked,
	//then make sure that it is up. If it WAS the button that was clicked,
	//remember that index as the new state.
	for (int i = 0; i < Count; i++)
	{
		if ( Buttons[i] != sender() )
			Buttons[i]->setDown( false );
		else
		{
			State = i;
			Buttons[i]->setDown( true );
		}
	}
	emit changed( State );
}
