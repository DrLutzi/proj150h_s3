#include <QApplication>
#include "mainwindow.h"
#include "viewer.h"
#include "boost/math/special_functions/factorials.hpp"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

    QGLFormat glFormat;
    glFormat.setVersion( 3, 3 );
    glFormat.setProfile( QGLFormat::CoreProfile ); // Requires >=Qt-4.8.0
    glFormat.setSampleBuffers( true );
	QGLFormat::setDefaultFormat(glFormat);

    MainWindow mw;
    mw.show();
    //   Viewer view;
        //view.show();

	return a.exec();

}
