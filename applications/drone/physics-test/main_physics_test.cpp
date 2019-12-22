#include "gtest/gtest.h"

#include "core/utils/utils.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/geometry/renderer/simpleRenderer.h"
#include "core/buffers/bufferFactory.h"
#include "core/filesystem/folderScanner.h"


#ifdef WITH_AVCODEC
#include "avEncoder.h"

#include <core/cameracalibration/projection/pinholeCameraIntrinsics.h>

#include <core/buffers/bufferFactory.h>
#endif

#include "frSkyMultimodule.h"

#include <bitset>
#include <multimoduleController.h>

#include "core/fileformats/meshLoader.h"
#include "copter/quad.h"


using namespace corecvs;


/**
 *
 *   \f[ x = x_0 + v_0 * t + (a / 2.0) * t^2  \f]
 *
 **/
TEST(Physics, freeFall)
{
    SimObject object;

    object.mass = 1.4352; /* More or less random mass */
    object.position = Vector3dd(0.0, 0.0, 45.0);

    double  t = 0.0;
    double dt = 0.001;

    Force gForce(0.0, 0.0, -9.8 * object.mass);

    for (int i = 0; i < 4000; i++)
    {

        object.startTick();
        object.addForce(gForce);
        object.tick(dt);
        t += dt;
        if (object.position.z() <= 0.0) {
            break;
        }
    }

    double expected = sqrt(2 * (45 / 9.8));

    SYNC_PRINT(("t collision %lfs expected (%lf)\n", t, expected));
    CORE_ASSERT_DOUBLE_EQUAL_E(t, expected, 1e-3, "Wrong fall time");

}

/* The Pit and The Pendulum */
/**
 *   Version 1
 *
 *   We have mass at zero with a hang point at 0,0,10
 *   We simulate it here by adding a force that point to hang point
 *
 **/
TEST(Physics, pendulum1)
{
    SimObject object;

    object.mass = 1.0;
    object.position = Vector3dd( 0.0, 0.0, -300.0);
    object.velocity = Vector3dd(10.5, 0.0,    0.0);

    double  t = 0.0;
    double dt = 0.001;

    Force gForce(0.0, 0.0, -9.8 * object.mass);

    Vector3dd supportPos(0.0, 0.0, 0.0);

    RGB24Buffer image(800, 800);
    Matrix33 toDraw = Matrix33::ShiftProj(200, 350);

    int LIMIT = 50000;

    for (int i = 0; i < LIMIT; i++)
    {
        Vector3dd arm = supportPos - object.position;
        Vector3dd dir = arm.normalised();

        Force supportF(-dir * (gForce.force & dir));

        /* We need this beacause otherwize error in tangential force would always move pendalum downwards */
        Force elastic;
        elastic.force = dir * (arm.l2Metric() - 300.0);

        object.startTick();
        object.addForce(gForce);
        object.addForce(supportF);
        object.addForce(elastic);
        object.tick(dt);
        t += dt;

        SYNC_PRINT(("pos (%lf %lf %lf)\n", object.position.x(), object.position.y(), object.position.z()));

        double x =  object.position.x() + 200;
        double y = -object.position.z();

        image.drawPixel(x, y, RGBColor::parula(i / 4000.0));

        /* Speed vs coord graph */
        image.drawPixel((double)i / LIMIT * 800, object.position.x() * 2 + 600, RGBColor::Yellow());
        image.drawPixel((double)i / LIMIT * 800, object.velocity.x() * 2 + 600, RGBColor::Red());


    }

    double expected = sqrt(2.0 * (45 / 9.8));

    SYNC_PRINT(("t collision %lfs expected (%lf)\n", t, expected));
    //CORE_ASSERT_DOUBLE_EQUAL_E(t, expected, 1e-3, "Wrong fall time");

    BufferFactory::getInstance()->saveRGB24Bitmap(&image, "pendulum1.bmp");
}

/**
 *   Version 2
 *
 *   We have mass at zero with a hang point at 0,0,10
 *   We simulate it here by adding non-central force
 *
 **/

#if 0
TEST(Physics, pendulum2)
{
    SimObject object;

    object.mass = 1.4352;
    object.position = Vector3dd(0.0, 0.0, 45.0);

    double  t = 0.0;
    double dt = 0.001;

    Force gForce(0.0, 0.0, -9.8 * object.mass);

    Vector3dd supportPos(0.0, 0.0, 10.0);

    for (int i = 0; i < 4000; i++)
    {
        Force supportF(suppo );

        object.startTick();
        object.addForce(gForce);
        object.tick(dt);
        t += dt;
        if (object.position.z() <= 0.0) {
            break;
        }
    }

    double expected = sqrt(2 * (45 / 9.8));

    SYNC_PRINT(("t collision %lfs expected (%lf)\n", t, expected));
    CORE_ASSERT_DOUBLE_EQUAL_E(t, expected, 1e-3, "Wrong fall time");
}
#endif


