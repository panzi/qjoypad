#include "layout_edit.h"

//build the dialog
LayoutEdit::LayoutEdit( LayoutManager* l ): QWidget(NULL) {
    lm = l;
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle( NAME );
    setWindowIcon(QPixmap(ICON24));

    LMain = new QVBoxLayout( this);
    LMain->setSpacing(5);
    LMain->setMargin(5);

    QFrame* frame = new QFrame(this);
    frame->setFrameStyle(QFrame::Box | QFrame::Sunken );
    QGridLayout* g = new QGridLayout(frame);
    g->setMargin(5);
    g->setSpacing(5);
    CLayouts = new QComboBox(frame);
    connect( CLayouts, SIGNAL(activated( const QString& )), lm, SLOT(load(const QString&)));
    g->addWidget(CLayouts,0,0,1,4);

    //most of these buttons can link directly into slots in the LayoutManager
    BAdd = new QPushButton("Add", frame);
    connect(BAdd, SIGNAL(clicked()), lm, SLOT(saveAs()));
    g->addWidget(BAdd,1,0);
    BRem = new QPushButton("Remove", frame);
    connect(BRem, SIGNAL(clicked()), lm, SLOT(remove()));
    g->addWidget(BRem,1,1);
    BUpd = new QPushButton("Update", frame);
    connect(BUpd, SIGNAL(clicked()), lm, SLOT(save()));
    g->addWidget(BUpd,1,2);
    BRev = new QPushButton("Revert", frame);
    connect(BRev, SIGNAL(clicked()), lm, SLOT(reload()));
    g->addWidget(BRev,1,3);
    LMain->addWidget( frame );

    //produce a list of names for the FlashRadioArray
    //this is only necesary since joystick devices need not always be
    //contiguous
    QStringList names;
    foreach (JoyPad *joypad, available) {
        names.append(joypad->getName());
        connect(this, SIGNAL(focusStateChanged(bool)), joypad, SLOT(focusChange(bool)));
    }
    
    //flash radio array
    JoyButtons = new FlashRadioArray(names, true, this );
    LMain->addWidget( JoyButtons );
    
    //we have a WidgetStack to represent the multiple joypads
    PadStack = new QStackedWidget( this );
    PadStack->setFrameStyle(QFrame::Box | QFrame::Sunken );
    LMain->addWidget(PadStack);

    //go through each of the available joysticks
    // i is the current index into PadStack
    int i = 0;
    foreach (JoyPad *joypad, available) {
        //add a new JoyPadWidget to the stack
        PadStack->insertWidget( i, joypad->widget(PadStack,i) );
        //every time it "flashes", flash the associated tab.
        connect( PadStack->widget(i), SIGNAL( flashed( int ) ), JoyButtons, SLOT( flash( int )));
        ++i;
    }
    //whenever a new tab is selected, raise the appropriate JoyPadWidget
    connect( JoyButtons, SIGNAL( changed( int ) ), PadStack, SLOT( setCurrentIndex( int )));

    updateLayoutList();

    //add the buttons at the bottom.
    QHBoxLayout* h = new QHBoxLayout(0);
    h->setMargin(0);
    h->setSpacing(5);
    QPushButton* close = new QPushButton( "-- Close Dialog --", this );
    connect(close, SIGNAL(clicked()), this, SLOT(close()));
    h->addWidget(close);
    QPushButton* quit = new QPushButton( "-- Quit --", this );
    connect( quit, SIGNAL( clicked() ), qApp, SLOT(quit()));
    h->addWidget(quit);
    LMain->addLayout(h);
    connect(qApp, SIGNAL(focusChanged ( QWidget * , QWidget *  ) ), this, 
        SLOT(appFocusChanged(QWidget *, QWidget *)));
    this->show();
}

void LayoutEdit::setLayout(QString layout) {
    //change the text,
    CLayouts->setCurrentIndex(lm->getLayoutNames().indexOf(layout));
    //update all the JoyPadWidgets.
    for (int i = 0; i < available.count(); i++) {
        ((JoyPadWidget*)PadStack->widget(i))->update();
    }
}

void LayoutEdit::updateLayoutList() {
    //blank the list, then load in new names from the LayoutManager.
    CLayouts->clear();
    QStringList layouts = lm->getLayoutNames();
    CLayouts->insertItems(-1, layouts);
    CLayouts->setCurrentIndex(layouts.indexOf(lm->currentLayout));
}

void LayoutEdit::updateJoypadWidgets() {
    int indexOfFlashRadio = LMain->indexOf(JoyButtons);
    FlashRadioArray *newJoyButtons;
    QStringList names;
    foreach (JoyPad *joypad, available) {
        names.append(joypad->getName());
    }
    
    newJoyButtons = new FlashRadioArray( names, true, this );
    LMain->insertWidget(indexOfFlashRadio, newJoyButtons);
    LMain->removeWidget(JoyButtons);
    FlashRadioArray* oldJoyButtons = JoyButtons;
    JoyButtons = newJoyButtons;
    connect( JoyButtons, SIGNAL( changed( int ) ), PadStack, SLOT( setCurrentIndex( int )));
    oldJoyButtons->deleteLater();
    int numberOfJoypads = PadStack->count();
    for(int i = 0; i<numberOfJoypads; i++) {
        PadStack->removeWidget(PadStack->widget(0));
    }
    int i = 0;
    foreach (JoyPad *joypad, available) {
        //add a new JoyPadWidget to the stack
        PadStack->insertWidget( i, joypad->widget(PadStack,i) );
        //every time it "flashes", flash the associated tab.
        connect( PadStack->widget(i), SIGNAL( flashed( int ) ), JoyButtons, SLOT( flash( int )));
        ++i;
    }
}

void LayoutEdit::closeEvent(QCloseEvent *event) {
    lm->leWindowClosed();
    event->accept();
}

void LayoutEdit::appFocusChanged(QWidget *old, QWidget *now) {
    if(now!=NULL && old==NULL) {
        emit focusStateChanged(false);
    } else if(old!=NULL && now==NULL) {
        emit focusStateChanged(true);
        foreach (JoyPad *joypad, available) {
            debug_mesg("iterating and releasing\n");
            joypad->release();
        }
        debug_mesg("done releasing!\n");
    }
}
