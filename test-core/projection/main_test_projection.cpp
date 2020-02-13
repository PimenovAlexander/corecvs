/**
 * \file main_test_projection.cpp
 * \brief This is the main file for the test projection 
 *
 * \date янв 20, 2018
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include "gtest/gtest.h"

#include "core/cameracalibration/projection/equidistantProjection.h"
#include "core/cameracalibration/projection/equisolidAngleProjection.h"
#include "core/cameracalibration/projection/omnidirectionalProjection.h"
#include "core/cameracalibration/ilFormat.h"


#include "core/cameracalibration/cameraModel.h"
#include "core/utils/global.h"



#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/geometry/mesh/mesh3d.h"
#include "core/cameracalibration/calibrationDrawHelpers.h"
#include "core/fileformats/bmpLoader.h"
#include "core/fileformats/objLoader.h"
#include "core/buffers/bufferFactory.h"


using namespace std;
using namespace corecvs;


TEST(projection, testEquidistant)
{
    EquidistantProjection projection;

    Vector2dd source(Vector2dd(1.0, 1.0));
    Vector3dd p = projection.reverse(source);
    Vector2dd rsource = projection.project(p);

    cout << "EquidistantProjection:" << endl
         << projection << endl;
    cout << " Source: " << source << endl;
    cout << " RayDir: " << p << endl;
    cout << "RSource: " << rsource << endl;

    ASSERT_TRUE(source.notTooFar(rsource, 1e-7));
}


TEST(projection, testEquisolid)
{
    EquisolidAngleProjection projection;

    Vector2dd source(Vector2dd(1.0, 1.0));
    Vector3dd p = projection.reverse(source);
    Vector2dd rsource = projection.project(p);

    cout << "EquisolidAngleProjection:" << endl
         << projection << endl;
    cout << " Source: " << source << endl;
    cout << " RayDir: " << p << endl;
    cout << "RSource: " << rsource << endl;

    ASSERT_TRUE(source.notTooFar(rsource, 1e-7));
}

TEST(projection, testOmnidirectional)
{
    OmnidirectionalProjection projection;

    Vector2dd source(Vector2dd(1.0, 1.0));
    Vector3dd p = projection.reverse(source);

    cout << "Will call project" << endl;
    Vector2dd rsource = projection.project(p);

    cout << "OmnidirectionalBaseParameters:" << endl
         << projection << endl;
    cout << " Source: " << source << endl;
    cout << " RayDir: " << p << endl;
    cout << "RSource: " << rsource << endl;

    ASSERT_TRUE(source.notTooFar(rsource, 1e-7));
}

TEST(projection, testOmnidirectional1)
{
    OmnidirectionalProjection projection;

    projection.mN[0] = 0.1;

    Vector2dd source(Vector2dd(1.0, 1.0));
    Vector3dd p = projection.reverse(source);

    cout << "Will call project" << endl;
    Vector2dd rsource = projection.project(p);

    cout << "OmnidirectionalBaseParameters:" << endl
         << projection << endl;
    cout << " Source: " << source << endl;
    cout << " RayDir: " << p << endl;
    cout << "RSource: " << rsource << endl;
}

TEST(projection, testOmnidirectional2)
{
    OmnidirectionalProjection projection;

    projection.mN[0] = 0.1;
    projection.mN[1] = 0.2;

    Vector2dd source(Vector2dd(1.0, 1.0));
    Vector3dd p = projection.reverse(source);

    cout << "Will call project" << endl;
    Vector2dd rsource = projection.project(p);

    cout << "OmnidirectionalBaseParameters:" << endl
         << projection << endl;
    cout << " Source: " << source << endl;
    cout << " RayDir: " << p << endl;
    cout << "RSource: " << rsource << endl;

    ASSERT_TRUE(source.notTooFar(rsource, 1e-7));
}

TEST(projection, testOmnidirectional3)
{
    OmnidirectionalProjection projection;

    projection.mN = vector<double>({ -3.97282, -7.61552, 26.471, -43.6205});

    Vector2dd source(Vector2dd(1.0, 1.0));
    Vector3dd p = projection.reverse(source);

    cout << "Will call project" << endl;
    Vector2dd rsource = projection.project(p);

    cout << "OmnidirectionalBaseParameters:" << endl
         << projection << endl;
    cout << " Source: " << source << endl;
    cout << " RayDir: " << p << endl;
    cout << "RSource: " << rsource << endl;

    ASSERT_TRUE(source.notTooFar(rsource, 1e-7));
}

TEST(projection, testFormatLoad)
{
    std::string input =
    "omnidirectional\n"
    "1578 1.35292 1.12018 5 0.520776 -0.561115 -0.560149 1.01397 -0.870155";
    std::istringstream ss(input);

    unique_ptr<CameraProjection> model;
    model.reset(ILFormat::loadIntrisics(ss));

    std::ostringstream so;
    so.imbue(std::locale("C"));
    so << std::setprecision(std::numeric_limits<double>::digits10 + 2);
    ILFormat::saveIntrisics(*(model.get()), so);
    cout << so.str();

    std::istringstream ss1(so.str());

    unique_ptr<CameraProjection>  model1;
    model1.reset(ILFormat::loadIntrisics(ss1));

    CORE_ASSERT_TRUE(model  != NULL, "Model is NULL");
    CORE_ASSERT_TRUE(model1 != NULL, "Model1 is NULL");


    CORE_ASSERT_TRUE(model ->projection == ProjectionType::OMNIDIRECTIONAL, "Failed to load");
    CORE_ASSERT_TRUE(model1->projection == ProjectionType::OMNIDIRECTIONAL, "Failed to resave");

    OmnidirectionalProjection *o1 = static_cast<OmnidirectionalProjection *>(model .get());
    OmnidirectionalProjection *o2 = static_cast<OmnidirectionalProjection *>(model1.get());

    cout << "Old:" << *o1 << endl;
    cout << "New:" << *o2 << endl;


    CORE_ASSERT_TRUE(o1->principal().notTooFar(o2->principal()), "Failed to resave pos");
    CORE_ASSERT_DOUBLE_EQUAL_E(o1->focal(), o2->focal(), 1e-7, "Failed to resave focal");

    CORE_ASSERT_TRUE(o1->mN.size() == o2->mN.size(), "Failed to load params");
    for (size_t i = 0; i < o1->mN.size(); i++)
    {
        CORE_ASSERT_DOUBLE_EQUAL_E(o1->mN[i], o2->mN[i], 1e-7, "Failed to load polynom");
    }

}


TEST(projection, testProjectionChange)
{

#if 0
    int h = 480;
    int w = 640;

    // RGB24Buffer *image = new RGB24Buffer(h,w);
    RGB24Buffer *image = BufferFactory::getInstance()->loadRGB24Bitmap("data/pair/image0001_c0.pgm");

    slowProjection.setSizeX(w);
    slowProjection.setSizeY(h);

    slowProjection.setPrincipalX(image->w / 2.0);
    slowProjection.setPrincipalY(image->h / 2.0);

    slowProjection.setFocal(image->w / 2.0);

    CameraModel model(slowProjection.clone());
#else
    std::string input =
    "omnidirectional\n"
    "1578 1.35292 1.12018 5 0.520776 -0.561115 -0.560149 1.01397 -0.870155";
    std::istringstream ss(input);

    CameraModel model;
    model.intrinsics.reset(ILFormat::loadIntrisics(ss));
    OmnidirectionalProjection slowProjection = * static_cast<OmnidirectionalProjection *>(model.intrinsics.get());

    RGB24Buffer *image = new RGB24Buffer(slowProjection.sizeY(), slowProjection.sizeX());
    image->checkerBoard(50, RGBColor::White());
#endif

    cout << "Input model:" << endl;
    cout << "=============================" << endl;
    cout << model << endl;
    cout << "=============================" << endl;
    cout << "Input image:" << endl;
    cout << image->getSize() << endl;
    cout << "=============================" << endl;

    Mesh3DDecorated mesh;
    mesh.switchColor(true);    
    mesh.switchTextures(true);
    mesh.setColor(RGBColor::Yellow());
    CalibrationDrawHelpers draw;

    Mesh3D toDraw;
    toDraw.addIcoSphere(Vector3dd(0, 0, 100.0), 70, 2);

    for (size_t i = 0; i < toDraw.vertexes.size(); i++)
    {
        Vector2dd prj = model.project(toDraw.vertexes[i]);
        Vector2d<int> prji(fround(prj.x()), fround(prj.y()));

        if (image->isValidCoord(prji)) {
            image->element(prji) = RGBColor::Red();
        }
    }


    draw.drawCameraEx(mesh, model, 5, 0);
    mesh.dumpPLY("catadioptric.ply");
    mesh.materials.push_back(OBJMaterial());
    mesh.materials.front().name = "image1";
    mesh.materials.front().tex[OBJMaterial::KOEF_AMBIENT] = new RGB24Buffer(image);


    OBJLoader loader;
    loader.saveMaterials("catadioptric.mtl", mesh.materials, "");
    loader.saveObj("catadioptric", mesh);

    /** Ok  Equidistant **/
    EquidistantProjection target;
    target.setPrincipalX(slowProjection.principalX());
    target.setPrincipalY(slowProjection.principalY());
    target.setDistortedSizeX(slowProjection.distortedSizeX());
    target.setDistortedSizeY(slowProjection.distortedSizeY());
    target.setSizeX(slowProjection.sizeX());
    target.setSizeY(slowProjection.sizeY());

    cout << "EquidistantProjection" << endl;
    cout << "Start Focal: " << slowProjection.focal() << endl;

    double minF = 0;
    double minFerr = std::numeric_limits<double>::max();

    for (double f = slowProjection.focal() / 4.0; f < slowProjection.focal()*2; f*=1.1 )
    {
        target.setFocal(f);

        double err = 0.0;
        for (int i = 0; i < image->h; i++)
        {
            for (int j = 0; j < image->w; j++)
            {
                Vector2dd pixel(i, j);
                Vector3dd dir = slowProjection.reverse(pixel);

                Vector2dd map = target.project(dir);

                double newErr = (map - pixel).l2Metric();
                if (err < newErr) {
                    err = newErr;
                }
            }
        }
        if (err < minFerr) {
            minFerr = err;
            minF = f;
        }

        cout << "F is: " << f << " Max Diff is: " << err << "px" << endl;
    }

