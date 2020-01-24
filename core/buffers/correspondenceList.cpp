/**
 * \file correspondenceList.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Mar 24, 2010
 * \author alexander
 */

#include "core/math/mathUtils.h"
#include "core/buffers/correspondenceList.h"
#include "core/kltflow/kltGenerator.h"
namespace corecvs {

CorrespondenceList::CorrespondenceList()
{
}

CorrespondenceList::CorrespondenceList(const CorrespondenceList &list) : vector<Correspondence>(list), h(list.h), w(list.w)
{
}

CorrespondenceList::~CorrespondenceList()
{
}

template<bool swap = false>
class FlowBufferToucherOperator
{
public:
    CorrespondenceList *parent;

    FlowBufferToucherOperator(CorrespondenceList *_parent) : parent(_parent){}

    void operator ()(int32_t i, int32_t j, const FlowElement &flow)
    {
        if (flow.x() == FlowBuffer::FLOW_UNKNOWN_X || flow.y() == FlowBuffer::FLOW_UNKNOWN_Y)
            return;

        if (!swap)
        {
            Correspondence corr(Vector2dd(j,i), Vector2dd((double)j + flow.x(),(double)i + flow.y()));
            parent->push_back(corr);
        } else {
            Correspondence corr(Vector2dd((double)j + flow.x(),(double)i + flow.y()), Vector2dd(j,i));
            parent->push_back(corr);
        }
    }

    void operator ()(int32_t i, int32_t j, const FloatFlow &flow)
    {
        if (!flow.isKnown)
            return;
        Correspondence corr(Vector2dd(j,i), Vector2dd(j,i) + flow.vector);
        parent->push_back(corr);
    }

};

void CorrespondenceList::getNormalizingTransform(Matrix33 &transformRight, Matrix33 &transformLeft, double *scale)
{
    Vector2dd rightMean(0.0);
    Vector2dd rightMeanSq(0.0);
    Vector2dd leftMean(0.0);
    Vector2dd leftMeanSq(0.0);

    for (unsigned  i = 0; i < this->size(); i++)
    {
        rightMean   += (*this)[i].start;
        rightMeanSq += ((*this)[i].start) * ((*this)[i].start);

        leftMean   += (*this)[i].end;
        leftMeanSq += ((*this)[i].end) * ((*this)[i].end);
    }
    int count = (this->size() == 0) ? 1 : (int)this->size();

    rightMean   /= count;
    rightMeanSq /= count;
    leftMean    /= count;
    leftMeanSq  /= count;

    Vector2dd rightSqMean = rightMean * rightMean;
    Vector2dd leftSqMean  = leftMean * leftMean;

    double rightScaler = sqrt (rightMeanSq.x() - rightSqMean.x()  + rightMeanSq.y() - rightSqMean.y());
    double leftScaler  = sqrt (leftMeanSq.x()  - leftSqMean.x()   + leftMeanSq.y()  - leftSqMean.y());

    if (rightScaler < 1e-15) rightScaler = 1.0;
    if (leftScaler < 1e-15) leftScaler = 1.0;

    transformRight = Matrix33::ScaleProj(sqrt(2.0) / rightScaler) * Matrix33::ShiftProj(-rightMean);
    transformLeft  = Matrix33::ScaleProj(sqrt(2.0) / leftScaler ) * Matrix33::ShiftProj(-leftMean);

    if (scale != NULL )
        *scale = sqrt(2.0f) / ((rightScaler + leftScaler) * 2.0) ;
}



void CorrespondenceList::transform(const ProjectiveTransform &transformRight, const ProjectiveTransform &transformLeft)
{
    for (unsigned i = 0; i < this->size(); i++)
    {
        Correspondence *corr = &((*this)[i]);
        corr->start = transformRight (corr->start);
        corr->end   = transformLeft  (corr->end);
    }
}


CorrespondenceList::CorrespondenceList(FlowBuffer *input, bool swap) :
            h(input->h),
            w(input->w)
{
    if (swap)
    {
        FlowBufferToucherOperator<true> toucher(this);
        input->touchOperationElementwize(toucher);
    } else {
        FlowBufferToucherOperator<false> toucher(this);
        input->touchOperationElementwize(toucher);
    }
}

CorrespondenceList::CorrespondenceList(FloatFlowBuffer *input) :
            h(input->h),
            w(input->w)
{
    // SYNC_PRINT(("CorrespondenceList::CorrespondenceList():called\n"));
    FlowBufferToucherOperator<> toucher(this);
    input->touchOperationElementwize(toucher);
}

CorrespondenceList * CorrespondenceList::makePreciseCopy(G12Buffer *first, G12Buffer *second)
{
    SpatialGradient *sg = new SpatialGradient(first);
    SpatialGradientIntegralBuffer *gradient = new SpatialGradientIntegralBuffer(sg);
    delete sg;

    KLTCalculationContext context;
    context.first = first;
    context.second = second;
    context.gradient = gradient;

    KLTGenerator<BilinearInterpolatorD> kltGenerator;

    CorrespondenceList *toReturn = new CorrespondenceList();
    toReturn->h = this->h;
    toReturn->w = this->w;

    for (unsigned i = 0; i < this->size(); i++)
    {
        // TODO: Be careful with this.
        Vector2dd start = Vector2dd(
                    fround(at(i).start.x()),
                    fround(at(i).start.y())
                  );
        Vector2dd guess = at(i).end - at(i).start;
        bool status = kltGenerator.kltIterationSubpixel(context, start, &guess, 2);
        if (status)
        {
            Correspondence corr(
                    Vector2dd(start.x(),start.y()),
                    Vector2dd(start.x() + guess.x(),start.y() + guess.y())
                    );
            toReturn->push_back(corr);
        }
    }

    delete gradient;
    return toReturn;
}

void CorrespondenceList::makePrecise(G12Buffer *first, G12Buffer *second)
{
    SpatialGradient *sg = new SpatialGradient(first);
    SpatialGradientIntegralBuffer *gradient = new SpatialGradientIntegralBuffer(sg);
    delete sg;

    KLTCalculationContext context;
    context.first = first;
    context.second = second;
    context.gradient = gradient;

    KLTGenerator<BilinearInterpolatorD> kltGenerator;

    for (unsigned i = 0; i < this->size(); i++)
    {
        // TODO: Be careful with this.
        Vector2dd start = Vector2dd(at(i).start.x(),at(i).start.y());
        Vector2dd guess = at(i).end - at(i).start;
        bool status = kltGenerator.kltIterationSubpixel(context, start, &guess, 2);
        if (status)
        {
            Correspondence corr(
                    Vector2dd(start.x(),start.y()),
                    Vector2dd(start.x() + guess.x(),start.y() + guess.y())
                    );
            int oldFlags = at(i).flags;
            at(i) = corr;
            at(i).flags = oldFlags;
        } else {
            at(i).flags |= Correspondence::FLAG_FILTERED_KLT;
        }

    }

    delete gradient;
}


} //namespace corecvs

