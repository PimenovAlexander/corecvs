/**
 * \file triangulator.cpp
 * \brief Add Comment Here
 *
 * \date Nov 17, 2011
 * \author alexander
 */

#include "triangulator.h"

namespace corecvs {

Triangulator::Triangulator(const RectificationResult &_rectifierData) :
    rectifierData(_rectifierData)
{
    rectifiedToCameraFrameLeft =
            rectifierData. leftCamera.getInvKMatrix33() * rectifierData. leftTransform.inv();

    rectifiedToCameraFrameRight =
            rectifierData.rightCamera.getInvKMatrix33() * rectifierData.rightTransform.inv();

}

Vector3dd Triangulator::triangulate(const Vector2dd &start, const Vector2dd &shift) const
{
    Vector2dd end = start + shift;

    Vector2dd startInCameraFrame = rectifiedToCameraFrameRight * start;
    Vector2dd endInCameraFrame   = rectifiedToCameraFrameLeft  * end;

    double d1;
    double d2;
    double err;

    Vector3dd start3d(startInCameraFrame, 1.0);
    Vector3dd end3d  (  endInCameraFrame, 1.0);

    Vector3dd point;

    if (0 /*Old style triangulation*/) {
        rectifierData.decomposition.getScaler(start3d, end3d, d1, d2, err);
        point = (start3d * d1) * rectifierData.baseline;
    } else {
        rectifierData.decomposition.getDistance(start3d, end3d, d1, d2, err);
        point = (start3d.normalised() * d1) * rectifierData.baseline;
    }
    return point;
}

Vector3dd Triangulator::triangulate(const Vector2dd &start, double shift) const
{
    return triangulate(start, Vector2dd(shift, 0));
}

/* functions to getDepth */
double Triangulator::getDepth(const Vector2dd &start, const Vector2dd &shift) const
{
    Vector2dd end = start + shift;

    Vector2dd startInCameraFrame = rectifiedToCameraFrameRight * start;
    Vector2dd endInCameraFrame   = rectifiedToCameraFrameLeft  * end;

    double d1, d2, err;

    Vector3dd start3d(startInCameraFrame, 1.0);
    Vector3dd end3d  (  endInCameraFrame, 1.0);

    rectifierData.decomposition.getScaler(start3d, end3d, d1, d2, err);
    return d1 * rectifierData.baseline;
}

double Triangulator::getDepth(const Vector2dd &start, double shift) const
{
    return getDepth(start, Vector2dd(shift, 0));
}

double Triangulator::getDisparity(const Vector3dd &point) const
{
    Vector2dd projectRight = rectifierData.getRectifiedProjectionRight(point);
    Vector2dd projectLeft  = rectifierData.getRectifiedProjectionLeft (point);
    return projectLeft.x() - projectRight.x();
}


template<class Type>
Vector2dd getInputDisparity(Type *input, int y, int x)
{
    return input->element(y,x);
}

template<>
Vector2dd getInputDisparity<DisparityBuffer>(DisparityBuffer *input, int y, int x)
{
    FlowElement *flowElement = &input->element(y,x);
    return Vector2dd(flowElement->x(), flowElement->y());
}

template<>
Vector2dd getInputDisparity<FloatFlowBuffer>(FloatFlowBuffer *input, int y, int x)
{
    return input->element(y,x).vector;
}


template<class InputType>
class ParallelTriangulate
{
    const Triangulator *realThis;
    InputType *input;
    int density;
    bool enforceRectify;
public:
    Cloud *result;
    void operator()( const BlockedRange<int>& r )
    {
        int i, j;
        for (i = r.begin(); i < r.end(); i++)
        {
            for (j = 0; j < input->w; j++)
            {
                if (!input->isElementKnown(i, j))
                    continue;
                /* Point in right frame*/
                Vector2dd start(j,i);
                Vector2dd shift;
                shift = getInputDisparity<InputType>(input, i,j);
                if (enforceRectify)
                {
                    shift.y() = 0;
                }
                SwarmPoint cloudPoint;
                cloudPoint.point = realThis->triangulate(start, shift);
                cloudPoint.texCoor = start;
                result->push_back(cloudPoint);
            }
        }
    }
#ifdef WITH_TBB
    ParallelTriangulate( ParallelTriangulate& x, tbb::split ) :
        realThis(x.realThis)
      , input(x.input)
      , density(x.density)
      , enforceRectify(x.enforceRectify)
    {
        result = new Cloud();
        result->reserve(10000);
    }

    void join( const ParallelTriangulate& y )
    {
        result->insert(result->end(), y.result->begin(), y.result->end());
    }
#endif
    ParallelTriangulate(const Triangulator *_realThis, InputType *_input, int _density, bool _enforceRectify ) :
        realThis(_realThis)
      , input(_input)
      , density(_density)
      , enforceRectify(_enforceRectify)
    {
        result = new Cloud();
        result->reserve(10000);
    }

