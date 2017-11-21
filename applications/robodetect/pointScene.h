#pragma once

#include <vector>

#include "core/utils/global.h"

#include "scene3D.h"
#include "core/math/vector/vector3d.h"
#include "core/buffers/rgb24/rgbColor.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/geometry/ellipticalApproximation.h"


/**
 * \file pointScene.h
 *
 * \date Oct 13, 2013
 **/

using std::vector;

class PointScene : public Scene3D
{
public:
    class Point : public Vector3dd {
    public:
        Point(const Vector3dd &value) : Vector3dd(value) {}
        Point(const RGBColor &value) : Vector3dd(value.r(), value.g(), value.b()) {}

    };

    void showBuffer(RGB24Buffer *buffer)
    {
        scene.clear();
        scene.reserve(buffer->h * buffer->w);
        for (int i = 0; i < buffer->h; i++)
        {
            for (int j = 0; j < buffer->w; j++)
            {
                scene.push_back(Point(buffer->element(i,j) ));
            }
        }

    }


    vector<Point> scene;


    virtual void prepareMesh(CloudViewDialog *dialog);
    virtual void drawMyself(CloudViewDialog *dialog);
};


class EllApproxScene : public Scene3D
{
public:
    Vector3dd center;
    EllipticalApproximationUnified<Vector3dd> mEllip;


    EllApproxScene(Vector3dd mean, const EllipticalApproximationUnified<Vector3dd> &ellip) :
         center(mean)
       , mEllip(ellip)
    {
        mEllip.getEllipseParameters();
        printf("Input approximation %d size\n", ellip.mAxes.size());
        printf("Our   approximation %d size\n", mEllip.mAxes.size());
        printf("This: %p\n", this);

    }

    virtual void prepareMesh(CloudViewDialog *dialog);
    virtual void drawMyself(CloudViewDialog *dialog);

};

/* EOF */
