#include "gtest/gtest.h"

#include "core/utils/utils.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/geometry/renderer/simpleRenderer.h"



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
 /*       if (object.position.z() <= 0.0) {
            break;
        }*/
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
    if (!HelperUtils::pathExists(modelFile))
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



int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


