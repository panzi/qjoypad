#include "flash.h"
//Added by qt3to4:

//Modified here (and in .h) to not have default arguments for 2 and 3.
//This caused an error with a development version of g++ on a Mandrake system
//in Sweden.
FlashButton::FlashButton( QString text, QWidget* parent, QString name )
        :QPushButton( text, parent )
{
    this->setObjectName(name);
    //record the base palette for posterity.
    Normal = palette();

    //define the palette the button will have when it flashes.
    QPalette cg = this->palette();
    cg.setCurrentColorGroup(QPalette::Inactive);
    cg.setColor(QPalette::Button, HIGHLIGHT);
    cg.setColor(QPalette::Light, HIGHLIGHT.light(150));
    cg.setColor(QPalette::Midlight, HIGHLIGHT.light(125));
    cg.setColor(QPalette::Dark, HIGHLIGHT.dark(200));
    cg.setColor(QPalette::Mid, HIGHLIGHT.dark(150));
    Flash = cg;
    isflash=false;

    setAutoDefault( false );
    setFocusPolicy(Qt::NoFocus);
    
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


FlashRadioArray::FlashRadioArray( const QStringList &names, bool horizontal, QWidget* parent)
        :QWidget( parent )
{
    if (horizontal) {
        LMain = new QHBoxLayout( this);
        LMain->setMargin(5);
        LMain->setSpacing(5);
    } else {
        LMain = new QVBoxLayout( this);
        LMain->setMargin(5);
        LMain->setSpacing(5);
    }
// TODO: fix memleak
    int count = names.size();
    int i = 0;
    Buttons = new FlashButton*[count];
    foreach (const QString &name, names) {
        Buttons[i] = new FlashButton( name, this, "" );
        //when any of the buttons is clicked, it calls the same function on this.
        connect( Buttons[i], SIGNAL( clicked() ), this, SLOT( clicked() ));
        LMain->addWidget(Buttons[i]);
        ++i;
    }

    Count = count;
    State = 0;
    if (count > 0) {
        Buttons[0]->setDown( true );
    }
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
