#pragma once

#include <QDialog>
#include <QString>
#include <QLabel>
#include <QKeyEvent>
#include <QMouseEvent>

class KeyDialog : public QDialog {
    Q_OBJECT

protected:
    bool m_acceptMouse;
    QLabel m_text;

    bool m_isMouse;
    quint32 m_value;

    KeyDialog( const QString& button, bool m = false, QWidget* parent = 0 );
    void keyPressEvent( QKeyEvent* );
    void keyReleaseEvent( QKeyEvent* );
    void mouseReleaseEvent( QMouseEvent* );
    void wheelEvent( QWheelEvent *event );

public:
    static int getKey( const QString& button, bool acceptMouse = false, bool* mousePressed = 0, QWidget* parent = 0 );

};
