#include "flash.h"

//Modified here (and in .h) to not have default arguments for 2 and 3.
//This caused an error with a development version of g++ on a Mandrake system
//in Sweden.
FlashButton::FlashButton( QString text, QWidget* parent, QString name )
				:QPushButton( text, parent, name )
{
	Normal = palette();

	QColorGroup cg = this->palette().inactive();
	cg.setColor(QColorGroup::Button, HIGHLIGHT);
	cg.setColor(QColorGroup::Light, HIGHLIGHT.light(150));
	cg.setColor(QColorGroup::Midlight, HIGHLIGHT.light(125));
	cg.setColor(QColorGroup::Dark, HIGHLIGHT.dark(200));
	cg.setColor(QColorGroup::Mid, HIGHLIGHT.dark(150));
	Flash = QPalette(cg,cg,cg);
	isflash=false;

	cg.setColor( QColorGroup::Button,   Normal.color( QPalette::Normal, QColorGroup::Button).dark(125));
	cg.setColor( QColorGroup::Light,    Normal.color( QPalette::Normal, QColorGroup::Light).dark(125));
	cg.setColor( QColorGroup::Midlight, Normal.color( QPalette::Normal, QColorGroup::Midlight).dark(125));
	cg.setColor( QColorGroup::Dark,     Normal.color( QPalette::Normal, QColorGroup::Dark).dark(125));
	cg.setColor( QColorGroup::Mid,      Normal.color( QPalette::Normal, QColorGroup::Mid).dark(125));
	Dark = QPalette(cg,cg,cg);
	isdark=false;

	setAutoDefault( false );
}

void FlashButton::flash()
{
	if (isdark) return;
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

void FlashButton::dark()
{
	if (isdark)
	{
		setPalette( Normal );
		isdark = false;
	}
	else
	{
		setPalette( Dark );
		isdark = true;
		isflash = false;
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

void FlashRadioArray::dark( int index )
{
	Buttons[index]->dark();
}

void FlashRadioArray::clicked()
{
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
