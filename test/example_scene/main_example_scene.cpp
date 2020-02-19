#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <QtXml/QDomDocument>
#include <vector>
#include <fstream>

#include "core/reflection/commandLineSetter.h"
#include "core/utils/utils.h"

#include "core/buffers/rgb24/abstractPainter.h"
#include "core/buffers/bufferFactory.h"

#include "core/fileformats/bmpLoader.h"

#include "core/camerafixture/fixtureScene.h"
#include "core/camerafixture/cameraFixture.h"
#include "core/cameracalibration/ilFormat.h"

#include "core/math/vector/vector3d.h"
#include "visitors/xmlSetter.h"
#include "visitors/xmlGetter.h"

#include "visitors/jsonGetter.h"
#include "visitors/jsonSetter.h"

#include "core/reflection/jsonPrinter.h"
#include "core/geometry/mesh/mesh3d.h"

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

        model.getPinhole()->setCx(100);
        model.getPinhole()->setCy(100);

        model.getPinhole()->setFx(100);
        model.getPinhole()->setFy(100);

        model.getPinhole()->setSize(Vector2dd(200, 200));
        model.getPinhole()->setDistortedSize(Vector2dd(200, 200));

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

    cout << "Original scene:" << std::endl;
    cout << "================================" << std::endl;
    scene->dumpInfo(cout);
    cout << "================================" << std::endl;

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

    cout << "Loaded scene:" << std::endl;
    cout << "================================" << std::endl;
    loaded->dumpInfo(cout);
    cout << "================================" << std::endl;
}

