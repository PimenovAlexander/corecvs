#include <stdio.h>
#include <QtCore/QCoreApplication>
//#include <unistd.h>

#include <QImage>

#include "qtFileLoader.h"

#include "imageCaptureInterface.h"
//#include "V4L2Capture.h"
#include "core/fileformats/bmpLoader.h"
#include "core/geometry/ellipticalApproximation.h"
#include "core/buffers/rgb24/rgb24Buffer.h"

#if 0
void func (void)
{
    Vector3dd mean(0.0);

    for (int i = 0; i < mImage->h; i++)
    {
        for (int j = 0; j < mImage->w; j++)
        {
            RGBColor &color = mImage->element(i,j);
            mean += Vector3dd(color.r(), color.g(), color.b());
        }
    }
    mean /= (mImage->h * mImage->w);

    EllipticalApproximationUnified<Vector3dd> ellip;
    for (int i = 0; i < mImage->h; i++)
    {
        for (int j = 0; j < mImage->w; j++)
        {
            RGBColor &color = mImage->element(i,j);
            ellip.addPoint(Vector3dd(color.r(), color.g(), color.b()) - mean);
        }
    }
    ellip.getEllipseParameters();
}
#endif


RGB24Buffer * alphaBlend(RGB24Buffer *in1, RGB24Buffer *in2, G8Buffer *alpha)
{

    RGB24Buffer *result = new RGB24Buffer(in1->getSize());

    for (int i = 0; i < result->h; i++)
    {
        for (int j = 0; j < result->w; j++)
        {
            RGBColor maskEl   = in1->element(i,j);
            RGBColor faceEl   = in2->element(i,j);
            int a = alpha->element(i,j);
            int b = 255 - a;

            int r1 = maskEl.r();
            int g1 = maskEl.g();
            int b1 = maskEl.b();

            int r2 = faceEl.r();
            int g2 = faceEl.g();
            int b2 = faceEl.b();

            RGBColor resultEl( (r1 * a + r2 * b) / 255, (g1 * a + g2 * b) / 255, (b1 * a + b2 * b) / 255 );
            result->element(i,j) = resultEl;
        }
    }
    return result;
}


