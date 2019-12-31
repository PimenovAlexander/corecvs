#include <QApplication>

#include "mainWindow.h"

using namespace corecvs;
using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();
    mainWindow.setImage("sdf");
    return app.exec();
}


