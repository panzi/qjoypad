#ifndef JOY_LOOP_H
#define JOY_LOOP_H

#include <qeventloop.h>
#include <qvaluelist.h>

//#include <sys/ioctl.h>
//#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>

#include "joypad.h"
#include "device.h"


class JoyLoop : public QEventLoop {
	Q_OBJECT
	public:
		bool processEvents( ProcessEventsFlags );
};

#endif
