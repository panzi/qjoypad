#include "button.h"


Button::Button( int i ) {
	index = i;
	isOn = false;
	//to keep toDefault from calling tossTimer without first calling takeTimer
	rapidfire = false;
	toDefault();
}

Button::~Button() {
	release();
}

bool Button::read( QTextStream* stream ) {
//	at this point, toDefault() has just been called.
	
	//read in a line of text and break it into words
	QString input = stream->readLine().lower();
	QRegExp regex("[\\s,]+");
	QStringList words = QStringList::split(regex,input);
	
	//used to assure correct conversion of QStrings -> ints
	bool ok;
	//used to receive converted ints from QStrings.
	int val;
	
	//go through every word on the line describing this button.
    for ( QStringList::Iterator it = words.begin(); it != words.end(); ++it ) {
        if (*it == "mouse") {
			++it;
			if (it == words.end()) return false;
			val = (*it).toInt(&ok);
			if (ok && val >= 0 && val <= MAXKEY) {
				useMouse = true;
				mousecode = val;
			}
			else return false;
		}
		else if (*it == "key") {
			++it;
			if (it == words.end()) return false;
			val = (*it).toInt(&ok);
			if (ok && val >= 0 && val <= MAXKEY) {
				useMouse = false;
				keycode = val;
			}
			else return false;
		}
		else if (*it == "rapidfire") {
			if (rapidfire == false) takeTimer( this );
			rapidfire = true;
		}
		else if (*it == "sticky") {
			sticky = true;
		}
    }
	return true;
}

void Button::write( QTextStream* stream ) {
	*stream << "\t" << getName() << ": ";
	if (rapidfire) *stream << "rapidfire, ";
	if (sticky) *stream << "sticky, ";
	*stream << (useMouse?"mouse ":"key ") << (useMouse?mousecode:keycode) << "\n";
}

void Button::release() {
	if (isDown) {
		click(false);
		isDown = true;
	}
}

void Button::jsevent( int value ) {
	bool newval = (value == 1);
	if (sticky) {
		//the state of a sticky key only changes on button press, not button release.
		if (newval) isOn = !isOn;
		else return;
	}
	//if the received event indicates a change in state,
	else if (newval != isOn) {
		isOn = newval; //change state
	}
	//otherwise... we don't care. This shouldn't happen.
	else return;
	//if rapidfire is on, then timer() will do its job. Otherwise we must
	//manually triger the key event.
	if (!rapidfire) click(isOn);
}

void Button::toDefault() {
	if (rapidfire == true) tossTimer( this );
	rapidfire = false;
	sticky = false;
	useMouse = false;
	keycode = 0;
	mousecode = 0;
}

bool Button::isDefault() {
	return	(rapidfire == false) &&
			(sticky == false) &&
			(useMouse == false) &&
			(keycode == 0) &&
			(mousecode == 0);
}

QString Button::status() {
	if (useMouse) {
		return getName() + " : Mouse " + QString::number(mousecode);
	}
	else {
		return getName() + " : " + QString(ktos(keycode));
	}
}

void Button::setKey( bool mouse, int value ) {
	if (mouse) {
		mousecode = value;
		useMouse = true;
	}
	else {
		keycode = value;
		useMouse = false;
	}
}

void Button::timer( int tick ) {
	if (isOn) {
		//originally I just clicked true and then false right after, but this
		//was not recognized by some programs. I need a delay in between.
		if (tick % FREQ == 0) {
			click(true);
		}
		if (tick % FREQ == FREQ / 2) {
			click(false);
		}
	}
}

void Button::click( bool press ) {
	if (isDown == press) return;
	isDown = press;
	xevent click;
	//determine which of the four possible events we're sending.
	if (press) click.type = useMouse?BPRESS:KPRESS;
		else click.type = useMouse?BREL:KREL;
	//set up the event,
	click.value1 = useMouse?mousecode:keycode;
	click.value2 = 0;
	//and send it.
	sendevent( click );
}