    ~ParallelTriangulate()
    {
        delete_safe(result);
    }
};

template<class InputType>
Cloud *Triangulator::triangulateHelperParallel (InputType *input,  int density, bool enforceRectify) const
{
    ParallelTriangulate<InputType> tr(this, input, density, enforceRectify);

    parallelable_reduce(0, input->h, tr);

    return new Cloud(*(tr.result));
}

template<class InputType>
Cloud *Triangulator::triangulateHelper (InputType *input,  int density, bool enforceRectify) const
{
    Cloud *result = new Cloud();
    result->reserve(density);

    for (int i = 0; i < input->h; i++)
    {
        for (int j = 0; j < input->w; j++)
        {
            if (!input->isElementKnown(i, j))
                continue;
            /* Point in right frame*/
            Vector2dd start(j,i);
            Vector2dd shift;
            shift = getInputDisparity<InputType>(input, i,j);
            if (enforceRectify) {
                shift.y() = 0;
            }

            SwarmPoint cloudPoint;
            cloudPoint.point = triangulate(start, shift);
            cloudPoint.texCoor = start;
            result->push_back(cloudPoint);
        }
    }
    return result;
}

Cloud *Triangulator::triangulate (FlowBuffer *input,  int density, bool enforceRectify) const
{
#ifdef WITH_TBB
    return triangulateHelperParallel(input, density, enforceRectify);
#endif
    return triangulateHelper(input,  density, enforceRectify);
}

Cloud *Triangulator::triangulate (FloatFlowBuffer *input,  int density, bool enforceRectify) const
{
#ifdef WITH_TBB
    return triangulateHelperParallel(input, density, enforceRectify);
#endif
    return triangulateHelper(input,  density, enforceRectify);
}

#if 0
/* Merge this to one function with */
Cloud *Triangulator::triangulate (FlowBuffer *input,  int density, bool enforceRectify) const
{
    //vector<SwarmPoint> *result = new vector<SwarmPoint>();
    Cloud *result = new Cloud();
    result->reserve(density);

    for (int i = 0; i < input->h; i++)
    {
        for (int j = 0; j < input->w; j++)
        {
            if (!input->isElementKnown(i, j))
                continue;

            FlowElement *flowElement = &input->element(i,j);
            /* Point in right frame*/
            Vector2dd start(j,i);
            Vector2dd shift;
            shift = Vector2dd(flowElement->x(), enforceRectify ? 0 : flowElement->y());

            SwarmPoint cloudPoint;
            cloudPoint.point = triangulate(start, shift);
            cloudPoint.texCoor = start;
            result->push_back(cloudPoint);
        }
    }
    return result;
}


Cloud *Triangulator::triangulate (FloatFlowBuffer *input,  int density, bool enforceRectify) const
{
    Cloud *result = new Cloud();
    result->reserve(density);

    for (int i = 0; i < input->h; i++)
    {
        for (int j = 0; j < input->w; j++)
        {
            if (!input->isElementKnown(i, j))
                continue;

            FloatFlow *flowElement = &input->element(i,j);
            /* Point in right frame*/
            Vector2dd start(j,i);
            Vector2dd shift;
            shift = Vector2dd(flowElement->vector.x(), enforceRectify ? 0 : flowElement->vector.y());

            SwarmPoint cloudPoint;
            cloudPoint.point = triangulate(start, shift);
            cloudPoint.texCoor = start;
            result->push_back(cloudPoint);
        }
    }
    return result;
}
#endif


Cloud *Triangulator::triangulate (SixDBuffer *input, int density) const
{
    Cloud *result = new Cloud();
    result->reserve(density);

    for (int i = 0; i < input->h; i++)
    {
        for (int j = 0; j < input->w; j++)
        {
            const Element6D &element = input->element(i,j);
            if (!element.hasDisp() || element.disp <= 0)
                continue;

            /* Point in right frame*/
            Vector2dd start(j,i);
            Vector2dd shift = Vector2dd(element.disp, 0);

            SwarmPoint swarmPoint;
            Vector3dd current3d = triangulate(start, shift);
            swarmPoint.point = current3d;
            swarmPoint.texCoor = start;

            /* Now 6D */
            if (element.hasFlow())
            {
                /*Position at previous frame */
                Vector2dd start1(start.x() + element.flow.x(), start.y() + element.flow.y());
                Vector3dd old3d = current3d;

                if (element.hasPrevDisp() && element.dispPrev > 0)
                {
                    Vector2dd shift1 = Vector2dd(element.dispPrev, 0);
                    old3d = triangulate(start1, shift1);
                    swarmPoint.is6D = true;
                } else {
                    //Vector2dd normalisedEnd1 = rectifiedToNormalizedRight * end1;
                    //old3d = Vector3dd(normalisedEnd1, 1.0) * current3d.z();
                    //old3d = current3d;
                }
                swarmPoint.speed = current3d - old3d;
            }

            result->push_back(swarmPoint);
        }
    }
    return result;
}

DepthBuffer *Triangulator::triangulateToDB (DisparityBuffer *input, bool enforceRectify) const
{
    if (input != NULL) {
        return NULL;
    }
    DepthBuffer *result = new DepthBuffer(input->h, input->w, false);
    for (int i = 0; i < result->h; i++)
    {
        for (int j = 0; j < result->w; j++)
        {
            if (!input->isElementKnown(i, j)) {
                result->element(i,j) = DepthBuffer::DEPTH_UNKNOWN;
                continue;
            }

            FlowElement *flowElement = &input->element(i,j);
            /* Point in right frame*/
            Vector2dd start(j,i);
            Vector2dd shift(flowElement->x(), enforceRectify ? 0 : flowElement->y());
            result->element(i,j) = getDepth(start, shift);
        }
    }
    return result;
}


} //namespace corecvs

