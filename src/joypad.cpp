
#include "joypad.h"
JoyPad::JoyPad( int i, int dev ) {
	index = i;
	
	resetToDev(dev);
	
	jpw = NULL;
}

void JoyPad::resetToDev(int dev ) {
	joydev = dev;

	axes = 0;
	ioctl (joydev, JSIOCGAXES, &axes);
	buttons = 0;
	ioctl (joydev, JSIOCGBUTTONS, &buttons);
	
	for (int i = 0; i < axes; i++) {
		if (Axes[i] == 0) Axes.insert(i, new Axis(  i ));
	}
	for (int i = 0; i < buttons; i++) {
		if (Buttons[i] == 0) Buttons.insert(i, new Button( i ));
	}
	
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

//only actually writes something if this JoyPad is NON DEFAULT.
void JoyPad::write( QTextStream* stream ) {
	int i = 0; //use to test if this is default or not.
	QString result;
	QTextStream* s = new QTextStream(&result, IO_WriteOnly);
	*s << getName() << " {\n";
    for ( QIntDictIterator<Axis> it( Axes ); it.current(); ++it ) {
		if (!it.current()->isDefault()) {
			it.current()->write( s );
			++i;
		}
	}
    for ( QIntDictIterator<Button> it( Buttons ); it.current(); ++it ) {
		if (!it.current()->isDefault()) {
			it.current()->write( s );
			++i;
		}
	}
	
	if (i > 0) {
		*stream << result << "}\n\n";
	}
}

void JoyPad::jsevent( js_event msg ) {
	if (jpw != NULL) {
		jpw->jsevent(msg);
	}
	if (qApp->activeWindow() != 0 && qApp->activeModalWidget() != 0) return;

	if (msg.type & JS_EVENT_AXIS) {
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