TEST(Physics, dzhanibekovEffect)
{

}

TEST(Quad, drawQuad)
{
    Quad quad;
    Mesh3D mesh;
    mesh.switchColor();
    quad.drawMyself(mesh);
    mesh.dumpPLY("copter.ply");
}

#if 0
TEST(Physics, checkFrSky)
{
    CopterInputs channels1 = {-200, -100, -50, 0, 50, 100, 200, 300};
    CopterInputs channels2 = {-200, -100, -50, 0, 50, 100, 200, 300};

    uint8_t message1[FrSkyMultimodule::MESSAGE_SIZE];
    uint8_t message2[FrSkyMultimodule::MESSAGE_SIZE];

    FrSkyMultimodule().pack (channels1, message1);

    for (int i = 0; i < CopterInputs::CHANNEL_LAST; i++) {
        channels2.axis[i] = (channels1.axis[i] - FrSkyMultimodule::MAGIC_DIFFERENCE) * 8 / 10;
    }

    FrSkyMultimodule().pack1(channels2, message2);

    for (int i = 0; i < FrSkyMultimodule::MESSAGE_SIZE; i++) {
        printf("%02d ", i);
    }
    printf("\n");
    printf("Reference:\n");
    for (int i = 0; i < FrSkyMultimodule::MESSAGE_SIZE; i++) {
        printf("%02x ", message1[i]);
    }
    printf("\n");
    printf("New version:\n");
    for (int i = 0; i < FrSkyMultimodule::MESSAGE_SIZE; i++) {
        printf("%02x ", message2[i]);
    }
    printf("\n");

    printf("\n");
    for (int i = 0; i < 8; i++) {
        std::bitset<8> x(message1[i]);
        cout << x << " ";
    }
    printf("\n");
    for (int i = 0; i < 8; i++) {
        std::bitset<8> x(message2[i]);
        cout << x << " ";
    }
    printf("\n");
}
#endif

TEST(Physics, testMultimodule)
{
    MultimoduleController controller;
    controller.connectToModule("/dev/ttyUSB0");
    SYNC_PRINT(("Waiting a bit for data sending\n"));
    sleep(4);
    SYNC_PRINT(("Stoping communication\n"));
    controller.disconnect();
}

