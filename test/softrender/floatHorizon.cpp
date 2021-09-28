#include "floatHorizon.h"

#include "core/buffers/rgb24/abstractPainter.h"
#include "core/math/mathUtils.h"


using namespace corecvs;

FloatHorizon::FloatHorizon()
{

}

double f(double x, double y)
{
    double pscale = 4;
    x /= pscale;
    y /= pscale;

    double sigma2 = x * x + y * y;
    double scale = -1.0 / (M_PI * sigma2 * sigma2);
    double denum = 2 * sigma2;

    double rsc = sigma2 / denum;
   // return scale * (1 - rsc) * exp(rsc);
 //   return 0;
    return  cos (x / 2) * cos (y / 2) * 15;
}

void FloatHorizon::render0()
{

    if (output == NULL) {
        SYNC_PRINT(("Output buffer is null"));
        return;
    }

    Matrix33 rotation = Matrix33::RotationX(phi) * Matrix33::RotationZ(psi) ;
    Vector2dd center(output->w / 2, output->h / 2);

    for (int i = -20; i < 20; i++)
    {
        for (int j = -20; j < 20; j++) {
            Vector3dd point(i, j, f(i,j));
            Vector2dd prj = center + (rotation * point).xz() * 10;

            output->drawPixel(prj.x(), prj.y(), RGBColor::Red());
            //SYNC_PRINT(("%d %d - %lf %lf\n", i, j,  ));
        }
    }

}

void FloatHorizon::render1()
{

    if (output == NULL) {
        SYNC_PRINT(("Output buffer is null"));
        return;
    }

    Matrix33 rotation = Matrix33::RotationX(phi) * Matrix33::RotationZ(psi) ;
    Vector2dd center(output->w / 2, output->h / 2);

    for (int i = -20; i < 20; i++)
    {
        for (int j = -20; j < 20; j++) {
            Vector3dd point(i, j, f(i,j));

            Vector3dd rot = (rotation * point);
            Vector2dd prj = center + rot.xz() * 10;

            output->drawPixel(prj.x(), prj.y(), RGBColor::parula(point.z() / 15.0));
            //SYNC_PRINT(("%d %d - %lf %lf\n", i, j,  ));
        }
    }

}


void FloatHorizon::render2()
{

    if (output == NULL) {
        SYNC_PRINT(("Output buffer is null"));
        return;
    }

    Matrix33 rotation =  Matrix33::RotationZ(psi) * Matrix33::RotationX(phi);
    Vector2dd center(output->w / 2, output->h / 2);

    const int SIZE_X = 41;
    const int SIZE_Y = 41;

    const int CENTER_X = SIZE_X / 2;
    const int CENTER_Y = SIZE_Y / 2;


    AbstractBuffer<Vector3dd> buffer(SIZE_Y, SIZE_X);

    for (int i = 0; i < SIZE_Y; i++)
    {
        for (int j = 0; j < SIZE_X; j++) {


            Vector3dd point(i - CENTER_Y, j - CENTER_X, f(i,j));

            buffer.element(i, j) = (rotation * point);

        }
    }

    for (int i = 0; i < SIZE_Y - 1; i++)
    {
        for (int j = 0; j < SIZE_X - 1; j++)
        {

            Vector2dd prj0 = center + buffer.element(i    , j    ).xz() * 10;
            Vector2dd prjX = center + buffer.element(i    , j + 1).xz() * 10;
            Vector2dd prjY = center + buffer.element(i + 1, j    ).xz() * 10;
            output->drawLine(prj0, prjX, RGBColor::Red());
            output->drawLine(prj0, prjY, RGBColor::Green());
        }

        Vector2dd prj0 = center + buffer.element(i    , SIZE_X - 1 ).xz() * 10;
        Vector2dd prjY = center + buffer.element(i + 1, SIZE_X - 1 ).xz() * 10;
        output->drawLine(prj0, prjY, RGBColor::Green());
    }
    for (int j = 0; j < SIZE_X - 1; j++)
    {
        Vector2dd prj0 = center + buffer.element(SIZE_Y - 1, j    ).xz() * 10;
        Vector2dd prjX = center + buffer.element(SIZE_Y - 1, j + 1).xz() * 10;
        output->drawLine(prj0, prjX, RGBColor::Red());
    }

    Vector2dd prj0 = center + buffer.element(0, 0).xz() * 10;
    AbstractPainter<RGB24Buffer> p(output);

    p.drawCircle(prj0, 10.0, RGBColor::Blue());

}

