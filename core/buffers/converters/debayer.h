/**
 * \file    buffers/converters/debayer.h
 *
 * \author pavel.vasilev
 * \date 21 Oct 2015
 *
 * Declares the Debayer class.
 */
#ifndef DEBAYER_H
#define DEBAYER_H

#include <string>
#include <map>

#include "utils/global.h"

#include "math/vector/vector2d.h"
#include "math/vector/vector3d.h"
#include "buffers/g12Buffer.h"
#include "buffers/rgb24/rgbTBuffer.h"
#include "buffers/rgb24/rgb24Buffer.h"
#include "fileformats/metamap.h"

#include "xml/generated/debayerParameters.h"

namespace corecvs {

class Debayer
{
public:
    class Parameters : public DebayerParameters
    {
    public:
        Parameters(int bayerPos = -1                            // bayerPos autodetect from pgm
            , const std::vector<double> &gn = { 1.0, 1.0, 1.0 } // gains R,G,B, where G=1.0 always
            , const std::vector<double> &gm = { 1.0, 1.0 }      // gammas for dark and white areas
            , DebayerMethod::DebayerMethod mtd = DebayerMethod::BILINEAR // 0:near, 1:bilinear, 2:AHD
            , int ob = 12                                       // output gets 12bits also
        )
        {
            setMethod(mtd), setBayerPos(bayerPos), setNumBitsOut(ob);
            setGains(gn), setGamma(gm);
        }

        static Parameters& BestDefaultsByExt(const std::string& ext);
        static std::map<std::string, Parameters> bestDefsMap;
    };

    /**
     * Common useful method to perform demosaic for the given Bayer with given params
     *
     */
    static corecvs::RGB48Buffer *DemosaicRgb48(corecvs::G12Buffer* bayer, const Debayer::Parameters &params, MetaData &meta);
    static corecvs::RGB24Buffer *Demosaic(corecvs::G12Buffer* bayer, const Debayer::Parameters &params);

    static void ConvertRgb48toRgb24(const corecvs::RGB48Buffer *in, corecvs::RGB24Buffer *out, int shift = 12 - 8);
    static void ConvertRgb24toRgb48(const corecvs::RGB24Buffer *in, corecvs::RGB48Buffer *out);

    /**
     * Constructor.
     *
     * \param [in,out] bayer Raw bayer data
     * \param depthOut       Processing bit depth
     * \param [in,out] data  (Optional) Metadata
     * \param bayerPos       (Optional) Bayer position (RGGB or others)
     */
    Debayer(G12Buffer *bayer, int depthOut = 12, MetaData *data = nullptr, int bayerPos = -1);

    ~Debayer();

    /**
     * Converts the image to RGB48.
     *
     * \param   quality Demosaic quality (method).
     * \param   out     Resulting image.
     *
     * \return  Error code.
     */
    int         toRGB48(DebayerMethod::DebayerMethod method, RGB48Buffer* out);

    /** this is quick and dirty helper **/
    int         toRGB24(DebayerMethod::DebayerMethod method, RGB24Buffer* out);

    /**
    * Fill bayer data from RGB48 image applying Bayerian grid to it.
    *
    * \param   inRGB Image to get pixel data from.
    */
    void        fromRgb(RGB48Buffer *inRgb);

    void        getYChannel(AbstractBuffer<double,int> *output);

    uint8_t     colorFromBayerPos(uint i, uint j, bool rggb = true);

    enum CompareMethod { PSNR = 0, RMSD = 1 };

private:
    Vector3dd   mScaleMul   = { 1, 1, 1 };
    uint16_t    mBlack      = 0;
    uint8_t     mBayerPos   = 0;
    uint16_t*   mCurve      = nullptr;
    uint16_t    mMaximum    = 0;
    bool        mScale      = true;

    G12Buffer*  mBayer;
    MetaData *  mMetadata;
    int         mDepth;

    void        scaleCoeffs();
    void        gammaCurve(uint16_t *curve, int imax);
    void        preprocess(bool overwrite = false);

    void        linear (RGB48Buffer* out);
    void        nearest(RGB48Buffer* out);
    void        ahd    (RGB48Buffer* out);

    // use for testing only!
    void        fourier(RGB48Buffer *result);

    void        borderInterpolate(int radius, RGB48Buffer *result);


    /* utilitary functions */

    /**
     * Clip int to uint16.
     *
     * \param   x       Value.
     * \param   depth   Bits to clip at.
     *
     * \return  Clipped value.
     */
    inline uint16_t clip(int32_t x)                          { return x < 0 ? 0 : (x > mMaximum ? mMaximum : (uint16_t)x); }

    inline uint16_t getC(uint32_t channelValue, int chanIdx) { return clip(roundUp((channelValue - mBlack) * mScaleMul[chanIdx])); }
    inline uint16_t outR(uint32_t r)                         { return getC(r, 0); }
    inline uint16_t outG(uint32_t g)                         { return getC(g, 1); }
    inline uint16_t outB(uint32_t b)                         { return getC(b, 2); }

    /**
     * Clamp the given value. If a &lt; b, treat a as left limit and b as right. Invert limits
     * otherwise.
     *
     * \param   x   Value.
     * \param   a   Limit.
     * \param   b   Limit.
     *
     * \return  Clamped value.
     */
    inline int32_t clamp(int32_t x, int32_t a, int32_t b) { if (a > b) SwapXY(a, b); return x < a ? a : (x > b ? b : x); }

    /**
     * Calculate weighted average.
     *
     * \param   coords  The coordinates to average pixels at. If the coordinate exceeds image
     *                  boundaries, it will be ignored.
     *
     * \return  Averaged value.
     */
    inline int32_t weightedBayerAvg(const vector<Vector2d32>& coords, const vector<int>& coeffs = vector<int>());
    inline int32_t weightedBayerAvg(const Vector2d32& coords);

};

} // namespace corecvs

#endif // DEBAYER_H
