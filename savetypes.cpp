#include "savetypes.h"


QString Name( int index )
{
	QString fullname;
	
	switch (index)
	{
		case UP: 	fullname = "Up";
						break;
		case DOWN: 	fullname = "Down";
						break;
		case LEFT:	fullname = "Left";
						break;
		case RIGHT:	fullname = "Right";
						break;
		default:
						fullname = "Button " + QString::number( index - DIRECTIONS + 1);
	}
	return fullname;
}














JoyKeys::JoyKeys()
{
	Sensitivity = 7000;
	Buttons.setAutoDelete( true );
}

int JoyKeys::read( QTextStream* stream )
{
	Buttons.clear();

	QString line;
	QStringList words;
	bool okay, parsedok;
	int index, value;
	int result = 0;

	do
	{
		line = stream->readLine().lower();
		parsedok=false;
		words = QStringList::split(QRegExp("\\s+"), line);
		if (words.count() > 2)
		{
			if (words.count() > 3)
			{
				if (words[0] == "button" && words[2] == "=")
				{
					index = words[1].toInt(&okay);
					if (okay)
					{
						value = words[3].toInt(&okay);
						if (okay)
						{
							Buttons.replace(DIRECTIONS + index - 1, new int(value));
							parsedok=true;
						}
					}
				}
			}
			else
			{
				if (words[1] == "=")
				{
					value = words[2].toInt(&okay);
					if (okay)
					{
						index = -1;
						if (words[0] == "sensitivity")
						{
							if (value < 0) Sensitivity = 0;
							else if (value > 32766) Sensitivity = 32766;
							else Sensitivity = value;
							parsedok=true;
						}
						else if (words[0] == "up") index = UP;
						else if (words[0] == "down") index = DOWN;
						else if (words[0] == "left") index = LEFT;
						else if (words[0] == "right") index = RIGHT;

						if (index != -1)
						{
							if (value > 122 || value < 0) value = 0;
							Buttons.replace( index, new int(value));
							parsedok=true;
						}
					}
				}
			}
		}
		if (! parsedok)
		{
			if (words.count() != 0 && words[0] != "}" && words[0][0] != '#')
			result = -1;
		}
		if (line == NULL || line.find('{') > -1)
		{
			result = -2;
			break;
		}
	} while (line.find('}') == -1);
	return result;
}

void JoyKeys::write( QTextStream* stream )
{
	*stream << "\tSensitivity = " << Sensitivity << "\n";
	
	QIntDictIterator<int> it( Buttons );
	for ( ; it.current(); ++it )
	{
		if (*it.current() != 0)
			*stream << "\t" << Name( it.currentKey() ) << " = " << *it.current() << "\n";
	}
	*stream << "}\n\n";
}

int JoyKeys::getButton( int index )
{
	if (Buttons[index] == 0)
		return 0;
	else return *Buttons[index];
}

void JoyKeys::setButton( int index, int value )
{
	Buttons.replace(index, new int(value));
}

int JoyKeys::getSensitivity()
{
	return Sensitivity;
}

void JoyKeys::setSensitivity( int value )
{
	Sensitivity = value;
}

















OneSave::OneSave()
{
	Name = "";
	Changed = false;
	Sticks.setAutoDelete( true );
	File = 0;
}

