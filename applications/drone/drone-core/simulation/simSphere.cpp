#include "simSphere.h"

#include <3d/mesh3DScene.h>

SimSphere::SimSphere()
{
}


SimSphere::SimSphere(Vector3dd c, double r) :
    SimObject(c),
    radius(r)
{
}

SimSphere::SimSphere(Vector3dd c, double r, corecvs::RGBColor col) :
    SimObject(c),
    radius(r)
{
    mainColour=col;
}


void SimSphere::saveMesh(const std::string &name)
{
    cout<<"here"<<endl;
    Affine3DQ copterPos = Affine3DQ::Shift(10,10,10);

    //Mesh3DDecorated *mesh = new Mesh3DDecorated;
    Mesh3DScene *scene = new Mesh3DScene;
    Mesh3D *mesh = new Mesh3D();
    scene->setMesh(mesh);

    mesh->switchColor();

    mesh->mulTransform(copterPos);

    mesh->setColor(RGBColor::Red());
    mesh->addIcoSphere(position, 2, 2);

    mesh->popTransform();

    //mesh->dumpPLY(name+".ply");

    delete_safe(mesh);
}


void SimSphere::drawMesh(Vector3dd coords, double radius, corecvs::RGBColor color)
{
    Affine3DQ copterPos = Affine3DQ::Shift(10,10,10);

    //Mesh3DDecorated *mesh = new Mesh3DDecorated;
    Mesh3D *mesh = new Mesh3D;

    mesh->switchColor();

    mesh->mulTransform(copterPos);

    mesh->setColor(color);
    mesh->addIcoSphere(coords, 2, 2);

    mesh->popTransform();

    //mesh->dumpPLY(name+".ply");
    delete_safe(mesh);
}

void SimSphere::addToMesh(Mesh3D &mesh)
{
    mesh.addIcoSphere(position, radius, 3);
}
