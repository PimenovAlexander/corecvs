/**
 * \file main_test_camerafixture.cpp
 * \brief This is the main file for the test camerafixture 
 *
 * \date дек. 30, 2015
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include "gtest/gtest.h"

#include "global.h"
#include "fixtureScene.h"
#include "printerVisitor.h"


using namespace std;
using namespace corecvs;

TEST(Fixture, testAllocations)
{    
    FixtureScene *scene = new FixtureScene();
    CameraFixture *fixture1 = scene->createCameraFixture();
    fixture1->name = "Fixture1";

    FixtureCamera *camera1 = scene->createCamera();
    camera1->nameId = "Camera1";
    FixtureCamera *camera2 = scene->createCamera();
    camera2->nameId = "Camera2";

    scene->addCameraToFixture(camera1, fixture1);

    scene->dumpInfo(cout);


    delete_safe(scene);
}

TEST(Fixture, testVisitors)
{
    cout << "----------------Running the test-------------" << std::endl;
    FixtureScene *scene = new FixtureScene();
    CameraFixture *fixture1 = scene->createCameraFixture();
    fixture1->name = "Fixture1";

    FixtureCamera *camera1 = scene->createCamera();
    camera1->nameId = "Camera1";
    FixtureCamera *camera2 = scene->createCamera();
    camera2->nameId = "Camera2";

    scene->addCameraToFixture(camera1, fixture1);

    scene->dumpInfo(cout);
    PrinterVisitor visitor;
    scene->accept<PrinterVisitor, FixtureScene>(visitor);




    delete_safe(scene);
}

FixtureScene *createTestScene()
{
    FixtureScene *scene = new FixtureScene();

    CameraFixture *fixture1 = scene->createCameraFixture();
    fixture1->name = "Fixture1";
    CameraFixture *fixture2 = scene->createCameraFixture();
    fixture2->name = "Fixture2";
    CameraFixture *fixture3 = scene->createCameraFixture();
    fixture3->name = "Fixture3";

    //SYNC_PRINT(("%p %p %p\n", fixture1, fixture2, fixture3));
    //SYNC_PRINT(("%p %p %p\n", scene->fixtures[0], scene->fixtures[1], scene->fixtures[2]));

    //SYNC_PRINT(("Length: %d\n", scene->fixtures.size()));

    fixture1->setLocation(Affine3DQ(Vector3dd( 0, 0,-10)));
    fixture2->setLocation(Affine3DQ(Vector3dd(50, 0,  0)));
    fixture3->setLocation(Affine3DQ(Vector3dd( 0,50, 30)));

    for (int j = 0; j < 5; j++)
    {

        CameraModel model;
        double angle = degToRad(360.0 / 5 * j);
        CameraLocationAngles ang(angle, 0.0, 0.0);
        Affine3DQ position(Quaternion::RotationZ(angle), Vector3dd::FromCylindrical(angle, 5.0, 0.0));

        model.extrinsics = CameraLocationData(position);
        model.intrinsics.principal.x() = 100;
        model.intrinsics.principal.y() = 100;

        model.intrinsics.focal.x() = 100;
        model.intrinsics.focal.y() = 100;

        model.intrinsics.size = Vector2dd(200, 200);

        //SYNC_PRINT(("Length: %d\n", scene->fixtures.size()));

        for (size_t i = 0; i < scene->fixtures.size(); i++)
        {
            CameraFixture *fixture = scene->fixtures[i];
            FixtureCamera *camera = scene->createCamera();
            char buffer[100];
            snprintf2buf(buffer, "camera %d(%d)", j, i);
            camera->nameId = buffer;
            camera->copyModelFrom(model);
            //scene->positionCameraInFixture(fixture, camera, camera->extrinsics.toAffine3D());
            //scene->positionCameraInFixture(fixture, camera, position);
            scene->positionCameraInFixture(fixture, camera, position);

            //SYNC_PRINT(("Adding camera %s to fixture %d %p\n", buffer, i, fixture));

            scene->addCameraToFixture(camera, fixture);

        }

    }

    for (int i = 0; i < 5; i++)
    {
        SceneFeaturePoint *point = scene->createFeaturePoint();
        point->setPosition(Vector3dd::FromCylindrical(degToRad(i * (360 / 5)), 130.0, 0.5));
        point->color = RGBColor::rainbow(i / (5.0 - 1));
    }

    scene->projectForward(SceneFeaturePoint::POINT_ALL);

    return scene;
}


TEST(Fixture, testMerge)
{
  FixtureScene *scene1 = createTestScene();
  FixtureScene *scene2 = new FixtureScene();

  scene2->merge(scene1);

  scene1->dumpInfo();
  scene2->dumpInfo();

  delete_safe(scene1);
  delete_safe(scene2);
}
