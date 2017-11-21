#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <QtXml/QDomDocument>
#include <vector>
#include <fstream>

#include "core/buffers/rgb24/abstractPainter.h"
#include "core/fileformats/bmpLoader.h"

#include "core/camerafixture/fixtureScene.h"
#include "core/camerafixture/cameraFixture.h"

#include "core/math/vector/vector3d.h"
#include "xmlSetter.h"
#include "xmlGetter.h"

#include "jsonGetter.h"
#include "jsonSetter.h"

#include "core/reflection/jsonPrinter.h"

using namespace std;


void testJSON_FixtureScene()
{
    cout << "----------------Running the test-------------" << std::endl;
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

        for (size_t i = 0; i < scene->fixtures().size(); i++)
        {
            CameraFixture *fixture = scene->fixtures()[i];
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

    cout << "Original scene:" << endl;
    cout << "================================" << endl;
    scene->dumpInfo(cout);
    cout << "================================" << endl;

    {
        JSONSetter setter("scene.json");
        setter.visit(*scene, "scene");
    }
    delete_safe(scene);



    /** Now loading **/
    FixtureScene *loaded = new FixtureScene();
    {
        JSONGetter getter("scene.json");
        getter.visit(*loaded, "scene");
    }

    cout << "Loaded scene:" << endl;
    cout << "================================" << endl;
    loaded->dumpInfo(cout);
    cout << "================================" << endl;
}

void testJSON_StereoScene(int targetSize = 3, bool useDistortion = false )
{
    cout << "----------------Running the test-------------" << std::endl;
    if (useDistortion) {
        cout << "Distortion would be applied" << endl;
    }
    FixtureScene *scene = new FixtureScene();

    CameraFixture *fixture = scene->createCameraFixture();
    fixture->name = "Z";


    CameraModel model;
    model.intrinsics.principal.x() = 320;
    model.intrinsics.principal.y() = 240;
    model.intrinsics.focal.x() = 589;
    model.intrinsics.focal.y() = 589;
    model.intrinsics.size = Vector2dd(640, 480);
    model.distortion.mKoeff = std::vector<double>({std::numeric_limits<double>::min()});

    if (useDistortion)
    {
        model.distortion.setPrincipalPoint(Vector2dd(400, 150));
        model.distortion.setShift(Vector2dd(6, 8));

        model.distortion.setTangentialX(0.01);
        model.distortion.setTangentialY(0.01);
        model.distortion.mKoeff.push_back(0.5);
        model.distortion.setNormalizingFocal(model.intrinsics.size.l2Metric());
    }



    FixtureCamera *camera1 = scene->createCamera();
    FixtureCamera *camera2 = scene->createCamera();

    camera1->nameId = "1";
    camera1->copyModelFrom(model);
    camera2->nameId = "2";
    camera2->copyModelFrom(model);

    scene->addCameraToFixture(camera1, fixture);
    scene->addCameraToFixture(camera2, fixture);

    scene->positionCameraInFixture(fixture, camera1, Affine3DQ(Vector3dd::Zero()));
    scene->positionCameraInFixture(fixture, camera2
                                   , Affine3DQ(Vector3dd::OrtY() * 10.0));

    RGB24Buffer *image1 = new RGB24Buffer(model.intrinsics.h(), model.intrinsics.w(), RGBColor::gray(39));
    RGB24Buffer *image2 = new RGB24Buffer(model.intrinsics.h(), model.intrinsics.w(), RGBColor::gray(56));
    RGB24Buffer *image3 = new RGB24Buffer(model.intrinsics.h(), model.intrinsics.w(), RGBColor::gray(75));

    image1->checkerBoard(20, RGBColor::Gray(50));
    image2->checkerBoard(20, RGBColor::Gray(50));
    image3->checkerBoard(20, RGBColor::Gray(50));

    image1->drawCrosshare2(image1->w / 2, image1->h / 2, RGBColor::Blue());
    image2->drawCrosshare2(image2->w / 2, image2->h / 2, RGBColor::Blue());
    image3->drawCrosshare2(image3->w / 2, image3->h / 2, RGBColor::Blue());

    AbstractPainter<RGB24Buffer> painter1(image1);
    AbstractPainter<RGB24Buffer> painter2(image2);
    AbstractPainter<RGB24Buffer> painter3(image3);

    painter1.drawCircle(10, 10, 5, RGBColor::White());
    painter2.drawCircle(10, 10, 7, RGBColor::White());
    painter3.drawCircle(10, 10, 9, RGBColor::White());


    int count = 0;
    int namedCount = 0;

    double MAX_SHIFT = 5.0;

    double delta = MAX_SHIFT / (targetSize - 1);

    for (double x = 0.0; x <= MAX_SHIFT; x += delta)
        for (double y = 0.0; y <= MAX_SHIFT; y += delta)
            for (double z = 0.0; z <= MAX_SHIFT; z += delta)
            {
                SceneFeaturePoint *point  = scene->createFeaturePoint();

                if (namedCount < 30) {
                    char buffer[100];
                    snprintf2buf(buffer, "Test Point %d", count++);
                    point->type = SceneFeaturePoint::POINT_UNKNOWN;
                    point->name = buffer;
                    namedCount++;
                }

                point->setPosition(Vector3dd(20.0 + x , y, z));
                point->color = RGBColor::rainbow((x + y + z) / 25.0);
            }



    scene->projectForward(SceneFeaturePoint::POINT_ALL);

    /**
     * Add data to observations
     **/
    count = 1;
    for (SceneFeaturePoint *point: scene->featurePoints())
    {
        for (auto &obs: point->observations)
        {
            count++;
            KeyPointArea keyPointArea(5.0, 1.0, 1.0, 1);
            obs.second.keyPointArea = keyPointArea;
        }
    }


    /**
     *   Additional camara
     **/

    FixtureCamera *camera3 = scene->createCamera();

    camera3->nameId = "3";
    camera3->copyModelFrom(model);

    scene->addCameraToFixture(camera3, fixture);
    scene->positionCameraInFixture(fixture,
                                   camera3,
                        Affine3DQ( Quaternion::RotationZ(degToRad(0)), Vector3dd(3, 10, 0)));

    for (SceneFeaturePoint *point: scene->featurePoints())
    {
        point->projectForward(camera3, fixture, true);
    }

    for (size_t i = 0; i < scene->featurePoints().size(); i++)
    {
        SceneFeaturePoint *point = scene->featurePoints()[i];
        if (point->getObservation(camera1) != NULL) {
            Vector2dd p = point->getObservation(camera1)->getDist();
            painter1.drawCircle(p.x(), p.y(), 3, point->color);
        }

        if (point->getObservation(camera2) != NULL) {
            Vector2dd p = point->getObservation(camera2)->getDist();
            painter2.drawCircle(p.x(), p.y(), 3, point->color);
        }

        if (point->getObservation(camera3) != NULL) {
            Vector2dd p = point->getObservation(camera3)->getDist();
            painter3.drawCircle(p.x(), p.y(), 3, point->color);
        }
    }

    std::string name1 = std::string("SP") +  fixture->name + camera1->nameId + ".bmp";
    std::string name2 = std::string("SP") +  fixture->name + camera2->nameId + ".bmp";
    std::string name3 = std::string("SP") +  fixture->name + camera3->nameId + ".bmp";

    if (!useDistortion)
    {
        BMPLoader().save(name1, image1);
        BMPLoader().save(name2, image2);
        BMPLoader().save(name3, image3);
        SYNC_PRINT(("Saving ideal images\n"));

    } else {
        FixedPointDisplace displacer(model.distortion, model.intrinsics.h(), model.intrinsics.w());
        RGB24Buffer *dist1 = image1->doReverseDeformationBlPrecomp(&displacer, displacer.h, displacer.w);
        RGB24Buffer *dist2 = image2->doReverseDeformationBlPrecomp(&displacer, displacer.h, displacer.w);
        RGB24Buffer *dist3 = image3->doReverseDeformationBlPrecomp(&displacer, displacer.h, displacer.w);

        BMPLoader().save(name1, dist1);
        BMPLoader().save(name2, dist2);
        BMPLoader().save(name3, dist3);

        SYNC_PRINT(("Saving distorted images\n"));
    }


    /**
     *  Camera prototype
     **/
    CameraPrototype *testProto = scene->createCameraPrototype();
    testProto->nameId = "Test Prototype";
    testProto->copyModelFrom(model);

    /** Geometry **/
    FixtureSceneGeometry *geometry = scene->createSceneGeometry();
    geometry->frame   = PlaneFrame(Vector3dd(100,0,0), Vector3dd(0,1,0), Vector3dd(0,0,1));
    geometry->polygon = Polygon::RegularPolygon(5, Vector2dd::Zero(), 50, 0);
    geometry->relatedPoints.push_back(scene->featurePoints()[0]);
    geometry->relatedPoints.push_back(scene->featurePoints()[1]);
    geometry->relatedPoints.push_back(scene->featurePoints()[2]);



    cout << "Original scene:" << endl;
    cout << "================================" << endl;
    scene->dumpInfo(cout);
    cout << "================================" << endl;

    {
        JSONSetter setter("stereo.json");
        setter.visit(*scene, "scene");
    }
    {
        ofstream file;
        file.open("stereo_new.json", std::ofstream::out);
        {
            JSONPrinter printer(file);
            printer.visit(*scene, "scene");
        } // Stream would be finalised on JSONPrinter destructor
        file.close();
    }

    delete_safe(scene);

}

void testJSON_StereoRecheck()
{
     FixtureSceneFactory::getInstance()->print();
     FixtureScene *scene = FixtureSceneFactory::getInstance()->sceneFactory();


     {
         JSONGetter getter("stereo.json");
         getter.visit(*scene, "scene");
     }
     {
         JSONSetter setter("stereo-secondary.json");
         setter.visit(*scene, "scene");
     }
}

int main (int argc, char ** argv)
{
    printf("Generate some test scenes\n");

    int size = 3;
    bool custom     = false;
    bool distorting = false;
    if (argc == 2 || argc == 3) {
        size = std::stoi(argv[1]);
        custom = true;
        SYNC_PRINT(("We will create scene of custom size %d\n", size));
        if (argc == 3)
        {
            if (string("true") == string(argv[2]))
            {
                distorting = true;
                SYNC_PRINT(("We will create scene with distortion\n"));
            }
        }
    }

//    testJSON_FixtureScene();
    testJSON_StereoScene(size, distorting);

    if (!custom) {
        testJSON_StereoRecheck();
    }

	return 0;
}
