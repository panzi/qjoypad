#include "joyslider.h"
//Added by qt3to4:

JoySlider::JoySlider( int dz, int xz, int val, QWidget* parent )
        :QWidget( parent )
{
    //initialize  :)

    boxwidth = 0;
    boxheight = 0;
    rboxstart = 0;
    rboxend = 0;
    lboxstart = 0;
    lboxend = 0;
    twidth = 0;
    tend = 0;

    throttle = 0;
    dragState = DragNone;

    joyval = val;
    deadzone = dz;
    xzone = xz;

    setMinimumHeight(20);
}

void JoySlider::setValue( int newval )
{
    //adjust the new position based on the throttle settings
    if (throttle == 0) joyval = newval;
    else if (throttle < 0) joyval = (newval + JOYMIN) / 2;
    else joyval = (newval + JOYMAX) / 2;
    //then redraw!
    update();
}

void JoySlider::setThrottle( int newval )
{
    //change throttle settings. This WILL quite likely cause minor issues with
    //status if the axis is not currently at zero, but these will be corrected
    //as soon as the axis moves again.
    throttle = newval;
    update();
}

int JoySlider::pointFor( int value, bool negative )
{
    //complicated... this just finds the pixel the given value should be.
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
    //the inverse of above  :)
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
    //when we resize, we need to recalculate a bunch of measurements.
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
    //draws a nice, pretty, 3d-styled box. that takes up the full height of the
    //widget but is defined by x-coordinate and width
    QPainter paint( this );

    paint.setPen( (isEnabled())?Qt::white:palette().window().color() );
    paint.setBrush( (isEnabled())?Qt::white:palette().window() );
    paint.drawRect( x, 1, width, boxheight);

    paint.setPen( palette().dark().color() );
    paint.drawLine( x, 1 + boxheight, x, 1 );
    paint.drawLine( x, 1, x + width - 1, 1);

    paint.setPen( palette().shadow().color() );
    paint.drawLine( x + 1, 1 + boxheight - 1, x + 1, 2);
    paint.drawLine( x + 1, 2, x + width - 2, 2);

    paint.setPen( palette().light().color() );
    paint.drawLine( x + 2, 1 + boxheight - 1, x + width - 1, 1 + boxheight - 1);
    paint.drawLine( x + width - 1, 1 + boxheight - 1, x + width - 1, 2);

    paint.setPen( palette().midlight().color() );
    paint.drawLine( x + 1, 1 + boxheight, x + width, 1 + boxheight );
    paint.drawLine( x + width, 1 + boxheight, x + width, 1 );
}

void JoySlider::paintEvent( QPaintEvent* )
{
    //when we need to redraw,
    //start by making our boxes
    if (throttle == 0) {
        drawBox( lboxstart, boxwidth );
        drawBox( rboxstart, boxwidth );
    }
    //or box, if we are in throttle mode.
    else {
        drawBox( lboxstart, twidth );
    }

    //if this is disabled, that's enough of that.
    if (!isEnabled()) return;

    //now we need to draw.
    QPainter paint( this );

    //prepare to draw a bar of the appropriate color
    QColor bar;
    if (abs(joyval) < deadzone) bar = Qt::gray;
    else if (abs(joyval) < xzone) bar = Qt::blue;
    else bar = Qt::red;
    paint.setPen( bar );
    paint.setBrush( bar );

    //find out the dimensions of the bar, then draw it
    int width = (throttle == 0)?boxwidth:twidth;
    int barlen = abs(((width - 4) * joyval) / JOYMAX);
    if (joyval > 0)
        paint.drawRect( ((throttle == 0)?rboxstart:lboxstart) + 2, 3, barlen, boxheight - 3 );
    else if (joyval < 0)
        paint.drawRect( lboxstart + width - 2 - barlen, 3, barlen, boxheight - 3 );


    //and now draw the tabs! We only need one set if we're doing a throttle mode
    //but we need two if we're not. However, it's important to draw the right
    //set of tabs depending on the mode! Negative throttle gets negative tabs.
    int point;
    QPolygon shape;

    paint.setPen( Qt::black );
    paint.setBrush( Qt::blue );
    if (throttle >= 0) {
        point = pointFor(deadzone, false);
        shape.putPoints(0,5,
                        point, boxheight - 4,
                        point + 3, boxheight - 1,
                        point + 3, boxheight + 2,
                        point - 3, boxheight + 2,
                        point - 3, boxheight - 1);
        paint.drawPolygon(shape);
    }

    if (throttle <= 0) {
        point = pointFor(deadzone, true);
        shape.putPoints(0,5,
                        point, boxheight - 4,
                        point + 3, boxheight - 1,
                        point + 3, boxheight + 2,
                        point - 3, boxheight + 2,
                        point - 3, boxheight - 1);
        paint.drawPolygon(shape);
    }



    paint.setBrush( Qt::red );
    if (throttle >= 0) {
        point = pointFor(xzone, false);
        shape.putPoints(0,5,
                        point, boxheight - 4,
                        point + 3, boxheight - 1,
                        point + 3, boxheight + 2,
                        point - 3, boxheight + 2,
                        point - 3, boxheight - 1);
        paint.drawPolygon(shape);
    }

    if (throttle <= 0) {
        point = pointFor(xzone, true);
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
    //store the x coordinate.
    int xpt = e->x();
    //see if this happened near one of the tabs. If so, start dragging that tab.
    if (throttle <= 0 && abs( xpt - pointFor( xzone, true )) < 5) dragState = DragXZ;
    else if (throttle <= 0 && abs( xpt - pointFor( deadzone, true )) < 5) dragState = DragDZ;
    else if (throttle >= 0 && abs( xpt - pointFor( xzone, false )) < 5) dragState = DragXZ;
    else if (throttle >= 0 && abs( xpt - pointFor( deadzone, false )) < 5) dragState = DragDZ;
    else dragState = DragNone;
}

void JoySlider::mouseReleaseEvent( QMouseEvent* )
{
    //when the mouse releases, all dragging stops.
    dragState = DragNone;
}

void JoySlider::mouseMoveEvent( QMouseEvent* e )
{
    //get the x coordinate
    int xpt = e->x();
    //if we're dragging, move the appropriate tab!
    switch (dragState) {
    case DragXZ:
        xzone = valueFrom( xpt );
        break;

    case DragDZ:
        deadzone = valueFrom( xpt );
        break;

    default:
        return;
    }
    //if we moved a tab, redraw!
    update();
}