TEST(Physics, dzhanibekovVideo)
{
    /** Following this guide                        **/
    /** 	            **/

    /* Let's begin */
    Vector3dd pos;  /**< Position */
    Vector3dd v;    /**< Speed */

    /* Rotation */
    Quaternion q;    /**< Rodrigues-Hamilton parameters aka Quaternion */
    Vector3dd  w;    /**< Angular velocity. For presentation look into the paper */

    Matrix33 I;      /**< Inertial tensor */

    int videoLen = 10000;
    int ticks = 10;

    pos = Vector3dd(-100, 0, 100 );
    v   = Vector3dd( 200.0 / (videoLen * ticks), 0, 0 );

    q = Quaternion::Identity();
    w = Vector3dd(0.99,0.01,0) * 0.0008;
    double mw = w.l2Metric();

    I = Matrix33::FromDiagonal(1.0, 1.5, 0.75);

    Mesh3D obj;
    obj.addAOB(AxisAlignedBox3d::ByCenter(Vector3dd::Zero(), Vector3dd(5, 20,10) * 2.0));
    obj.addAOB(AxisAlignedBox3d::ByCenter(Vector3dd(7.0,0,0) , Vector3dd(15, 4, 4) * 2.0));

    obj.switchColor(true);
    for (size_t c = 0; c < obj.faces.size(); c++) {
        obj.facesColor[c] = RGBColor::rainbow(lerpLimit(0.0, 1.0, c, 0, obj.faces.size()));
    }

    /** AVI Preparation **/
#ifdef WITH_AVCODEC
    AVEncoder encoder;
    RGB24Buffer buffer(500, 500);
    AVEncoder::printCaps();
    PinholeCameraIntrinsics cam(Vector2dd(buffer.w, buffer.h), degToRad(50));
    Affine3DQ camPosition = Affine3DQ::Shift(0, 0, -100);
    ClassicRenderer renderer;
    renderer.modelviewMatrix = cam.getKMatrix() * Matrix44(camPosition.inverted());
    renderer.drawFaces = true;
    encoder.startEncoding("out-dzh.avi", buffer.h, buffer.w * 2);

    RGB24Buffer graph(500, 500, RGBColor::White());
#endif

    for (int i = 0; i < videoLen; i++)
    {
        buffer.checkerBoard(10, RGBColor::Gray(70));
        /* Create 3d scene */
        Mesh3DDecorated mesh;
        mesh.switchColor(true);
        mesh.currentTransform = Matrix44(q.toMatrix(), pos);
        mesh.add(obj, true);

        /** Physics **/
        for (int j = 0; j < ticks; j++ )
        {
            pos += v;
            Quaternion dq = Quaternion::Rotation(w, w.l2Metric());
            q = q ^ dq;
            if (q.hasNans()) {
                cout << w << endl;
                return;
            }

#if 1
            Matrix33 omega = Matrix33::CrossProductLeft(w);
            Vector3dd dw = -I.inv() * ( omega * I * w );
            w += dw;
            w *= mw / w.l2Metric();
#else
            double iy = I.a(1,1) / I.a(0,0);
            double iz = I.a(2,2) / I.a(0,0);

            Vector3dd dw = Vector3dd(
              (iy - iz)      * w.y() * w.z(),
              (iz -  1) / iy * w.x() * w.z(),
              (1  - iy) / iz * w.x() * w.y()
            );
            w += dw;
#endif
            q.normalise();
        }

#ifdef WITH_AVCODEC
        renderer.render(&mesh, &buffer);
        RGB24Buffer output(buffer.h, buffer.w * 2);
        output.fillWith(buffer, 0, 0);

        Vector3dd ort = q * Vector3dd::OrtX() * 100;

        //cout << "Res" << (ort.x() + 750) << " " << (i/2) << endl;
        double x = (double) i / videoLen * graph.w;

        graph.setElement(ort.x() + graph.h / 2, x, RGBColor::Red());
        graph.setElement(ort.y() + graph.h / 2, x, RGBColor::Green());
        graph.setElement(ort.z() + graph.h / 2, x, RGBColor::Blue());
        output.fillWith(graph, 0, 500);
        cout << "Q:\n";
        q.printAxisAndAngle();
        cout << " x: " << pos << endl;

        cout << "Kinetic Energy:" << ((I * w) & w) << endl;

        encoder.addFrame(&output);
#endif


    }

#ifdef WITH_AVCODEC
    encoder.endEncoding();
#endif

}



TEST(Physics, dummyVideo)
{
#ifdef WITH_AVCODEC
    AVEncoder encoder;
    RGB24Buffer buffer(200,200);

    AVEncoder::printCaps();


    PinholeCameraIntrinsics cam(Vector2dd(buffer.w,buffer.h), degToRad(50));
    Affine3DQ camPosition = Affine3DQ::Shift(0,0,-100);

    ClassicRenderer renderer;
    renderer.modelviewMatrix = cam.getKMatrix() * Matrix44(camPosition.inverted());
    renderer.drawFaces = true;

    encoder.startEncoding("out.avi", buffer.h, buffer.w);

    Vector3dd pos  = Vector3dd(0, -20, 0);
    Quaternion rot = Quaternion::Identity();

    Mesh3D baseModel;
    std::string modelFile = "model.stl";
    if (!FolderScanner::pathExists(modelFile))
    {
        baseModel.switchColor(true);

        size_t faceColorStart = baseModel.facesColor.size();
        baseModel.addIcoSphere(Vector3dd::Zero(), 25, 1);
        size_t faceColorEnd   =  baseModel.facesColor.size();

        for (size_t c = faceColorStart; c < faceColorEnd; c++)
        {
            baseModel.facesColor[c] = RGBColor::rainbow(lerpLimit(0.0, 1.0, c, faceColorStart, faceColorEnd));
        }
    } else {
        MeshLoader().load(&baseModel, modelFile);
    }

    for (int i = 0; i < 50; i++)
    {
        buffer.checkerBoard(10, RGBColor::Gray(70));
        /* Create 3d scene */
        Mesh3DDecorated mesh;
        mesh.switchColor(true);
        mesh.currentTransform = Matrix44(rot.toMatrix(), pos);
        mesh.add(baseModel, true);
        renderer.render(&mesh, &buffer);
        encoder.addFrame(&buffer);

        BufferFactory::getInstance()->saveRGB24Bitmap(&buffer, "avi.bmp");

        pos += Vector3dd(0, 1, 0);
        rot = rot ^ Quaternion::Rotation(Vector3dd(1,1,1), degToRad(5));
    }
    encoder.endEncoding();
#endif

    SYNC_PRINT(("Done."));
}

