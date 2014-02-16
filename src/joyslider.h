#ifndef QJOYPAD_JOYSLIDER_H
#define QJOYPAD_JOYSLIDER_H

//the parent of this

#include <stdlib.h>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QSpinBox>
#include "constant.h"

class JoySlider : public QWidget
{
    Q_OBJECT

    enum DragState {
        DragNone = 0,

        //When dragState == DragXZ, that means we are currently dragging the xZone mark
        DragXZ   = 1,

        //When dragState == DragDZ, that means we are currently dragging the deadZone mark
        DragDZ   = 2
    };

	public:
		JoySlider( int dz, int xz, int val, QWidget* parent );
		//set where the axis physically is
		void setValue( int );
		//change the throttle mode
		void setThrottle( int );
		//get the current settings
        int deadZone() { return deadzone; }
        int xZone() { return xzone; }
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
        DragState dragState;
		
		//the axis' position
        int joyval;
		//the dead and extreme zone values
        int deadzone;
        int xzone;
};


#endif