#if 0
    /** Ok Equisolid **/
    EquisolidAngleProjection target1;
    target1.setPrincipalX(slowProjection.principalX());
    target1.setPrincipalY(slowProjection.principalY());
    target1.setDistortedSizeX(slowProjection.distortedSizeX());
    target1.setDistortedSizeY(slowProjection.distortedSizeY());
    target1.setSizeX(slowProjection.sizeX());
    target1.setSizeY(slowProjection.sizeY());

    cout << "EquisolidAngleProjection" << endl;
    cout << "Start Focal: " << slowProjection.focal() << endl;

    double minF1 = 0;
    double minFerr1 = std::numeric_limits<double>::max();

    for (double f = slowProjection.focal() / 4.0; f < slowProjection.focal()*2; f*=1.1 )
    {
        target1.setFocal(f);

        double err = 0.0;
        for (int i = 0; i < image->h; i++)
        {
            for (int j = 0; j < image->w; j++)
            {
                Vector2dd pixel(i, j);
                Vector3dd dir = slowProjection.reverse(pixel);

                Vector2dd map = target1.project(dir);

                double newErr = (map - pixel).l2Metric();
                if (err < newErr) {
                    err = newErr;
                }
            }
        }
        if (err < minFerr) {
            minFerr1 = err;
            minF1 = f;
        }


        cout << "F is: " << f << " Max Diff is: " << err << "px" << endl;
    }

    /** Stereographic **/
    StereographicProjection target2;
    target2.setPrincipalX(slowProjection.principalX());
    target2.setPrincipalY(slowProjection.principalY());
    target2.setDistortedSizeX(slowProjection.distortedSizeX());
    target2.setDistortedSizeY(slowProjection.distortedSizeY());
    target2.setSizeX(slowProjection.sizeX());
    target2.setSizeY(slowProjection.sizeY());

    cout << "Stereographic" << endl;
    cout << "Start Focal: " << slowProjection.focal() << endl;

    double minF2 = 0;
    double minFerr2 = std::numeric_limits<double>::max();

    for (double f = slowProjection.focal() / 16.0; f < slowProjection.focal()*2; f*=1.1 )
    {
        target2.setFocal(f);

        double err = 0.0;
        for (int i = 0; i < image->h; i++)
        {
            for (int j = 0; j < image->w; j++)
            {
                Vector2dd pixel(i, j);
                Vector3dd dir = slowProjection.reverse(pixel);

                Vector2dd map = target2.project(dir);

                double newErr = (map - pixel).l2Metric();
                if (err < newErr) {
                    err = newErr;
                }
            }
        }
        if (err < minFerr) {
            minFerr2 = err;
            minF2 = f;
        }


        cout << "F is: " << f << " Max Diff is: " << err << "px" << endl;
    }
#endif

    /* From the remap buffer */

    target.setFocal(minF);
    cout << "Using focal:" << minF << endl;

    AbstractBuffer<Vector2dd> *displacement = new AbstractBuffer<Vector2dd>(image->h, image->w);

    /**
     *
     *    2D    ->          catadioptric           -> 3D
     *    2D    <-  remap  <-  2D  <- equiditstant -> 3D
     *
     *
     **/
    for (int i = 0; i < image->h; i++)
    {
        for (int j = 0; j < image->w; j++)
        {
            Vector2dd pixel(j, i); /* This is a remap pixel */

            Vector3dd dir = target.reverse(pixel);
            Vector2dd map = slowProjection.project(dir);

            /* We need to imvent something */

            if (displacement->isValidCoord(i, j)) {
                displacement->element(i, j) = Vector2dd(map.x(), map.y());
            }
        }
    }

    RGB24Buffer *disp = new RGB24Buffer(displacement->getSize());
    disp->drawDoubleVecBuffer(displacement);

    BMPLoader().save("catad.bmp", image);
    BMPLoader().save("catad-disp.bmp", disp);


}
