#include "axis.h"

#define sqr(a) ((a)*(a))

Axis::Axis( int i ) {
	index = i;
	isOn = false;
	tick = 0;
	isDown = false;
	toDefault();
}

Axis::~Axis() {
	release();
}

void Axis::toDefault() {
	killTimers();
	release();
	
	gradient = false;
	throttle = 0;
	maxSpeed = 100;
	dZone = 3000;
	xZone = 30000;
	mode = keybd;
	pkeycode = 0;
	nkeycode = 0;
	downkey = 0;
	state = 0;
	isDown = false;
}

bool Axis::isDefault() {
	return (gradient == false) && (throttle == 0) && (maxSpeed == 100) &&
		(dZone == 3000) && (xZone == 30000) && (mode == keybd) &&
		(pkeycode == 0) && (nkeycode == 0) && (downkey == 0) && (state == 0) &&
		(isDown == false);
}

bool Axis::read( QTextStream* stream ) {
//	toDefault(); Now done by JoyPad
	
	QString input = stream->readLine().lower();
	QRegExp regex("[\\s,]+");
	QStringList words = QStringList::split(regex,input);
	bool ok;
	int val;
	mode = keybd;
    for ( QStringList::Iterator it = words.begin(); it != words.end(); ++it ) {
        if (*it == "maxspeed") {
			++it;
			if (it == words.end()) return false;
			val = (*it).toInt(&ok);
			if (ok && val >= 0 && val <= 100) maxSpeed = val;
			else return false;
		}
		else if (*it == "dzone") {
			++it;
			if (it == words.end()) return false;
			val = (*it).toInt(&ok);
			if (ok && val >= 0 && val <= JOYMAX) dZone = val;
			else return false;
		}
		else if (*it == "xzone") {
			++it;
			if (it == words.end()) return false;
			val = (*it).toInt(&ok);
			if (ok && val >= 0 && val <= JOYMAX) xZone = val;
			else return false;
		}
		else if (*it == "+key") {
			++it;
			if (it == words.end()) return false;
			val = (*it).toInt(&ok);
			if (ok && val >= 0 && val <= MAXKEY) pkeycode = val;
			else return false;
		}
		else if (*it == "-key") {
			++it;
			if (it == words.end()) return false;
			val = (*it).toInt(&ok);
			if (ok && val >= 0 && val <= MAXKEY) nkeycode = val;
			else return false;
		}
		else if (*it == "gradient") {
			gradient = true;
			startTimer(MSEC);
		}
		else if (*it == "throttle+") {
			throttle = 1;
		}
		else if (*it == "throttle-") {
			throttle = -1;
		}
		else if (*it == "mouse+v") {
			mode = mousepv;
		}
		else if (*it == "mouse-v") {
			mode = mousenv;
		}
		else if (*it == "mouse+h") {
			mode = mouseph;
		}
		else if (*it == "mouse-h") {
			mode = mousenh;
		}
    }
	return true;
}

void Axis::write( QTextStream* stream ) {
	*stream << "\t" << getName() << ": ";
	if (gradient)  *stream << "gradient, ";
	if (throttle > 0) *stream << "throttle+, ";
	else if (throttle < 0) *stream << "throttle-, ";
	*stream << "dZone " << dZone << ", "
	        << "xZone " << xZone << ", ";
	if (mode == keybd) {
		*stream << "+key " << pkeycode << ", "
		        << "-key " << nkeycode << "\n";
	}
	else {
		if (gradient) *stream << "maxSpeed " << maxSpeed << ", ";
		*stream << "mouse";
		if (mode == mousepv)
			*stream << "+v\n";
		else if (mode == mousenv)
			*stream << "-v\n";
		else if (mode == mouseph)
			*stream << "+h\n";
		else if (mode == mousenh)
			*stream << "-h\n";
	}
	
}

void Axis::move( bool press ) {
	xevent e;
	isDown = press;
	if (mode == keybd) {
		if (press) {
			e.type = KPRESS;
			downkey = (state > 0)?pkeycode:nkeycode;
		}
		else {
			e.type = KREL;
		}
		e.value1 = downkey;
		e.value2 = 0;
	}
	else if (press){
		int dist;
		if (gradient) {
			float a = (float) (maxSpeed-0) / sqr(xZone - dZone);
			float b = -2 * a * dZone;
			float c = a * sqr(dZone);
			dist = (int) (a*sqr(state) + b*state + c);
			dist = (dist>0)?dist:0;
			dist = (dist<maxSpeed)?dist:maxSpeed;
		}
		else dist = maxSpeed;
		if (state < 0) dist = -dist;
		e.type = WARP;
		if (mode == mousepv) {
			e.value1 = 0;
			e.value2 = dist;
		}
		else if (mode == mousenv) {
			e.value1 = 0;
			e.value2 = -dist;
		}
		else if (mode == mouseph) {
			e.value1 = dist;
			e.value2 = 0;
		}
		else if (mode == mousenh) {
			e.value1 = -dist;
			e.value2 = 0;
		}
	}
	sendevent(e);
}


void Axis::release() {
	if (isDown) {
		move(false);
	}
}

void Axis::timerEvent( QTimerEvent* ) {
	if (isOn) {
		if (mode == keybd) {
			if (tick % FREQ == 0) {
				if (duration == FREQ) {
					if (!isDown) move(true);
					duration = (abs(state) * FREQ) / JOYMAX;
					return;
				}
				move(true);
			}
			if (tick % FREQ == duration) {
				move(false);
				duration = (abs(state) * FREQ) / JOYMAX;
			}
		}
		else {
			move(true);
		}
		++tick;
	}
}

void Axis::jsevent( int value ) {
	if (throttle == 0)
		state = value;
	else if (throttle == -1)
		state = (value + JOYMIN) / 2;
	else
		state = (value + JOYMAX) / 2;
	if (isOn && abs(state) <= dZone) {
		isOn = false;
		if (gradient) release();
	}
	else if (!isOn && abs(state) >= dZone) {
		isOn = true;
		if (gradient) duration = (abs(state) * FREQ) / JOYMAX;
	}
	else return;
	if (!gradient) {
		move(isOn);
	}
}

bool Axis::inDeadZone( int val ) {
	int value;
	if (throttle == 0)
		value = val;
	else if (throttle == -1)
		value = (val + JOYMIN) / 2;
	else
		value = (val + JOYMAX) / 2;
	return (abs(value) < dZone);
}

QString Axis::status() {
	QString result = getName() + " : [";
	if (mode == keybd) {
		if (throttle == 0)
			result += "KEYBOARD";
		else result += "THROTTLE";
	}
	else result += "MOUSE";
	return result + "]";
}

void Axis::setKey(bool positive, int value) {
	if (positive)
		pkeycode = value;
	else
		nkeycode = value;
}