bool OneSave::open( QString filename )
{
	File = new QFile( QDir::homeDirPath() + "/.qjoypad/" + filename );
	
	if (! File->open( IO_ReadOnly ))
	{
		QMessageBox mb( "Error loading file",
			"Could not open " + filename,
			QMessageBox::Critical,
			QMessageBox::Ok | QMessageBox::Default,
			QMessageBox::NoButton,
			QMessageBox::NoButton);
		mb.exec();
		File = 0;
		return false;
	}
		
	QTextStream stream( File );
		
	Sticks.clear();
	Name = "";
	Changed = false;
	
	QString line;
	QStringList words;
	bool okay, parsedok;
	int index,start,end,res;
	int result = 0;
	JoyKeys* key;

	do
	{
		line = stream.readLine();
		parsedok=false;
		words = QStringList::split(QRegExp("\\s+"), line.lower());

		if (words.count() > 2)
		{
			if (words[0] == "name" && words[1] == "=")
			{
				start = line.find( '"' );
				end = line.findRev( '"' );
				if (start > -1 && end > -1 & (start + 1) < end )
				{
					Name = line.mid(start + 1, end - start - 1);
					parsedok=true;
				}
			}
			else if (words[0] == "joystick" && words[2] == "{")
			{
				index = words[1].toInt(&okay);
				if (okay)
				{
					key = new JoyKeys();
					res = key->read( &stream );
					if (res != 0) result = res;
					if (result == -2) break;
					Sticks.replace( index - 1, key );
					parsedok = true;
				}
			}
		}
		if (! parsedok)
		{
			if (words.count() != 0 && words[0][0] != '#') result = -1;
		}
	} while (! stream.eof() );
	
	File->close();
	if (Name == "")
	{
		QMessageBox mb( "Error loading file",
			"No name given for layout in " + filename,
			QMessageBox::Critical,
			QMessageBox::Ok | QMessageBox::Default,
			QMessageBox::NoButton,
			QMessageBox::NoButton);
		mb.exec();
		return false;
	}
	if (result == -2)
	{
		QMessageBox mb( "Error loading file",
			"Unterminated joystick field in " + filename,
			QMessageBox::Critical,
			QMessageBox::Ok | QMessageBox::Default,
			QMessageBox::NoButton,
			QMessageBox::NoButton);
		mb.exec();
		return false;
	}
	if (result == -1)
	{
		QMessageBox mb( "Error loading file",
			"Not all options were fully recognized in " + filename + "\nLoading this file will work, but if you make any changes\nto the layout " + Name + " the changes will be saved and\nthe unrecognized options will be forgotten. Should I go\nahead and load the file, even though some data might be lost?",
			QMessageBox::Warning,
			QMessageBox::Yes | QMessageBox::Default,
			QMessageBox::No | QMessageBox::Escape,
			QMessageBox::NoButton);
		if (mb.exec() == QMessageBox::No) return false;
	}
	return true;
}

void OneSave::save()
{
	if (!Changed) return;
	if (File == 0)
	{
		QString root = QDir::homeDirPath() + "/.qjoypad/";
		QString filename = Name.lower().replace(QRegExp("\\W"),"");
		File = new QFile( root + filename +".lyt" );
		if (File->exists())
		{
			for (int i = 0; i < 256; i++)
			{
				File->setName( root + filename + QString::number( i ) + ".lyt");
				if (!File->exists()) break;
			}
			if (File->exists())
			{
				QMessageBox mb( "Error saving file",
					"Too many layouts have very similar names!\nthe layout called " + Name + " could not be saved.",
					QMessageBox::Critical,
					QMessageBox::Ok | QMessageBox::Default,
					QMessageBox::NoButton,
					QMessageBox::NoButton);
				mb.exec();
				return;
			}
		}
	}
	if (! File->open( IO_WriteOnly ))
	{
		QMessageBox mb( "Error saving file",
			"Could not save file " + File->name(),
			QMessageBox::Critical,
			QMessageBox::Ok | QMessageBox::Default,
			QMessageBox::NoButton,
			QMessageBox::NoButton);
		mb.exec();
		return;
	}

	QTextStream stream( File );

	stream << "# " << NAME << " layout file\n\n";
		
	stream << "Name = \"" << Name << "\"\n\n";
	
	QIntDictIterator<JoyKeys> it( Sticks );
	for ( ; it.current(); ++it )
	{
		stream << "Joystick " << it.currentKey() + 1 << " {\n";
		it.current()->write( &stream );
	}

	File->close();
}

void OneSave::remove()
{
	if (File)
	{
		if (!File->remove())
		{
			QMessageBox mb( "Error deleting file",
				"Could not remove file " + File->name(),
				QMessageBox::Warning,
				QMessageBox::Ok | QMessageBox::Default,
				QMessageBox::NoButton,
				QMessageBox::NoButton);
			mb.exec();
		}
	}
}

JoyKeys* OneSave::getStick( int index )
{
	if (Sticks[index] == 0)
		return new JoyKeys;
	else return Sticks[index];
}

void OneSave::setStick( int index, JoyKeys* keys )
{
	Sticks.replace( index, keys );
}

QString OneSave::name()
{
	return Name;
}

void OneSave::setName( QString newname )
{
	Name = newname;
}

bool OneSave::hasChanged()
{
	return Changed;
}

void OneSave::setChanged()
{
	Changed = true;
}
