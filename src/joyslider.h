#ifndef Q_JOYSLIDER_H
#define Q_JOYSLIDER_H

#include <qwidget.h>
#include <qpainter.h>
#include <stdlib.h>

#include "constant.h"

#define DRAG_XZ 1
#define DRAG_DZ 2

class JoySlider : public QWidget
{
	public:
		JoySlider( int dz, int xz, int val, QWidget* parent );
		void setValue( int );
		void setThrottle( int );
		int dZone() { return DeadZone; };
		int xZone() { return XZone; };
	protected:
		void drawBox( int x, int width );
		void paintEvent( QPaintEvent* );
		void resizeEvent( QResizeEvent* );
		void mousePressEvent( QMouseEvent* e );
		void mouseReleaseEvent( QMouseEvent* );
		void mouseMoveEvent( QMouseEvent* e );
	private:
		int boxwidth;
		int boxheight;
		int rboxstart;
		int rboxend;
		int lboxstart;
		int lboxend;
		int twidth;
		int tend;
		
		int throttle;
		
		int pointFor( int value, bool negative );
		int valueFrom( int point );
		
		int dragging;
		
		int JoyVal;
		int DeadZone;
		int XZone;
};


#endif