int main (int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	printf("Loading mask...\n");
	QTFileLoader::registerMyself();

	QImage imageMask    ("data/adopt/orig.png");
	QImage imageAlpha   ("data/adopt/alpha.bmp");
    QImage imageFace    ("data/adopt/face.png");

    RGB24Buffer *alpha24 = QTFileLoader::RGB24BufferFromQImage(&imageAlpha);
    RGB24Buffer *mask    = QTFileLoader::RGB24BufferFromQImage(&imageMask);
    RGB24Buffer *face    = QTFileLoader::RGB24BufferFromQImage(&imageFace);

    G8Buffer *alpha = alpha24->getChannel(RGB24Buffer::CHANNEL_GRAY);

    Vector3dd meanMask(0.0);
    Vector3dd meanFace(0.0);
    double count = 0;

	/* Get hole statistics */
    for (int i = 0; i < mask->h; i++)
    {
        for (int j = 0; j < mask->w; j++)
        {
            if (alpha->element(i,j) > 10)
                continue;

            count++;
            meanFace += face->element(i,j).toDouble();
            meanMask += mask->element(i,j).toDouble();
        }
    }

    meanFace /= count;
    meanMask /= count;

    cout << "Mean face value is" << meanFace << endl;
    cout << "Mean face value is" << meanMask << endl;

    EllipticalApproximationUnified<Vector3dd> facePrincipal;
    EllipticalApproximationUnified<Vector3dd> maskPrincipal;

    for (int i = 0; i < mask->h; i++)
    {
       for (int j = 0; j < mask->w; j++)
       {
           facePrincipal.addPoint(face->element(i,j).toDouble() - meanFace);
           maskPrincipal.addPoint(mask->element(i,j).toDouble() - meanMask);
       }
    }
    facePrincipal.getEllipseParameters();
    maskPrincipal.getEllipseParameters();

    cout << "Face Principals" << endl;
    cout << facePrincipal.mAxes[0] << "->" << facePrincipal.mValues[0] << endl;
    cout << facePrincipal.mAxes[1] << "->" << facePrincipal.mValues[1] << endl;
    cout << facePrincipal.mAxes[2] << "->" << facePrincipal.mValues[2] << endl;

    cout << "Mask Principals" << endl;
    cout << maskPrincipal.mAxes[0] << "->" << maskPrincipal.mValues[0] << endl;
    cout << maskPrincipal.mAxes[1] << "->" << maskPrincipal.mValues[1] << endl;
    cout << maskPrincipal.mAxes[2] << "->" << maskPrincipal.mValues[2] << endl;

    Vector3dd scalers;
    scalers.x() = sqrt(maskPrincipal.mValues[0]) / sqrt(facePrincipal.mValues[0]);
    scalers.y() = sqrt(maskPrincipal.mValues[1]) / sqrt(facePrincipal.mValues[1]);
    scalers.z() = sqrt(maskPrincipal.mValues[2]) / sqrt(facePrincipal.mValues[2]);

    /* Making correction for face */
    RGB24Buffer *faceCorr = new RGB24Buffer(face->getSize(), false);
    for (int i = 0; i < faceCorr->h; i++)
    {
       for (int j = 0; j < faceCorr->w; j++)
       {
           Vector3dd color = face->element(i,j).toDouble() - meanFace;
           Vector3dd projected(color & facePrincipal.mAxes[0],
                      color & facePrincipal.mAxes[1],
                      color & facePrincipal.mAxes[2]);

           projected = projected * scalers;

           Vector3dd newColor =
                   maskPrincipal.mAxes[0] * projected.x() +
                   maskPrincipal.mAxes[1] * projected.y() +
                   maskPrincipal.mAxes[2] * projected.z() + meanMask;
           RGBColor newrgb;
           double c;
           c = newColor.x();
           if (c <   0) c =   0;
           if (c > 255) c = 255;
           newrgb.r() = c;
           c = newColor.y();
           if (c <   0) c =   0;
           if (c > 255) c = 255;
           newrgb.g() = c;
           c = newColor.z();
           if (c <   0) c =   0;
           if (c > 255) c = 255;
           newrgb.b() = c;

           faceCorr->element(i,j) = newrgb;
       }
    }

    /* With Matrix*/
    Matrix33 scalerM    = Matrix33::Scale3(scalers);
    Matrix33 toUnityM   = Matrix33::FromRows(facePrincipal.mAxes[0], facePrincipal.mAxes[1], facePrincipal.mAxes[2]);
    Matrix33 fromUnityM = Matrix33::FromColumns(maskPrincipal.mAxes[0], maskPrincipal.mAxes[1], maskPrincipal.mAxes[2]);

    Matrix33 transform = fromUnityM * scalerM * toUnityM;

    RGB24Buffer *faceCorr2 = new RGB24Buffer(face->getSize(), false);
    for (int i = 0; i < faceCorr2->h; i++)
    {
       for (int j = 0; j < faceCorr2->w; j++)
       {
           Vector3dd newColor = transform * (face->element(i,j).toDouble() - meanFace) + meanMask;
           RGBColor newrgb;
           double c;
           c = newColor.x();
           if (c <   0) c =   0;
           if (c > 255) c = 255;
           newrgb.r() = c;
           c = newColor.y();
           if (c <   0) c =   0;
           if (c > 255) c = 255;
           newrgb.g() = c;
           c = newColor.z();
           if (c <   0) c =   0;
           if (c > 255) c = 255;
           newrgb.b() = c;

           faceCorr2->element(i,j) = newrgb;
       }
    }


    /* Without roots */
    scalers.x() = maskPrincipal.mValues[0] / facePrincipal.mValues[0];
    scalers.y() = maskPrincipal.mValues[1] / facePrincipal.mValues[1];
    scalers.z() = maskPrincipal.mValues[2] / facePrincipal.mValues[2];


    /* Making correction for face */
    RGB24Buffer *faceCorr1 = new RGB24Buffer(face->getSize(), false);
    for (int i = 0; i < faceCorr1->h; i++)
    {
       for (int j = 0; j < faceCorr1->w; j++)
       {
           Vector3dd color = face->element(i,j).toDouble() - meanFace;
           Vector3dd projected(color & facePrincipal.mAxes[0],
                      color & facePrincipal.mAxes[1],
                      color & facePrincipal.mAxes[2]);

           projected = projected * scalers;

           Vector3dd newColor =
                   maskPrincipal.mAxes[0] * projected.x() +
                   maskPrincipal.mAxes[1] * projected.y() +
                   maskPrincipal.mAxes[2] * projected.z() + meanMask;
           RGBColor newrgb;
           double c;
           c = newColor.x();
           if (c <   0) c =   0;
           if (c > 255) c = 255;
           newrgb.r() = c;
           c = newColor.y();
           if (c <   0) c =   0;
           if (c > 255) c = 255;
           newrgb.g() = c;
           c = newColor.z();
           if (c <   0) c =   0;
           if (c > 255) c = 255;
           newrgb.b() = c;

           faceCorr1->element(i,j) = newrgb;
       }
    }



    /* Make a final blending */
    BMPLoader().save("output0.bmp", mask);


    RGB24Buffer *result = alphaBlend(mask, face, alpha);
	BMPLoader().save("output1.bmp", result);

	RGB24Buffer *result1 = alphaBlend(mask, faceCorr, alpha);
	BMPLoader().save("output2.bmp", result1);

	RGB24Buffer *result2 = alphaBlend(mask, faceCorr1, alpha);
    BMPLoader().save("output3.bmp", result2);

    RGB24Buffer *result3 = alphaBlend(mask, faceCorr2, alpha);
    BMPLoader().save("matrix-out.bmp", result3);


	delete_safe(alpha);
    delete_safe(mask);
    delete_safe(face);
    delete_safe(faceCorr);
    delete_safe(faceCorr1);
    delete_safe(faceCorr2);

    delete_safe(result);
    delete_safe(result1);
    delete_safe(result2);
    delete_safe(result3);
    return 0;
}