void FloatHorizon::init()
{
    top   .resize(output->w, output->h);
    bottom.resize(output->w, 0);
}



void FloatHorizon::drawDebug()
{
    if (output == NULL) {
        SYNC_PRINT(("Output buffer is null"));
        return;
    }

    for (int j = 0; j < output->w; j++)
    {
        for(int i = top[j]; i <= bottom[j]; i++)
        {
            output->element(i, j) = RGBColor::FromFloat((RGBColor::White().toFloat() + output->element(i, j).toFloat()) / 2);
        }
    }

}





void FloatHorizon::render3()
{

    if (output == NULL) {
        SYNC_PRINT(("Output buffer is null"));
        return;
    }

    Matrix33 rotation =  Matrix33::RotationX(phi) * Matrix33::RotationZ(psi);
    Vector2dd center(output->w / 2, output->h / 2);

    const int SIZE_X = 41;
    const int SIZE_Y = 41;

    const int CENTER_X = SIZE_X / 2;
    const int CENTER_Y = SIZE_Y / 2;


    AbstractBuffer<Vector3dd> buffer(SIZE_Y, SIZE_X);

    for (int i = 0; i < SIZE_Y; i++)
    {
        for (int j = 0; j < SIZE_X; j++) {


            Vector3dd point(i - CENTER_Y, j - CENTER_X, f(i,j));
            buffer.element(i, j) = (rotation * point);

        }
    }


    int count = 0;

    DiagonalIterator d(SIZE_Y);
    for (auto p : d)
    {
        count++;
        if (stopAt > 0 && count > stopAt)
            break;

        vector<double> newTop;
        vector<double> newBottom;

        newTop   .resize(output->w, output->h);
        newBottom.resize(output->w, 0);

        for (int k = 0; k < output->w; k++)
        {
            newTop   [k] = top   [k];
            newBottom[k] = bottom[k];
        }


        int i = p.y();
        int j = p.x();


        Vector2dd prj0 = center + buffer.element(i    , j    ).xz() * 8;

        if (j < SIZE_X - 1) {
            Vector2dd prjX = center + buffer.element(i    , j + 1).xz() * 8;

            LineSpanIterator iterator0(prj0, prjX);
            for (auto point : iterator0)
            {
                if (point.x() < 0 || point.x() >= output->w)
                    continue;

                if (point.y() < top[point.x()] ) {
                    output->element(point) = RGBColor::parula((double)i/SIZE_Y);
                    newTop[point.x()] = point.y();
                }

                if (point.y() > bottom[point.x()] ) {
                    output->element(point) = RGBColor::Green()/*bright((double)i/SIZE_Y)*/;
                    newBottom[point.x()] = point.y();
                }
            }
        }

        if (i < SIZE_X - 1) {
            Vector2dd prjY = center + buffer.element(i + 1, j    ).xz() * 8;
            LineSpanIterator iterator1(prj0, prjY);
            for (auto point : iterator1)
            {
                if (point.x() < 0 || point.x() >= output->w)
                    continue;

                if (point.y() < top[point.x()] ) {
                    output->element(point) = RGBColor::parula((double)i/SIZE_Y);
                    newTop[point.x()] = point.y();
                }

                if (point.y() > bottom[point.x()] ) {
                    output->element(point) = RGBColor::Green()/*bright((double)i/SIZE_Y)*/;
                    newBottom[point.x()] = point.y();
                }
            }
        }


        for (int k = 0; k < output->w; k++)
        {
            top   [k] = newTop   [k];
            bottom[k] = newBottom[k];
        }

    }

    if (shouldHorison) {
        drawDebug();
    }


    Vector2dd prj0 = center + buffer.element(0, 0).xz() * 8;
    AbstractPainter<RGB24Buffer> p(output);
    p.drawCircle(prj0, 10.0, RGBColor::Blue());

}
