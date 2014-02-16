#include "layout_edit.h"

//build the dialog
LayoutEdit::LayoutEdit( LayoutManager* l ): QWidget(NULL) {
    lm = l;
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle( NAME );
    setWindowIcon(QPixmap(ICON24));

    mainLayout = new QVBoxLayout( this);
    mainLayout->setSpacing(5);
    mainLayout->setMargin(5);

    QFrame* frame = new QFrame(this);
    frame->setFrameStyle(QFrame::Box | QFrame::Sunken );
    QGridLayout* g = new QGridLayout(frame);
    g->setMargin(5);
    g->setSpacing(5);
    cmbLayouts = new QComboBox(frame);
    connect( cmbLayouts, SIGNAL(activated( const QString& )), lm, SLOT(load(const QString&)));
    g->addWidget(cmbLayouts,0,0,1,4);

    //most of these buttons can link directly into slots in the LayoutManager
    btnAdd = new QPushButton("Add", frame);
    connect(btnAdd, SIGNAL(clicked()), lm, SLOT(saveAs()));
    g->addWidget(btnAdd,1,0);
    btnRem = new QPushButton("Remove", frame);
    connect(btnRem, SIGNAL(clicked()), lm, SLOT(remove()));
    g->addWidget(btnRem,1,1);
    btnUpd = new QPushButton("Update", frame);
    connect(btnUpd, SIGNAL(clicked()), lm, SLOT(save()));
    g->addWidget(btnUpd,1,2);
    btnRev = new QPushButton("Revert", frame);
    connect(btnRev, SIGNAL(clicked()), lm, SLOT(reload()));
    g->addWidget(btnRev,1,3);
    mainLayout->addWidget( frame );

    //produce a list of names for the FlashRadioArray
    //this is only necesary since joystick devices need not always be
    //contiguous
    QStringList names;
    foreach (JoyPad *joypad, lm->available) {
        names.append(joypad->getName());
        connect(this, SIGNAL(focusStateChanged(bool)), joypad, SLOT(focusChange(bool)));
    }
    
    //flash radio array
    joyButtons = new FlashRadioArray(names, true, this );
    mainLayout->addWidget( joyButtons );
    
    //we have a WidgetStack to represent the multiple joypads
    padStack = new QStackedWidget( this );
    padStack->setFrameStyle(QFrame::Box | QFrame::Sunken );
    mainLayout->addWidget(padStack);

    //go through each of the available joysticks
    // i is the current index into PadStack
    int i = 0;
    foreach (JoyPad *joypad, lm->available) {
        //add a new JoyPadWidget to the stack
        padStack->insertWidget( i, joypad->widget(padStack,i) );
        //every time it "flashes", flash the associated tab.
        connect( padStack->widget(i), SIGNAL( flashed( int ) ), joyButtons, SLOT( flash( int )));
        ++i;
    }
    //whenever a new tab is selected, raise the appropriate JoyPadWidget
    connect( joyButtons, SIGNAL( changed( int ) ), padStack, SLOT( setCurrentIndex( int )));

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
    mainLayout->addLayout(h);
    connect(qApp, SIGNAL(focusChanged ( QWidget * , QWidget *  ) ), this, 
        SLOT(appFocusChanged(QWidget *, QWidget *)));
    this->show();
}

void LayoutEdit::setLayout(QString layout) {
    //change the text,
    cmbLayouts->setCurrentIndex(lm->getLayoutNames().indexOf(layout));
    //update all the JoyPadWidgets.
    for (int i = 0, n = lm->available.count(); i < n; i++) {
        ((JoyPadWidget*)padStack->widget(i))->update();
    }
}

void LayoutEdit::updateLayoutList() {
    //blank the list, then load in new names from the LayoutManager.
    cmbLayouts->clear();
    QStringList layouts = lm->getLayoutNames();
    cmbLayouts->insertItems(-1, layouts);
    cmbLayouts->setCurrentIndex(layouts.indexOf(lm->currentLayout));
}

void LayoutEdit::updateJoypadWidgets() {
    int indexOfFlashRadio = mainLayout->indexOf(joyButtons);
    FlashRadioArray *newJoyButtons;
    QStringList names;
    foreach (JoyPad *joypad, lm->available) {
        names.append(joypad->getName());
    }
    
    newJoyButtons = new FlashRadioArray( names, true, this );
    mainLayout->insertWidget(indexOfFlashRadio, newJoyButtons);
    mainLayout->removeWidget(joyButtons);
    FlashRadioArray* oldJoyButtons = joyButtons;
    joyButtons = newJoyButtons;
    connect( joyButtons, SIGNAL( changed( int ) ), padStack, SLOT( setCurrentIndex( int )));
    oldJoyButtons->deleteLater();
    int numberOfJoypads = padStack->count();
    for(int i = 0; i<numberOfJoypads; i++) {
        padStack->removeWidget(padStack->widget(0));
    }
    int i = 0;
    foreach (JoyPad *joypad, lm->available) {
        //add a new JoyPadWidget to the stack
        padStack->insertWidget( i, joypad->widget(padStack,i) );
        //every time it "flashes", flash the associated tab.
        connect( padStack->widget(i), SIGNAL( flashed( int ) ), joyButtons, SLOT( flash( int )));
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
        foreach (JoyPad *joypad, lm->available) {
            debug_mesg("iterating and releasing\n");
            joypad->release();
        }
        debug_mesg("done releasing!\n");
    }
}
