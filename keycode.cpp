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
0, //(Escape)
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
"R Alt"
};

const char* ktos( int keycode )
{
	if (keycode > 113 || keycode < 0) keycode = 0;
	return keynames[keycode];
}




GetKey::GetKey( QString button )
		 :QDialog( 0 )
{
	setCaption( "Choose a key" );

	//I'd use a QLabel, but that steals x11Events!
	//So, I'll draw the text directly. That means
	//I need to resolve the size of the dialog by hand:
	Text = "Choose a new key for " + button;
	QRect rect = fontMetrics().boundingRect( Text );
	Text += "\n(Escape for no key)";
	setFixedSize( QSize( rect.width() + 20, rect.height()*2 + 20 ) );
}

bool GetKey::x11Event( XEvent* e )
{
	//On a key press, return the key and quit
	if (e->type == KeyRelease) finish( e->xkey.keycode );

	//On a redraw, display the label
	if (e->type == Expose)
	{
		QPainter paint( this );
		paint.drawText( rect(), AlignCenter, Text );
	}
	
	return true;
}


void GetKey::finish( int key )
{
	done( key );
}
