#include "gtest/gtest.h"

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


using namespace corecvs;


TEST(Physics, checkFrSky)
{
    int16_t channels1[FrSkyMultimodule::CHANNEL_LAST] = {-200, -100, -50, 0, 50, 100, 200, 300};
    int16_t channels2[FrSkyMultimodule::CHANNEL_LAST] = {-200, -100, -50, 0, 50, 100, 200, 300};

    uint8_t message1[FrSkyMultimodule::MESSAGE_SIZE];
    uint8_t message2[FrSkyMultimodule::MESSAGE_SIZE];

    FrSkyMultimodule::pack (channels1, message1);

    for (int i = 0; i < FrSkyMultimodule::CHANNEL_LAST; i++) {
        channels2[i] = (channels1[i] - FrSkyMultimodule::MAGIC_DIFFERENCE) * 8 / 10;
    }

    FrSkyMultimodule::pack1(channels2, message2);

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

    for (int i = 0; i < 50; i++)
    {
        buffer.checkerBoard(10, RGBColor::Gray(70));
        /* Create 3d scene */
        Mesh3DDecorated mesh;
        mesh.switchColor(true);
        mesh.currentTransform = Matrix44(rot.toMatrix(), pos);

        size_t faceColorStart = mesh.facesColor.size();
        mesh.addIcoSphere(Vector3dd::Zero(), 25, 1);
        size_t faceColorEnd   =  mesh.facesColor.size();
        for (size_t c = faceColorStart; c < faceColorEnd; c++)
        {
            mesh.facesColor[c] = RGBColor::rainbow(lerpLimit(0.0, 1.0, c, faceColorStart, faceColorEnd));
        }

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


