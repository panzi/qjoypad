#include "button.h"

Button::Button( int i ) {
	index = i;
	isOn = false;
	tick = 0;
	toDefault();
}

void Button::toDefault() {
	killTimers();
	rapidfire = false;
	sticky = false;
	useMouse = false;
	keycode = 0;
	mousecode = 0;
}

bool Button::isDefault() {
	return (rapidfire == false) && (sticky == false) && (useMouse == false) &&
		(keycode == 0) && (mousecode == 0);
}

bool Button::read( QTextStream* stream ) {
//	toDefault(); Now done by JoyPad

	QString input = stream->readLine().lower();
	QRegExp regex("[\\s,]+");
	QStringList words = QStringList::split(regex,input);
	bool ok;
	int val;
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
			rapidfire = true;
			startTimer(MSEC);
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

void Button::click( bool press ) {
	xevent click;
	if (press) click.type = useMouse?BPRESS:KPRESS;
		else click.type = useMouse?BREL:KREL;
	click.value1 = useMouse?mousecode:keycode;
	click.value2 = 0;
	sendevent( click );
}

void Button::release() {
	click(false);
}

void Button::timerEvent( QTimerEvent* ) {
	if (isOn) {
		if (tick % FREQ == 0) {
			click(true);
			click(false);
		}
		++tick;
	}
}

void Button::jsevent( int value ) {
	bool newval = (value == 1);
	if (sticky) {
		if (newval) isOn = !isOn;
		else return;
	}
	else if (newval != isOn) {
		isOn = newval;
	}
	else return;
	if (!rapidfire) click(isOn);
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