TEST(Physics, dzhanibekovVideo1)
{
    /** Following this guide                        **/
    /** https://habr.com/ru/post/264381/            **/

    /* Let's begin */
    Vector3dd pos;  /**< Position */
    Vector3dd v;    /**< Speed */

    /* Rotation */
    Quaternion q;    /**< Rodrigues-Hamilton parameters aka Quaternion */
    Vector3dd  w;    /**< Angular velocity. For presentation look into the paper */

    Matrix33 I;      /**< Inertial tensor */

    int videoLen = 10000;
    int ticks = 10;

    pos = Vector3dd(-100, 0, 100 );
    v   = Vector3dd( 200.0 / (videoLen * ticks), 0, 0 );

    q = Quaternion::Identity();
    w = Vector3dd(0.99,0.01,0) * 0.0008;
    double mw = w.l2Metric();

    I = Matrix33::FromDiagonal(1.0, 1.5, 0.75);

    Mesh3D obj;
    obj.addAOB(AxisAlignedBox3d::ByCenter(Vector3dd::Zero(), Vector3dd(5, 20,10) * 2.0));
    obj.addAOB(AxisAlignedBox3d::ByCenter(Vector3dd(7.0,0,0) , Vector3dd(15, 4, 4) * 2.0));

    obj.switchColor(true);
    for (size_t c = 0; c < obj.faces.size(); c++) {
        obj.facesColor[c] = RGBColor::rainbow(lerpLimit(0.0, 1.0, c, 0, obj.faces.size()));
    }

    RGB24Buffer buffer(500, 500);
    /** AVI Preparation **/
#ifdef WITH_AVCODEC
    AVEncoder encoder;
    AVEncoder::printCaps();
    PinholeCameraIntrinsics cam(Vector2dd(buffer.w, buffer.h), degToRad(50));
    Affine3DQ camPosition = Affine3DQ::Shift(0, 0, -100);
    ClassicRenderer renderer;
    renderer.modelviewMatrix = cam.getKMatrix() * Matrix44(camPosition.inverted());
    renderer.drawFaces = true;
    encoder.startEncoding("out-dzh.avi", buffer.h, buffer.w * 2);

    RGB24Buffer graph(500, 500, RGBColor::White());
#endif


    for (int i = 0; i < videoLen; i++)
    {
        buffer.checkerBoard(10, RGBColor::Gray(70));
        /* Create 3d scene */
        Mesh3DDecorated mesh;
        mesh.switchColor(true);
        mesh.currentTransform = Matrix44(q.toMatrix(), pos);
        mesh.add(obj, true);

        /** Physics **/
        for (int j = 0; j < ticks; j++ )
        {
            pos += v;
            Quaternion dq = Quaternion::Rotation(w, w.l2Metric());
            q = q ^ dq;
            if (q.hasNans()) {
                cout << w << endl;
                return;
            }

#if 1
            Matrix33 omega = Matrix33::CrossProductLeft(w);
            Vector3dd dw = -I.inv() * ( omega * I * w );
            w += dw;
            w *= mw / w.l2Metric();
#else
            double iy = I.a(1,1) / I.a(0,0);
            double iz = I.a(2,2) / I.a(0,0);

            Vector3dd dw = Vector3dd(
              (iy - iz)      * w.y() * w.z(),
              (iz -  1) / iy * w.x() * w.z(),
              (1  - iy) / iz * w.x() * w.y()
            );
            w += dw;
#endif
            q.normalise();
        }

#ifdef WITH_AVCODEC
        renderer.render(&mesh, &buffer);
        RGB24Buffer output(buffer.h, buffer.w * 2);
        output.fillWith(buffer, 0, 0);

        Vector3dd ort = q * Vector3dd::OrtX() * 100;

        //cout << "Res" << (ort.x() + 750) << " " << (i/2) << endl;
        double x = (double) i / videoLen * graph.w;

        graph.setElement(ort.x() + graph.h / 2, x, RGBColor::Red());
        graph.setElement(ort.y() + graph.h / 2, x, RGBColor::Green());
        graph.setElement(ort.z() + graph.h / 2, x, RGBColor::Blue());
        output.fillWith(graph, 0, 500);
        cout << "Q:\n";
        q.printAxisAndAngle();
        cout << " x: " << pos << endl;

        cout << "Kinetic Energy:" << ((I * w) & w) << endl;

        encoder.addFrame(&output);
#endif


    }

#ifdef WITH_AVCODEC
    encoder.endEncoding();
#endif

}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


