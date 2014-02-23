#include "buttonw.h"


ButtonWidget::ButtonWidget( Button* b, QWidget* parent )
        : FlashButton(QString::null, QString::null, parent), on(false), button(b) {
    update();
}

void ButtonWidget::jsevent( int val ) {
    bool newOn = (val == 1);
    if (on != newOn) {
        on = newOn;
        flash();
    }
}

void ButtonWidget::update() {
    setText(button->status());
}

void ButtonWidget::mouseReleaseEvent( QMouseEvent* e ) {
    ButtonEdit* be = new ButtonEdit(button);
    be->exec();
    delete be;

    update();
    FlashButton::mouseReleaseEvent( e );
}
