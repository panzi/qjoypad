#include "layout_edit.h"

LayoutEdit::LayoutEdit( LayoutManager* l ) {
	lm = l;
	
	LMain = new QVBoxLayout( this,5,5 );
	QFrame* frame = new QFrame(this);
	frame->setFrameStyle(QFrame::Box | QFrame::Sunken );
	QGridLayout* g = new QGridLayout(frame,2,1,5,5);
		CLayouts = new QComboBox(frame);
		connect( CLayouts, SIGNAL(activated( const QString& )), lm, SLOT(load(const QString&)));
		g->addMultiCellWidget(CLayouts,0,0,0,3);
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
	
	PadStack = new QWidgetStack( this );
	PadStack->setFrameStyle(QFrame::Box | QFrame::Sunken );
	LMain->addWidget(PadStack);
	
	
	int padcount = available.count();
	QString names[padcount];
	int i = 0;
    QIntDictIterator<JoyPad> it( available );
    for ( ; it.current(); ++it ) {
		names[i] = it.current()->getName();
		++i;
	}

	JoyButtons = new FlashRadioArray(padcount, names, true, this );
	LMain->insertWidget( 1, JoyButtons );

	i = 0;
    for (it.toFirst() ; it.current(); ++it ) {
		PadStack->addWidget( it.current()->widget(PadStack,i),i );
		connect( PadStack->widget(i), SIGNAL( flashed( int ) ), JoyButtons, SLOT( flash( int )));
		++i;
	}
	connect( JoyButtons, SIGNAL( changed( int ) ), PadStack, SLOT( raiseWidget( int )));
	
	updateLayoutList();
	
	QHBoxLayout* h = new QHBoxLayout(this,0,5);		
		QPushButton* close = new QPushButton( "-- Close Dialog --", this );
		connect(close, SIGNAL(clicked()), this, SLOT(close()));
		h->addWidget(close);
		QPushButton* quit = new QPushButton( "-- Quit --", this );
		connect( quit, SIGNAL( clicked() ), qApp, SLOT(quit()));
		h->addWidget(quit);
	LMain->addLayout(h);
}

void LayoutEdit::setLayout(QString layout) {
	CLayouts->setCurrentText(layout);
	for (uint i = 0; i < available.count(); i++) {
		((JoyPadWidget*)PadStack->widget(i))->update();
	}
}

void LayoutEdit::updateLayoutList() {
	//insertStrList just doesn't seem to work for some reason!!!
	CLayouts->clear();
	QStringList layouts = lm->getLayoutNames();
	for ( QStringList::Iterator it = layouts.begin(); it != layouts.end(); ++it ) {
		CLayouts->insertItem(*it);
	}
	CLayouts->setCurrentText(lm->CurrentLayout);
}
