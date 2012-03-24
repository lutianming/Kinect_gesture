#include <QtGui/QApplication>
#include "mainwindow.h"
#include "opennicontroler.h"
#include "kinectreader.h"
#include <QGraphicsView>

int main(int argc, char *argv[])
{
    // initial OpenNI
    OpenNIControler  mOpenNI;
    if( !mOpenNI.initial() ){
        std::cout << "init error" << std::endl;
        return 1;
    }
    QApplication a(argc, argv);
    KinectReader KReader( mOpenNI);

    MainWindow window(KReader);
    window.show();
    KReader.init();
      // start!
    KReader.start();

    return a.exec();
}
