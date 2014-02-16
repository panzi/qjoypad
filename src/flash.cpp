#include "flash.h"
//Added by qt3to4:

//Modified here (and in .h) to not have default arguments for 2 and 3.
//This caused an error with a development version of g++ on a Mandrake system
//in Sweden.
FlashButton::FlashButton( QString text, QString name, QWidget* parent )
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
        mainLayout = new QHBoxLayout( this);
        mainLayout->setMargin(5);
        mainLayout->setSpacing(5);
    } else {
        mainLayout = new QVBoxLayout( this);
        mainLayout->setMargin(5);
        mainLayout->setSpacing(5);
    }
    foreach (const QString &name, names) {
        FlashButton *button = new FlashButton( name, QString::null, this );
        buttons.append(button);
        //when any of the buttons is clicked, it calls the same function on this.
        connect( button, SIGNAL( clicked() ), this, SLOT( clicked() ));
        mainLayout->addWidget(button);
    }

    state = 0;
    if (!buttons.isEmpty()) {
        buttons[0]->setDown( true );
    }
}

int FlashRadioArray::getState()
{
    return state;
}

void FlashRadioArray::flash( int index )
{
    if (index < buttons.size()) {
        buttons[index]->flash();
    }
}

void FlashRadioArray::clicked()
{
    //go through each button. If it wasn't the button that was just clicked,
    //then make sure that it is up. If it WAS the button that was clicked,
    //remember that index as the new state.
    int i = 0;
    foreach (FlashButton *button, buttons) {
        if ( button != sender() ) {
            button->setDown( false );
        }
        else {
            state = i;
            button->setDown( true );
        }
        ++ i;
    }
    emit changed( state );
}
