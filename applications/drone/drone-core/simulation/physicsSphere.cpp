#include "physicsSphere.h"

#include <mesh3DScene.h>

PhysicsSphere::PhysicsSphere(): MaterialObject ()
{
    radius = 1.0;
    ////L_INFO << "Created default PhysSphere";
}

void PhysicsSphere::setPos(const Vector3dd &pos)
{
    this->setPosition(Affine3DQ(pos));
}

PhysicsSphere::PhysicsSphere(const Affine3DQ &pos, double r, double m) :
    MaterialObject(pos, m)
{
    radius = r;
    ////L_INFO << "Created PhysSphere with pos: " << *pos << " , radius: " << *r << " , mass: " << *m;
}

void PhysicsSphere::saveMesh(const std::string &name)
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
    mesh->addIcoSphere(position(), 2, 2);
    mesh->popTransform();

    //mesh->dumpPLY(name+".ply");
    delete_safe(mesh);
}

void PhysicsSphere::drawMesh(corecvs::Mesh3D &mesh)
{
    if(mesh.hasColor) {
        mesh.setColor(color);
    }

    mesh.addIcoSphere(position(), radius, 1);


    /*
    //mesh is not using because I'm a little bit genius (really don't know how to make best architecture by now)
    Mesh3D *sphereMesh = new Mesh3D;
    corecvs::RGBColor color = corecvs::RGBColor(255, 255, 0);
    sphereMesh->switchColor();

    //Should be fixed with gathering coordinates of physMainObject or mesh should be transfering inside physMainObject
    sphereMesh->mulTransform(Affine3DQ(Vector3dd::Zero()));

    sphereMesh->setColor(color);
    sphereMesh->addIcoSphere(getPosVector(), 2, 2);
    sphereMesh->popTransform();

    //mesh->dumpPLY(name+".ply");
    delete_safe(sphereMesh);
    ////L_INFO << "Successfully drew sphere mesh";
    */
}

void PhysicsSphere::calcMoment()
{
    Vector3dd m = force() ^ position();
    addMoment(m);
    ////L_INFO << "Sphere has no calcMoment implementation";
}

void PhysicsSphere::calcForce()
{
    ////L_INFO << "Sphere has no calcForce() implementation";
}

void PhysicsSphere::addToMesh(Mesh3D &mesh)
{
    mesh.addIcoSphere(position(), radius, 3);
}
