
#include "joypad.h"
JoyPad::JoyPad( int i, int dev ) {
	joydev = dev;
	index = i;

	axes = 0;
	ioctl (joydev, JSIOCGAXES, &axes);
	buttons = 0;
	ioctl (joydev, JSIOCGBUTTONS, &buttons);
	
	for (int i = 0; i < axes; i++) {
		Axes.insert(i, new Axis(  i ));
	}
	for (int i = 0; i < buttons; i++) {
		Buttons.insert(i, new Button( i ));
	}
	
	jpw = NULL;
}


void JoyPad::toDefault() {
    for ( QIntDictIterator<Axis> it( Axes ); it.current(); ++it )
		it.current()->toDefault();
    for ( QIntDictIterator<Button> it( Buttons ); it.current(); ++it )
		it.current()->toDefault();
}

bool JoyPad::isDefault() {
    for ( QIntDictIterator<Axis> it( Axes ); it.current(); ++it )
		if (!it.current()->isDefault()) return false;
    for ( QIntDictIterator<Button> it( Buttons ); it.current(); ++it )
		if (!it.current()->isDefault()) return false;
	return true;
}

bool JoyPad::read( QTextStream* stream ) {
	toDefault();

	QString word;
	QChar dump;
	int num;
	
	*stream >> word;
	while (word != NULL && word != "}") {
		word = word.lower();
		if (word == "button") {
		 	*stream >> num;
			if (num > 0) {
				*stream >> dump;
				if (dump != ':') {
					error("Layout file error", "Expected ':', found '" + QString(dump) + "'.");
					return false;
				}
				if (Buttons[num-1] == 0) {
					Buttons.insert(num-1,new Button(num-1));
				}
				if (!Buttons[num-1]->read( stream )) {
					error("Layout file error", "Error reading Button " + QString::number(num));
					return false;
				}
			}
			else {
				stream->readLine();
			}
		 }
		 else if (word == "axis") {
		 	*stream >> num;
			if (num > 0) {
				*stream >> dump;
				if (dump != ':') {
					error("Layout file error", "Expected ':', found '" + QString(dump) + "'.");
					return false;
				}
				if (Axes[num-1] == 0) {
					Axes.insert(num-1,new Axis(num-1));
				}
				if (!Axes[num-1]->read(stream)) {
					error("Layout file error", "Error reading Axis " + QString::number(num));
					return false;
				}
			}
		 }
		 else {
		 	error( "Layout file error", "Error while reading layout. Unrecognized word: " + word );
			return false;
		 }
		*stream >> word;
	}
	return true;
}

void JoyPad::write( QTextStream* stream ) {
	*stream << getName() << " {\n";
    for ( QIntDictIterator<Axis> it( Axes ); it.current(); ++it )
		if (!it.current()->isDefault()) it.current()->write( stream );
    for ( QIntDictIterator<Button> it( Buttons ); it.current(); ++it )
		if (!it.current()->isDefault()) it.current()->write( stream );
	*stream << "}\n\n";
}

void JoyPad::jsevent( js_event msg ) {
	if (jpw != NULL) {
		jpw->jsevent(msg);
		return;
	}

	if (qApp->activeWindow() != 0 && qApp->activeModalWidget != 0) return;

	if (msg.type == JS_EVENT_AXIS) {
		Axes[msg.number]->jsevent(msg.value);
	}
	else {
		Buttons[msg.number]->jsevent(msg.value);
	}
}

QString JoyPad::getName() {
	return "Joystick " + QString::number( index + 1 );
}

JoyPadWidget* JoyPad::widget( QWidget* parent, int i) {
	jpw = new JoyPadWidget(this, i, parent);
	return jpw;
}

void JoyPad::releaseWidget() {
	jpw = NULL;
}
