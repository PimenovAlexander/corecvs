#include "transformationCache.h"

namespace corecvs
{

TransformationCache::TransformationCache(Matrix33 matrix, int w, int h, Vector2d<int> inputSize)
{
    mRemap      = new DirectRemapper    (&matrix, inputSize, Vector2d<int>(w, h));
    mDisplace   = new DisplacementBuffer(&matrix, h, w);
    mF8Displace = new FixedPointDisplace( matrix, h, w);
}

G12Buffer *TransformationCache::doDeformation(InterpolationType::InterpolationType type, G12Buffer *inputFrame)
{
    switch (type)
    {
        case InterpolationType::BILINEAR :
            return inputFrame->doReverseDeformationBl<G12Buffer, DisplacementBuffer> (
                        mDisplace,
                        mDisplace->h,
                        mDisplace->w);
            break;

        case InterpolationType::NEAREST :
            return mRemap->remap(inputFrame);
            break;

        case InterpolationType::BILINEAR_FIXED8 :
            return inputFrame->doReverseDeformationBlPrecomp(
                        mF8Displace,
                        mF8Displace->h,
                        mF8Displace->w);
            break;

        default:
            break;
    }
    return new G12Buffer(inputFrame);
}

/* TODO: Make a template for this */
RGB24Buffer *TransformationCache::doDeformation(InterpolationType::InterpolationType type, RGB24Buffer *inputFrame)
{
    switch (type)
    {
        case InterpolationType::BILINEAR :
            return inputFrame->doReverseDeformationBl<RGB24Buffer, DisplacementBuffer> (
                        mDisplace,
                        mDisplace->h,
                        mDisplace->w);
            break;

        case InterpolationType::NEAREST :
            return mRemap->remap(inputFrame);
            break;

        case InterpolationType::BILINEAR_FIXED8 :
            return inputFrame->doReverseDeformationBlPrecomp(
                        mF8Displace,
                        mF8Displace->h,
                        mF8Displace->w);
            break;

        default:
            break;
    }
    return new RGB24Buffer(inputFrame);
}


TransformationCache::~TransformationCache()
{
    delete_safe(mRemap);
    delete_safe(mDisplace);
    delete_safe(mF8Displace);
}

} // namespace corecvs
