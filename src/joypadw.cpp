#include "joypadw.h"

JoyPadWidget::JoyPadWidget( JoyPad* jp, int i, QWidget* parent )
	: QWidget(parent) {
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
	joypad->releaseWidget();
}

void JoyPadWidget::flash( bool on ) {
	bool wasOn = (flashcount != 0);
	
	flashcount += (on?1:-1);
	
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
	quickset = new QuickSet(joypad);
	quickset->exec();
	update();
	delete quickset;
	quickset = NULL;
}

void JoyPadWidget::jsevent( js_event msg ) {
	if (msg.type == JS_EVENT_AXIS) {
		Axes[msg.number]->jsevent(msg.value);
	}
	else {
		Buttons[msg.number]->jsevent(msg.value);
	}
	if (quickset != NULL)
		quickset->jsevent(msg);
}
