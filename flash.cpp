#include "flash.h"


FlashButton::FlashButton( QString text, QWidget* parent = 0, QString name = "" )
				:QPushButton( text, parent, name )
{
	QColorGroup cg = this->palette().inactive();
	cg.setColor(QColorGroup::Button, HIGHLIGHT);
	cg.setColor(QColorGroup::Light, HIGHLIGHT.light(150));
	cg.setColor(QColorGroup::Midlight, HIGHLIGHT.light(125));
	cg.setColor(QColorGroup::Dark, HIGHLIGHT.dark(200));
	cg.setColor(QColorGroup::Mid, HIGHLIGHT.dark(150));
	LastPalette = QPalette(cg,cg,cg);

	setAutoDefault( false );
}

void FlashButton::flash()
{
	QPalette last = palette();
	setPalette(LastPalette);
	LastPalette = last;
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
		Buttons[i] = new FlashButton( names[i], this );
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

void FlashRadioArray::setEnabled( int index, bool enabled)
{
	Buttons[index]->setEnabled( enabled );
	if (enabled && State == index) Buttons[index]->setDown( true );
}

void FlashRadioArray::flash( int index )
{
	Buttons[index]->flash();
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
