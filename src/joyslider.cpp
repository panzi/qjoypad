#include "joyslider.h"

JoySlider::JoySlider( int dz, int xz, int val, QWidget* parent )
	:QWidget( parent )
{
	JoyVal = val;
	DeadZone = dz;
	XZone = xz;
	setMinimumHeight(20);
}

void JoySlider::setValue( int newval )
{
	if (throttle == 0) JoyVal = newval;
	else if (throttle < 0) JoyVal = (newval + JOYMIN) / 2;
	else JoyVal = (newval + JOYMAX) / 2;
	update();
}

void JoySlider::setThrottle( int newval )
{
	throttle = newval;
	update();
}

int JoySlider::pointFor( int value, bool negative )
{
	if (throttle == 0) {
		int result = ((boxwidth - 4) * value) / JOYMAX;
		if (negative) result = lboxstart + boxwidth - 2 - result;
		else result += rboxstart + 2;
		return result;
	}
	else {
		int result = ((twidth - 4) * value) / JOYMAX;
		if (negative) result = lboxstart + twidth - 2 - result;
		else result += lboxstart + 2;
		return result;
	}
}

int JoySlider::valueFrom( int point )
{
	if (throttle == 0) {
		if (point <= lboxstart) return JOYMAX;
		if (point >= lboxend - 2 && point <= rboxstart + 2) return 0;
		if (point >= rboxend - 2) return JOYMAX;
		if (point < lboxend - 2) return ((lboxend - point) * JOYMAX) / boxwidth;
		if (point > rboxstart) return ((point - rboxstart) * JOYMAX) / boxwidth;
		else return 0;
	}
	else if (throttle > 0) {
		if (point <= lboxstart) return 0;
		else if (point >= tend) return JOYMAX;
		else return ((point - lboxstart) * JOYMAX) / twidth;
	}
	else {
		if (point <= lboxstart - 2) return JOYMAX;
		else if (point >= tend) return 0;
		else return ((tend - point) * JOYMAX) / twidth;
	}
}

void JoySlider::resizeEvent( QResizeEvent* )
{
	boxwidth = (this->width() - 6) / 2 - 1;
	twidth = this->width() - 4;
	boxheight = this->height() - 4;
	lboxstart = 1;
	lboxend = lboxstart + boxwidth - 1;
	tend = lboxstart + twidth - 1;
	rboxstart = lboxend + 5;
	rboxend = rboxstart + boxwidth - 1;
}

void JoySlider::drawBox( int x, int width ) {
	QPainter paint( this );

	paint.setPen( (isEnabled())?white:colorGroup().background() );
	paint.setBrush( (isEnabled())?white:colorGroup().background() );
	paint.drawRect( x, 1, width, boxheight);

	paint.setPen( colorGroup().dark() );
	paint.drawLine( x, 1 + boxheight, x, 1 );
	paint.drawLine( x, 1, x + width - 1, 1);
	
	paint.setPen( colorGroup().shadow() );
	paint.drawLine( x + 1, 1 + boxheight - 1, x + 1, 2);
	paint.drawLine( x + 1, 2, x + width - 2, 2);
	
	paint.setPen( colorGroup().light() );
	paint.drawLine( x + 2, 1 + boxheight - 1, x + width - 1, 1 + boxheight - 1);
	paint.drawLine( x + width - 1, 1 + boxheight - 1, x + width - 1, 2);
	
	paint.setPen( colorGroup().midlight() );
	paint.drawLine( x + 1, 1 + boxheight, x + width, 1 + boxheight );
	paint.drawLine( x + width, 1 + boxheight, x + width, 1 );
}

void JoySlider::paintEvent( QPaintEvent* )
{
	if (throttle == 0) {
		drawBox( lboxstart, boxwidth );
		drawBox( rboxstart, boxwidth );
	}
	else {
		drawBox( lboxstart, twidth );
	}
	
	if (!isEnabled()) return;

	QPainter paint( this );
	
	QColor bar;
	if (abs(JoyVal) < DeadZone) bar = gray;
	else if (abs(JoyVal) < XZone) bar = blue;
	else bar = red;
	paint.setPen( bar );
	paint.setBrush( bar );
	
	int width = (throttle == 0)?boxwidth:twidth;
	int barlen = abs(((width - 4) * JoyVal) / JOYMAX);
	if (JoyVal > 0)
		paint.drawRect( ((throttle == 0)?rboxstart:lboxstart) + 2, 3, barlen, boxheight - 3 );
	else if (JoyVal < 0)
		paint.drawRect( lboxstart + width - 2 - barlen, 3, barlen, boxheight - 3 );
	
	
	int point;
	QPointArray shape;
	
	paint.setPen( black );
	paint.setBrush( blue );
	if (throttle >= 0) {
		point = pointFor(DeadZone, false);
		shape.putPoints(0,5,
					point, boxheight - 4,
					point + 3, boxheight - 1,
					point + 3, boxheight + 2,
					point - 3, boxheight + 2,
					point - 3, boxheight - 1);
		paint.drawPolygon(shape);
	}
	
	if (throttle <= 0) {
		point = pointFor(DeadZone, true);
		shape.putPoints(0,5,
					point, boxheight - 4,
					point + 3, boxheight - 1,
					point + 3, boxheight + 2,
					point - 3, boxheight + 2,
					point - 3, boxheight - 1);
		paint.drawPolygon(shape);
	}



	paint.setBrush( red );
	if (throttle >= 0) {
		point = pointFor(XZone, false);
		shape.putPoints(0,5,
					point, boxheight - 4,
					point + 3, boxheight - 1,
					point + 3, boxheight + 2,
					point - 3, boxheight + 2,
					point - 3, boxheight - 1);
		paint.drawPolygon(shape);
	}
	
	if (throttle <= 0) {
		point = pointFor(XZone, true);
		shape.putPoints(0,5,
					point, boxheight - 4,
					point + 3, boxheight - 1,
					point + 3, boxheight + 2,
					point - 3, boxheight + 2,
					point - 3, boxheight - 1);
		paint.drawPolygon(shape);
	}
}

void JoySlider::mousePressEvent( QMouseEvent* e )
{
	int xpt = e->x();
	int result = 0;
	if (throttle <= 0 && abs( xpt - pointFor( XZone, true )) < 5) result = DRAG_XZ;
	else if (throttle <= 0 && abs( xpt - pointFor( DeadZone, true )) < 5) result = DRAG_DZ;
	else if (throttle >= 0 && abs( xpt - pointFor( XZone, false )) < 5) result = DRAG_XZ;
	else if (throttle >= 0 && abs( xpt - pointFor( DeadZone, false )) < 5) result = DRAG_DZ;
	dragging = result;
};

void JoySlider::mouseReleaseEvent( QMouseEvent* )
{
	dragging = 0;
}

void JoySlider::mouseMoveEvent( QMouseEvent* e )
{
	int xpt = e->x();
	if (dragging == DRAG_XZ)
	{
		XZone = valueFrom( xpt );
	}
	else if (dragging == DRAG_DZ)
	{
		DeadZone = valueFrom( xpt );
	}
	else return;
	update();
}
