#ifndef Q_JOYSLIDER_H
#define Q_JOYSLIDER_H

//the parent of this
#include <qwidget.h>
//for drawing the widget  :)
#include <qpainter.h>
//for abs()
#include <stdlib.h>

#include "constant.h"

//dragging constants.
//When dragging == DRAG_XZ, that means we are currently dragging the xZone mark
#define DRAG_XZ 1
//When dragging == DRAG_DZ, that means we are currently dragging the dZone mark
#define DRAG_DZ 2

class JoySlider : public QWidget
{
	public:
		JoySlider( int dz, int xz, int val, QWidget* parent );
		//set where the axis physically is
		void setValue( int );
		//change the throttle mode
		void setThrottle( int );
		//get the current settings
		int dZone() { return DeadZone; };
		int xZone() { return XZone; };
	protected:
		//all for getting the widget to look right:
		void drawBox( int x, int width );
		void paintEvent( QPaintEvent* );
		void resizeEvent( QResizeEvent* );
		//for working with drag and drop:
		void mousePressEvent( QMouseEvent* e );
		void mouseReleaseEvent( QMouseEvent* );
		void mouseMoveEvent( QMouseEvent* e );
	private:
		//dimensions of the box to draw!
		int boxwidth;
		int boxheight;
		int rboxstart;
		int rboxend;
		int lboxstart;
		int lboxend;
		int twidth;
		int tend;
		
		//the throttle mode
		int throttle;
		
		//convert from joystick value to pixel position on the widget
		//negative refers to whether or not we're on the "negative" end
		//of the widget
		int pointFor( int value, bool negative );
		//convert from pixel position to joystick value
		int valueFrom( int point );
		
		//the current drag and drop state
		int dragging;
		
		//the axis' position
		int JoyVal;
		//the dead and extreme zone values
		int DeadZone;
		int XZone;
};


#endif
