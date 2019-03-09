
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/geometry/renderer/simpleRenderer.h"

#ifdef WITH_AVCODEC
#include "avEncoder.h"

#include <core/cameracalibration/projection/pinholeCameraIntrinsics.h>

#include <core/buffers/bufferFactory.h>
#endif


using namespace corecvs;



int main(int argc, char **argv)
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

