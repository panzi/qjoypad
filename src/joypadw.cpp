#include "joypadw.h"

JoyPadWidget::JoyPadWidget( JoyPad* jp, int i, QWidget* parent )
	: QWidget(parent) {
	//initialize things, build the dialog
	joypad = jp;
	index = i;
	LMain = new QGridLayout(this, (joypad->axes+1)/2 +(joypad->buttons+1)/2 + 2, 2, 5, 5);
	LMain->setAutoAdd( true );
	flashcount = 0;
	quickset = NULL;
	
	Axes = new AxisWidget*[joypad->axes];
	for (int i = 0; i < joypad->axes; i++) {
		Axes[i] = new AxisWidget(joypad->Axes[i],this);
		connect( Axes[i], SIGNAL( flashed( bool ) ), this, SLOT( flash( bool )));
	}
	if (joypad->axes % 2 == 1) {
		new QWidget(this);
	}
	
	Buttons = new ButtonWidget*[joypad->buttons];
	for (int i = 0; i < joypad->buttons; i++) {
		Buttons[i] = new ButtonWidget(joypad->Buttons[i],this);
		connect( Buttons[i], SIGNAL( flashed( bool ) ), this, SLOT( flash( bool )));
	}
	if (joypad->buttons % 2 == 1) {
		new QWidget(this);
	}
	
	new QWidget(this);
	new QWidget(this);

	BClear = new QPushButton("Clear", this);
	connect(BClear, SIGNAL(clicked()), this, SLOT(clear()));
	BAll = new QPushButton("Quick Set", this);
	connect(BAll, SIGNAL(clicked()), this, SLOT(setAll()));
}

JoyPadWidget::~JoyPadWidget() {
	//so the joypad knows that we're done.
	joypad->releaseWidget();
}

void JoyPadWidget::flash( bool on ) {
	//true iff this entire widget was considered "flashed" before
	bool wasOn = (flashcount != 0);
	
	//adjust the count based on this new flash
	flashcount += (on?1:-1);
	
	//if we were on and should now be off, or visa versa, flash the whole widget
	if (wasOn != (flashcount != 0))
		emit flashed(index);
}

void JoyPadWidget::update() {
	for (int i = 0; i < joypad->axes; i++) {
		Axes[i]->update();
	}
	for (int i = 0; i < joypad->buttons; i++) {
		Buttons[i]->update();
	}
}

void JoyPadWidget::clear() {
	joypad->toDefault();
	update();
}

void JoyPadWidget::setAll() {
	//quickset is NULL if there is no quickset dialog, and a pointer to the
	//dialog otherwise. This is so we can forward jsevents properly.
	quickset = new QuickSet(joypad);
	quickset->exec();
	update();
	delete quickset;
	quickset = NULL;
}

void JoyPadWidget::jsevent( js_event msg ) {
	//notify the component this event applies to. this cannot generate anything
	//other than a flash  :)
	if (msg.type == JS_EVENT_AXIS) {
		Axes[msg.number]->jsevent(msg.value);
	}
	else {
		Buttons[msg.number]->jsevent(msg.value);
	}
	//if we're doing quickset, it needs to know when we do something.
	if (quickset != NULL)
		quickset->jsevent(msg);
}
