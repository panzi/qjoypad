#include "layout_edit.h"


//build the dialog
LayoutEdit::LayoutEdit( LayoutManager* l ) {
	lm = l;

	setCaption( NAME );
	setIcon(QPixmap(ICON24));

	LMain = new QVBoxLayout( this,5,5 );

	QFrame* frame = new QFrame(this);
	frame->setFrameStyle(QFrame::Box | QFrame::Sunken );
	QGridLayout* g = new QGridLayout(frame,2,1,5,5);
		CLayouts = new QComboBox(frame);
		connect( CLayouts, SIGNAL(activated( const QString& )), lm, SLOT(load(const QString&)));
		g->addMultiCellWidget(CLayouts,0,0,0,3);

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
	
	//we have a WidgetStack to represent the multiple joypads
	PadStack = new QWidgetStack( this );
	PadStack->setFrameStyle(QFrame::Box | QFrame::Sunken );
	LMain->addWidget(PadStack);
	
	//produce a list of names for the FlashRadioArray
	//this is only necesary since joystick devices need not always be
	//contiguous
	int padcount = available.count();
	QString names[padcount];
	int i = 0;
    QIntDictIterator<JoyPad> it( available );
    for ( ; it.current(); ++it ) {
		names[i] = it.current()->getName();
		++i;
	}

	//flash radio array
	JoyButtons = new FlashRadioArray(padcount, names, true, this );
	LMain->insertWidget( 1, JoyButtons );

	//go through each of the available joysticks
	i = 0; // i is the current index into PadStack
    for (it.toFirst() ; it.current(); ++it ) {
		//add a new JoyPadWidget to the stack
		PadStack->addWidget( it.current()->widget(PadStack,i),i );
		//every time it "flashes", flash the associated tab.
		connect( PadStack->widget(i), SIGNAL( flashed( int ) ), JoyButtons, SLOT( flash( int )));
		++i;
	}
	//whenever a new tab is selected, raise the appropriate JoyPadWidget
	connect( JoyButtons, SIGNAL( changed( int ) ), PadStack, SLOT( raiseWidget( int )));
	
	updateLayoutList();

	//add the buttons at the bottom.
	QHBoxLayout* h = new QHBoxLayout(0,0,5);		
		QPushButton* close = new QPushButton( "-- Close Dialog --", this );
		connect(close, SIGNAL(clicked()), this, SLOT(close()));
		h->addWidget(close);
		QPushButton* quit = new QPushButton( "-- Quit --", this );
		connect( quit, SIGNAL( clicked() ), qApp, SLOT(quit()));
		h->addWidget(quit);
	LMain->addLayout(h);
}

void LayoutEdit::setLayout(QString layout) {
	//change the text,
	CLayouts->setCurrentText(layout);
	//update all the JoyPadWidgets.
	for (uint i = 0; i < available.count(); i++) {
		((JoyPadWidget*)PadStack->widget(i))->update();
	}
}

void LayoutEdit::updateLayoutList() {
	//blank the list, then load in new names from the LayoutManager.
	CLayouts->clear();
	QStringList layouts = lm->getLayoutNames();
	//For some reason, insertStringList doesn't want to work for me!
	for ( QStringList::Iterator it = layouts.begin(); it != layouts.end(); ++it ) {
		CLayouts->insertItem(*it);
	}
	CLayouts->setCurrentText(lm->CurrentLayout);
}

void LayoutEdit::windowActivationChange( bool oldActive ) {
	if (oldActive) return;
	
	//whenever the window becomes active, release all pressed buttons! This way
	//you don't get any presses without releases to confuse things.
	QIntDictIterator<JoyPad> it( available );
	for ( ; it.current(); ++it ) {
		it.current()->release();
	}
}
