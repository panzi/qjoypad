#include <qapplication.h>
#include <qdialog.h>
#include "form.h"

int main( int argc, char **argv )
{

  QApplication a( argc, argv );
  
  QDialog* form = new DMain();
  
  a.setMainWidget( form );
  form->show();
  
  a.exec();
}
