#include "keycode.h"


const char* keynames[] =
{
"[NO KEY]",
0,
0,
0,
0,
0,
0,
0,
0,
"Escape",
"1",
"2",
"3",
"4",
"5",
"6",
"7",
"8",
"9",
"0",
"-",
"=",
"Backspace",
"Tab",
"Q",
"W",
"E",
"R",
"T",
"Y",
"U",
"I",
"O",
"P",
"[",
"]",
"Return",
"L Ctrl",
"A",
"S",
"D",
"F",
"G",
"H",
"J",
"K",
"L",
";",
"'",
"`",
"L Shift",
"\\",
"Z",
"X",
"C",
"V",
"B",
"N",
"M",
",",
".",
"/",
"R Shift",
"KP *",
"L Alt",
"Space",
0, //(Caps Lock)
"F1",
"F2",
"F3",
"F4",
"F5",
"F6",
"F7",
"F8",
"F9",
"F10,",
0, //(Num Lock)
0, //(Scroll Lock)
"KP 7",
"KP 8",
"KP 9",
"KP -",
"KP 4",
"KP 5",
"KP 6",
"KP +",
"KP 1",
"KP 2",
"KP 3",
"KP 0,",
"KP .",
0,
0,
0,
"F11",
"F12",
"Home",
"Up",
"PageUp",
"Left",
0,
"Right",
"End",
"Down",
"PageDown",
"Insert",
"Delete",
"KP Enter",
"R Ctrl",
0, //(Pause)
0, //(Print Screen)
"KP /",
"R Alt",
0, //(Break)
"L Win",
"R Win",
"Menu",
"F13",
"F14",
"F15",
"F16",
"F17"
};

const char* ktos( int keycode )
{
	if (keycode > MAXKEY || keycode < 0) keycode = 0;
	return keynames[keycode];

//	if (keycode == 0) return "[NO KEY]";
//	return XKeysymToString(XKeycodeToKeysym(display, keycode,0));
}




GetKey::GetKey( QString button, bool m )
		 :QDialog( )
{
	mouse = m;
	setCaption( "Choose a key" );

	//I'd use a QLabel, but that steals x11Events!
	//So, I'll draw the text directly. That means
	//I need to resolve the size of the dialog by hand:
	Text = "Choose a new key ";
	if (mouse) Text += "or mouse button ";
	Text += "for " + button;
	QRect rect = fontMetrics().boundingRect( Text );
	Text += "\n(Escape for no key)";
	setFixedSize( QSize( rect.width() + 20, rect.height()*2 + 20 ) );
	setFocus();
}

bool GetKey::x11Event( XEvent* e )
{
	//On a key press, return the key and quit
	//escape == cancel
	if (e->type == KeyRelease) {
		finish( (e->xkey.keycode == 9)?0:e->xkey.keycode );
		return true;
	}
	if (mouse && e->type == ButtonRelease) {
		finish ( e->xbutton.button + 200);
		return true;
	}

	return false;
}

void GetKey::paintEvent ( QPaintEvent * ) {
	QPainter paint( this );
	paint.drawText( rect(), AlignCenter, Text );
}

void GetKey::finish( int key )
{
	done( key );
}







KeyButton::KeyButton( QString name, int val, QWidget* parent, bool m, bool nowMouse)
	:QPushButton(nowMouse?"Mouse " + QString::number(val):QString(ktos(val)), parent) {
	mouse = m;
	buttonname = name;
	value = val;
	connect( this, SIGNAL( clicked() ), SLOT( onClick() ));
}

void KeyButton::onClick() {
	value = GetKey( buttonname, mouse ).exec();
	if (value > 200) {
		mouseClicked = true;
		value -= 200;
		setText( "Mouse " + QString::number(value));
	}
	else {
		mouseClicked = false;
		setText( ktos(value));
	}
}