void testJSON_StereoScene(int targetSize = 3, bool useDistortion = false )
{
    cout << "----------------Running the test-------------" << std::endl;
    if (useDistortion) {
        cout << "Distortion would be applied" << std::endl;
    }
    FixtureScene *scene = new FixtureScene();

    CameraFixture *fixture = scene->createCameraFixture();
    fixture->name = "Z";


    CameraModel model;

    model.getPinhole()->setCx(320);
    model.getPinhole()->setCy(240);

    model.getPinhole()->setFx(589);
    model.getPinhole()->setFy(589);

    model.getPinhole()->setSize(Vector2dd(640, 480));
    model.getPinhole()->setDistortedSize(Vector2dd(640, 480));

    model.distortion.mKoeff = std::vector<double>({std::numeric_limits<double>::min()});

    if (useDistortion)
    {
        model.distortion.setPrincipalPoint(Vector2dd(400, 150));
        model.distortion.setShift(Vector2dd(6, 8));

        model.distortion.setTangentialX(0.01);
        model.distortion.setTangentialY(0.01);
        model.distortion.mKoeff.push_back(0.5);
        model.distortion.setNormalizingFocal(model.getPinhole()->size().l2Metric());
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

    RGB24Buffer *image1 = new RGB24Buffer(model.intrinsics->h(), model.intrinsics->w(), RGBColor::gray(39));
    RGB24Buffer *image2 = new RGB24Buffer(model.intrinsics->h(), model.intrinsics->w(), RGBColor::gray(56));
    RGB24Buffer *image3 = new RGB24Buffer(model.intrinsics->h(), model.intrinsics->w(), RGBColor::gray(75));

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
        FixedPointRemapper displacer(model.distortion, model.intrinsics->h(), model.intrinsics->w());
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
    geometry->color   = RGBColor::Red();
    geometry->frame   = PlaneFrame(Vector3dd(100,0,0), Vector3dd(0,1,0), Vector3dd(0,0,1));
    geometry->polygon = Polygon::RegularPolygon(5, Vector2dd::Zero(), 50, 0);
    geometry->relatedPoints.push_back(scene->featurePoints()[0]);
    geometry->relatedPoints.push_back(scene->featurePoints()[1]);
    geometry->relatedPoints.push_back(scene->featurePoints()[2]);

    /** Two additional cameras with non pinhole models */

    /** Equidistant **/
    CameraModel equidist;
    equidist.intrinsics.reset(new EquidistantProjection(Vector2dd(100,100), 100, Vector2dd(200,200)));
    equidist.setLocation(Affine3DQ::Shift(0,30,0));

    FixtureCamera *cameraEq = scene->createCamera();
    cameraEq->nameId = "equidist";
    cameraEq->copyModelFrom(equidist);

    /** Omnidirectional **/
    CameraModel catadioptric;
    catadioptric.intrinsics.reset(new OmnidirectionalProjection(Vector2dd(100,100), 100, Vector2dd(200,200)));
    catadioptric.setLocation(Affine3DQ::Shift(0,40,0));

    FixtureCamera *cameraCat = scene->createCamera();
    cameraCat->nameId = "catadioptric";
    cameraCat->copyModelFrom(catadioptric);



    cout << "Original scene:" << std::endl;
    cout << "================================" << std::endl;
    scene->dumpInfo(cout);
    cout << "================================" << std::endl;

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


void testJSON_CarScene(CommandLineSetter &params)
{
    bool addNoise = params.getBool("noise");

    FixtureScene *scene = new FixtureScene();

    CameraFixture *fixture = scene->createCameraFixture();
    fixture->name = "Car Body";


    Mesh3D mesh;
    mesh.addIcoSphere(Vector3dd::Zero(), 20.0, 1);

    for (size_t i = 0; i < mesh.vertexes.size(); i++)
    {
        Vector3dd pos = mesh.vertexes[i];
        if (pos.z() < 0) {
            continue;
        }

        SceneFeaturePoint *point = scene->createFeaturePoint();
        point->setPosition(pos);
        point->color = RGBColor::Yellow();
    }

    std::string input =
    "omnidirectional\n"
    "1578 1.35292 1.12018 5 0.520776 -0.561115 -0.560149 1.01397 -0.870155";
    std::istringstream ss(input);

    CameraModel model;
    model.intrinsics.reset(ILFormat::loadIntrisics(ss));

    FixtureCamera *front = scene->createCamera(); scene->addCameraToFixture(front, fixture);
    front->nameId = "Front";
    FixtureCamera *right = scene->createCamera(); scene->addCameraToFixture(right, fixture);
    right->nameId = "Right";
    FixtureCamera *back  = scene->createCamera(); scene->addCameraToFixture(back , fixture);
    back->nameId = "Back";
    FixtureCamera *left  = scene->createCamera(); scene->addCameraToFixture(left , fixture);
    left->nameId = "Left";

    FixtureCamera *cams[] = {front, right, back, left};

    front->copyModelFrom(model);
    right->copyModelFrom(model);
    back ->copyModelFrom(model);
    left ->copyModelFrom(model);

    scene->positionCameraInFixture(fixture, front, Affine3DQ(Quaternion::RotationZ(degToRad(  0)), Vector3dd( 1, 0, 0)));
    scene->positionCameraInFixture(fixture, right, Affine3DQ(Quaternion::RotationZ(degToRad( 90)), Vector3dd( 0, 1, 0)));
    scene->positionCameraInFixture(fixture,  left, Affine3DQ(Quaternion::RotationZ(degToRad(180)), Vector3dd(-3, 0, 0)));
    scene->positionCameraInFixture(fixture,  back, Affine3DQ(Quaternion::RotationZ(degToRad(270)), Vector3dd( 0,-1, 0)));


    scene->projectForward(SceneFeaturePoint::POINT_ALL, addNoise);


    for (int i = 0; i < 4; i++)
    {
        RGB24Buffer *image = new RGB24Buffer(model.intrinsics->h(), model.intrinsics->w(), RGBColor::gray(75));

        image->checkerBoard(100, RGBColor::Gray(50));

        AbstractPainter<RGB24Buffer> painter(image);

        for (size_t pId = 0; pId < scene->featurePoints().size(); pId++)
        {
            SceneFeaturePoint *point = scene->featurePoints()[pId];

            if (point->getObservation(cams[i]) != NULL) {
                Vector2dd p = point->getObservation(cams[i])->getUndist();
                painter.drawCircle(p.x(), p.y(), 30, point->color);

                SYNC_PRINT(("Drawing point at %lf %lf\n", p.x(), p.y()));
            }
        }
        std::string name = std::string("SP") /*+  fixture->name*/ + cams[i]->nameId + ".bmp";
        BufferFactory::getInstance()->saveRGB24Bitmap(image, name);

        ImageRelatedData *sceneImage = scene->createImageData();
        sceneImage->mImagePath = name;
        cams[i]->addImageToCamera(sceneImage);

        delete_safe(image);
    }

    cout << "Car scene:" << std::endl;
    cout << "================================" << std::endl;
    scene->dumpInfo(cout);
    cout << "================================" << std::endl;

    {
        JSONSetter setter("car.json");
        setter.visit(*scene, "scene");
    }


    delete_safe(scene);
}

void testJSON_StereoRecheck()
{

}

void usage()
{
    SYNC_PRINT(("Example scene generator:\n"));
    SYNC_PRINT(("example_scene --help\n"));
    SYNC_PRINT(("       - print help\n"));
    SYNC_PRINT(("example_scene --car\n"));
    SYNC_PRINT(("       - create an example car scene\n"));
    SYNC_PRINT(("example_scene --resave  --file in.json [--format <format>] \n"));
    SYNC_PRINT(("       - loads a file to internal scene format and saves back.\n"
                "         this is used to check if legacy format is still readable\n"));

}

void doResave(CommandLineSetter &params)
{
    SYNC_PRINT(("Resaveing\n"));
    FixtureSceneFactory::getInstance()->print();


    std::string name = "stereo.json";
    name = params.getString("file", name);

    SYNC_PRINT(("Loading <%s>\n", name.c_str()));
    FixtureScene *scene = FixtureSceneFactory::getInstance()->sceneFactory();

    {
        JSONGetter getter(name);
        getter.visit(*scene, "scene");
    }


    std::string outname = corecvs::HelperUtils::getFullPathWithNewExt(name, "-secondary.json");
    SYNC_PRINT(("Saveing <%s>\n", outname.c_str()));

    {
        JSONSetter setter(outname);
        setter.visit(*scene, "scene");
    }
    SYNC_PRINT(("Done\n"));
}

int main (int argc, char ** argv)
{
    printf("Generate some test scenes\n");

    CommandLineSetter s(argc, argv);
    bool help = s.getBool("help");
    if (help || argc < 2) {
        usage();
        return 0;
    }

    bool resave = s.getBool("resave");
    if (resave) {
        doResave(s);
        return 0;
    }

    bool carScene = s.getBool("car");
    if (carScene) {
        testJSON_CarScene(s);
        return 0;
    }


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
