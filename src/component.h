#ifndef COMPONENT_H
#define COMPONENT_H


//for parsing data from a file
#include <qstringlist.h>
#include <qtextstream.h>
#include <qregexp.h>

//parent of Component
#include <qobject.h>

#include "event.h"
#include "constant.h"


//just a general blanket class to cover Button and Axis
//it is completely worthless in itself.
class Component : public QObject {
	public:
		virtual bool read( QTextStream* ) {return true; };
		virtual void write( QTextStream* ) {};
		virtual void release() {};
		virtual void jsevent( int ) {};
		virtual void toDefault() {};
		virtual bool isDefault() { return true; };
		virtual QString getName() { return ""; };
		virtual QString status() { return ""; };
		virtual void timer( int ) {};
	protected:
		bool isOn;
		bool isDown;
		int index;
};

#endif
