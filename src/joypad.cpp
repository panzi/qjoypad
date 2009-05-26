
#include "joypad.h"
JoyPad::JoyPad( int i, int dev ) {
	//remember the index,
	index = i;
	
	//load data from the joystick device, if available.
	resetToDev(dev);
	
	//there is no JoyPadWidget yet.
	jpw = NULL;
}

void JoyPad::resetToDev(int dev ) {
	//remember the device file descriptor
	joydev = dev;

	//read in the number of axes / buttons
	axes = 0;
	ioctl (joydev, JSIOCGAXES, &axes);
	buttons = 0;
	ioctl (joydev, JSIOCGBUTTONS, &buttons);
	
	//make sure that we have the axes we need.
	//if one that we need doesn't yet exist, add it in.
	//Note: if the current layout has a key assigned to an axis that did not
	//have a real joystick axis mapped to it, and this function suddenly brings
	//that axis into use, the key assignment will not be lost because the axis
	//will already exist and no new axis will be created.
	for (int i = 0; i < axes; i++) {
		if (Axes[i] == 0) Axes.insert(i, new Axis( i ));
	}
	for (int i = 0; i < buttons; i++) {
		if (Buttons[i] == 0) Buttons.insert(i, new Button( i ));
	}
}

void JoyPad::toDefault() {
	//to reset the whole, reset all the parts.
    for ( QIntDictIterator<Axis> it( Axes ); it.current(); ++it )
		it.current()->toDefault();
    for ( QIntDictIterator<Button> it( Buttons ); it.current(); ++it )
		it.current()->toDefault();
}

bool JoyPad::isDefault() {
	//if any of the parts are not at default, then the whole isn't either.
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
		//only write an axis if it is not at the default settings  :)
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

void JoyPad::release() {
    for ( QIntDictIterator<Axis> it( Axes ); it.current(); ++it ) {
		it.current()->release();
	}
    for ( QIntDictIterator<Button> it( Buttons ); it.current(); ++it ) {
		it.current()->release();
	}
}

void JoyPad::jsevent( js_event msg ) {
	//if there is a JoyPadWidget around, ie, if the joypad is being edited
	if (jpw != NULL) {
		//tell the dialog there was an event. It will use this to flash
		//the appropriate button, if necesary.
		jpw->jsevent(msg);
	}
	//if the dialog is open, stop here. We don't want to signal ourselves with
	//the input we generate.
	if (qApp->activeWindow() != 0 && qApp->activeModalWidget() != 0) return;

	//otherwise, lets create us a fake event! Pass on the event to whichever
	//Button or Axis was pressed and let them decide what to do with it.
	if (msg.type & JS_EVENT_AXIS) {
		Axes[msg.number]->jsevent(msg.value);
	}
	else {
		Buttons[msg.number]->jsevent(msg.value);
	}
}

JoyPadWidget* JoyPad::widget( QWidget* parent, int i) {
	//create the widget and remember it.
	jpw = new JoyPadWidget(this, i, parent);
	return jpw;
}

void JoyPad::releaseWidget() {
	//this is how we know that there is no longer a JoyPadWidget around.
	jpw = NULL;
}
