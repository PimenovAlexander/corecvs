#include "core/utils/utils.h"
#include "core/geometry/mesh3d.h"
#include <core/geometry/mesh3DDecorated.h>
#include <QApplication>
#include <bits/stdc++.h>


#include "physicsMainWidget.h"

using namespace corecvs;
using namespace std;

int mainExample1()
{/*
    Affine3DQ copterPos = Affine3DQ::Shift(10,10,10);

    Mesh3DDecorated *mesh = new Mesh3DDecorated;
    mesh->switchColor();
    mesh->mulTransform(copterPos);

    mesh->setColor(RGBColor::Green());
    mesh->addIcoSphere(Vector3dd( 5, 5, 0), 2, 2);
    mesh->addIcoSphere(Vector3dd(-5, 5, 0), 2, 2);

    mesh->setColor(RGBColor::Red());
    mesh->addIcoSphere(Vector3dd( 5, -5, 0), 2, 2);
    mesh->addIcoSphere(Vector3dd(-5, -5, 0), 2, 2);
    mesh->popTransform();

    mesh->dumpPLY("out.ply");
    printf("Hello\n");

*/

    return 0;
}


int main(int argc, char *argv[])
{
    mainExample1();

    SET_HANDLERS();

    Q_INIT_RESOURCE(main);
    Vector3d<int> f;
    //QTRGB24Loader::registerMyself();

    printf("Starting cloudView...\n");
    QApplication app(argc, argv);

    PhysicsMainWidget mainWindow;
    mainWindow.show();
    app.exec();
}
