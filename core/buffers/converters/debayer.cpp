#include "buffers/converters/debayer.h"
#include "buffers/converters/labConverter.h"
#include "utils/log.h"
#include "math/fftw/fftwWrapper.h"

namespace corecvs {

using std::pow;
using std::max;
using std::min;
using std::abs;

std::map<std::string, Debayer::Parameters> Debayer::Parameters::bestDefsMap;

Debayer::Parameters& Debayer::Parameters::BestDefaultsByExt(const string& ext)
{
    if (bestDefsMap.size() == 0)
    {
        Debayer::Parameters raw;
        raw.setGains({ 1.3, 1.0, 1.8 });
        raw.setGamma({ 0.4, 2.8 });
        bestDefsMap["raw"] = raw;

        Debayer::Parameters pgm(1); // bpos=1
        pgm.setGains({ 1.34, 1.0, 1.75 });
        bestDefsMap["pgm"] = pgm;
    }

    string s(ext);
    if (s[0] == '.') s = s.substr(1);       // remove dot if present

    if (!bestDefsMap.count(s))
    {
        bestDefsMap[s] = Debayer::Parameters();
    }
    return bestDefsMap[s];
}

corecvs::RGB48Buffer *Debayer::DemosaicRgb48(corecvs::G12Buffer* bayer, const Debayer::Parameters &params, MetaData &meta)
{
    auto toReturn = new RGB48Buffer(bayer->h, bayer->w, false);

    cint outBits = 12;
    cint maxVal = (1 << params.numBitsOut()) - 1; // real wished maxVal by given #bits

    if (meta["bits"].empty())                   // do overwrite possible data from PGM by given params
        meta["bits"].push_back(outBits);
    else
        meta["bits"][0] = outBits;

    if (meta["white"].empty())                  // do overwrite possible data from PGM by given params
        meta["white"].push_back(maxVal);
    else
        meta["white"][0] = maxVal;

    //while (maxVal >> int(meta["bits"][0])) { meta["bits"][0]++; }

    if (meta["gamm"].empty() && Vector2dd(params.gamma()) != Vector2dd(1, 1))  // don't overwrite data from PGM
    {
        meta["gamm"] = MetaValue(5, 0);
        meta["gamm"][0] = params.gamma()[0];
        meta["gamm"][1] = params.gamma()[1];
    }

    if (meta["cam_mul"].empty() && Vector3dd(params.gains()) != Vector3dd(1, 1, 1))  // don't overwrite data from PGM
    {
        meta["cam_mul"] = MetaValue(4, -1.0);
        meta["cam_mul"][0] = params.gains()[0];
        meta["cam_mul"][1] = params.gains()[1];
        meta["cam_mul"][2] = params.gains()[2];
    }

    L_INFO_P("bits:%d maxval:%d outBits:%d bpos:%d method:%s gamma:[%.1f,%.1f] gains:[%.1f,%.1f,%.1f]"
        , (int)meta["bits"][0]
        , (int)meta["white"][0]
        , params.numBitsOut()
        , params.bayerPos()
        , DebayerMethod::getName(params.method())
        , meta["gamm"].empty() ? 1. : meta["gamm"][0]
        , meta["gamm"].empty() ? 1. : meta["gamm"][1]
        , meta["cam_mul"].empty() ? 1. : meta["cam_mul"][0]
        , meta["cam_mul"].empty() ? 1. : meta["cam_mul"][1]
        , meta["cam_mul"].empty() ? 1. : meta["cam_mul"][2]
    );

    Debayer debayer(bayer, outBits, &meta, params.bayerPos());

    int code = debayer.toRGB48(params.method(), toReturn);
    if (code != 0) {
        L_ERROR_P("debayer returned error code: %d", code);
    }

    return toReturn;
}

corecvs::RGB24Buffer *Debayer::Demosaic(corecvs::G12Buffer* bayer, const Debayer::Parameters &params)
{
    auto toReturn = new RGB24Buffer(bayer->h, bayer->w, false);

#ifdef BASIC
    for (int i = 0; i < bayer->h; i += 2)
    {
        for (int j = 0; j < bayer->w; j += 2)
        {
            /* Copy paste so far. Integrate it into debayer*/
            uint32_t g1 = ((uint32_t)bayer->element(i, j + 1)) >> 2;
            uint32_t r = ((uint32_t)bayer->element(i, j)) >> 2;
            uint32_t b = ((uint32_t)bayer->element(i + 1, j + 1)) >> 2;
            uint32_t g2 = ((uint32_t)bayer->element(i + 1, j)) >> 2;

            if (r > 255 || g1 > 255 || g2 > 255 || b > 255) {
                printf("trouble\n");
            }
            RGBColor color = RGBColor((uint8_t)r, (uint8_t)((g1 + g2) / 2), (uint8_t)b);
            toReturn->element(i, j) = color;
            toReturn->element(i, j + 1) = color;
            toReturn->element(i + 1, j) = color;
            toReturn->element(i + 1, j + 1) = color;
        }
    }
#else // BASIC

    MetaData meta;
    std::unique_ptr<RGB48Buffer> rgb48(DemosaicRgb48(bayer, params, meta));

    cint outBits = 12;
    cint shift = outBits - 8;        // 16: 8, 12: 4, 10: 2
    ConvertRgb48toRgb24(rgb48.get(), toReturn, shift);

#endif // !BASIC

    return toReturn;
}

void Debayer::ConvertRgb48toRgb24(const RGB48Buffer *in, RGB24Buffer *out, int shift)
{
    CORE_ASSERT_TRUE_S(out->getSize() == in->getSize());

    for (int i = 0; i < out->h; ++i)
    {
        for (int j = 0; j < out->w; ++j)
        {
            RGBColor48 color = in->element(i, j);
            uint8_t r = (uint8_t)(color.r() >> shift);  // clip not needed
            uint8_t g = (uint8_t)(color.g() >> shift);
            uint8_t b = (uint8_t)(color.b() >> shift);

            out->element(i, j) = RGBColor(r, g, b);
        }
    }
}

void Debayer::ConvertRgb24toRgb48(const RGB24Buffer *in, RGB48Buffer *out)
{
    CORE_ASSERT_TRUE_S(out->getSize() == in->getSize());

    for (int i = 0; i < out->h; ++i)
    {
        for (int j = 0; j < out->w; ++j)
        {
            RGBColor c = in->element(i, j);
            RGBColor48 c48(c.r() << 8, c.g() << 8, c.b() << 8);

            out->element(i, j) = c48;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

Debayer::Debayer(G12Buffer *bayer, int depth, MetaData *metadata, int bayerPos)
    : mBayer(bayer)
    , mMetadata(metadata)
    , mDepth(depth)
{
    mMaximum = (1 << depth) - 1;
    
    if (bayerPos < 0)
    {
        if (metadata == nullptr) {
            mBayerPos = 0;
        }
        else {
            MetaData &meta = *metadata;
            mBayerPos = meta["b_pos"].empty() ? 0 : meta["b_pos"][0];
        }
    }
    else {
        mBayerPos = bayerPos;
    }
}

Debayer::~Debayer()
{
    deletearr_safe(mCurve);
}

void Debayer::nearest(RGB48Buffer *result)
{
    RGBColor48 pixel;

    // swapCols inverts least significant bit for cols when set so RG/GB becomes GR/BG, etc.
    // swapRows does the same for rows
    //
    int swapCols =  mBayerPos & 1;
    int swapRows = (mBayerPos & 2) >> 1;

    // for now, don't handle first and last rows/columns if swapRows/swapCols is set
    //
    int i = swapRows;
    for (; i < mBayer->h - swapRows; i += 2)
    {
        // TODO: sse-ify this part
        //
        for (int j = swapCols; j < mBayer->w - swapCols; j += 2)
        {
            uint32_t r =  mBayer->element(i, j);
            uint32_t g = (mBayer->element(i, j + 1) + mBayer->element(i + 1, j)) / 2;
            uint32_t b =  mBayer->element(i + 1, j + 1);

            pixel[2] = mCurve[outR(r)];     // in the b,g,r order
            pixel[1] = mCurve[outG(g)];
            pixel[0] = mCurve[outB(b)];

            result->element(i,     j    ) = pixel; // r
            result->element(i,     j + 1) = pixel; // g
            result->element(i + 1, j    ) = pixel; // g
            result->element(i + 1, j + 1) = pixel; // b
        }
    }
}

void Debayer::linear(RGB48Buffer *result)
{
    uint32_t r, g, b;
    int swapCols =  mBayerPos & 1;
    int swapRows = (mBayerPos & 2) >> 1;

    int i = 2 - swapRows;

    borderInterpolate(2 - min(swapCols, swapRows), result);

    for (; i < mBayer->h - 2 + swapRows; i += 2)
    {
        // TODO: SSE-ify this part!
// XXX: What's happening here? Why would you ever wrap non-sse related code into WITH_SSE?!
//#ifdef WITH_SSE
        for (int j = 2 - swapCols; j < mBayer->w - 2 + swapCols; j += 2)
        {
            // R pixel
            r =  mBayer->element(i, j);
            g = (mBayer->element(i, j - 1) + mBayer->element(i, j + 1) + mBayer->element(i + 1, j) + mBayer->element(i - 1, j)) / 4;
            b = (mBayer->element(i - 1, j - 1) + mBayer->element(i - 1, j + 1) +
                 mBayer->element(i + 1, j - 1) + mBayer->element(i + 1, j + 1)) / 4;

            result->element(i, j) = { mCurve[outR(r)], mCurve[outG(g)], mCurve[outB(b)] };

            // G1 pixel
            g =  mBayer->element(i, j + 1);
            r = (mBayer->element(i,     j    ) + mBayer->element(i,     j + 2)) / 2;
            b = (mBayer->element(i + 1, j + 1) + mBayer->element(i - 1, j + 1)) / 2;

            result->element(i, j + 1) = { mCurve[outR(r)], mCurve[outG(g)], mCurve[outB(b)] };


            // G2 pixel - G1 inverted
            g =  mBayer->element(i + 1, j);
            b = (mBayer->element(i + 1, j - 1) + mBayer->element(i + 1, j + 1)) / 2;
            r = (mBayer->element(i + 2, j    ) + mBayer->element(i,     j    )) / 2;

            result->element(i + 1, j) = { mCurve[outR(r)], mCurve[outG(g)], mCurve[outB(b)] };

            // B pixel - R inverted
            b =  mBayer->element(i + 1, j + 1);
            g = (mBayer->element(i + 1, j) + mBayer->element(i + 1, j + 2) + mBayer->element(i + 2, j + 1) + mBayer->element(i, j + 1)) / 4;
            r = (mBayer->element(i,     j) + mBayer->element(i,     j + 2) +
                 mBayer->element(i + 2, j) + mBayer->element(i + 2, j + 2)) / 4;

            result->element(i + 1, j + 1) = { mCurve[outR(r)], mCurve[outG(g)], mCurve[outB(b)] };
        }
//#endif
    }
}

int compare(const void * a, const void * b)
{
    return (*(int*)a - *(int*)b);
}
//inline int compared(const void * a, const void * b)
//{
//    return int(*(double*)a - *(double*)b);
//}

void Debayer::ahd(RGB48Buffer *result)
{
    // allocate buffers for two directions
    G12Buffer *green[2] = {
        new G12Buffer(mBayer->h, mBayer->w, false),
        new G12Buffer(mBayer->h, mBayer->w, false)
    };
    RGB48Buffer *rgb[2] = {
        new RGB48Buffer(mBayer->h, mBayer->w, true),
        new RGB48Buffer(mBayer->h, mBayer->w, true)
    };

    float(*Lab[2])[3] = {
        new float[mBayer->h * mBayer->w][3],
        new float[mBayer->h * mBayer->w][3]
    };

    const vector<int> filter = { -1, 2, 2, 2, -1 };

    int32_t val = 0;

    int swapCols =  mBayerPos & 1;
    int swapRows = (mBayerPos & 2) >> 1;

    // interpolate green
    for (int i = swapRows; i < mBayer->h - swapRows; i += 2)
    {
        for (int j = swapCols; j < mBayer->w - swapCols; j += 2)
        {
            green[0]->element(i, j + 1) = mBayer->element(i, j + 1);
            green[1]->element(i, j + 1) = mBayer->element(i, j + 1);

            green[0]->element(i + 1, j) = mBayer->element(i + 1, j);
            green[1]->element(i + 1, j) = mBayer->element(i + 1, j);

            // apply low-pass filter with coefficients -1/4,1/2,1/2,1/2,-1/4 and clamp the resulting green value by its two neighbours
            // the coefficients are close to these of Hirakawa & Parks' optimal filter
            // (i, j) - red
            //vertical
            val = weightedBayerAvg({ { j, i - 2 }, { j, i - 1 }, { j, i }, { j, i + 1 }, { j, i + 2 } }, filter) / 4;
            val = clamp(val, weightedBayerAvg({ j, i - 1 }), weightedBayerAvg({ j, i + 1 }));
            green[0]->element(i, j) = clip(val);

            // horizontal
            val = weightedBayerAvg({ { j - 2, i }, { j - 1, i }, { j, i }, { j + 1, i }, { j + 2, i } }, filter) / 4;
            val = clamp(val, weightedBayerAvg({ j - 1, i }), weightedBayerAvg({ j + 1, i }));
            green[1]->element(i, j) = clip(val);

            // (i + 1, j + 1) - blue
            //vertical
            val = weightedBayerAvg({ { j + 1, i - 1}, { j + 1, i }, { j + 1, i + 1 }, { j + 1, i + 2 }, { j + 1, i + 3 } }, filter) / 4;
            val = clamp(val, weightedBayerAvg({ j + 1, i }), weightedBayerAvg({ j + 1, i + 2 }));
            green[0]->element(i + 1, j + 1) = clip(val);

            // horizontal
            val = weightedBayerAvg({ { j - 1, i + 1 }, { j, i + 1 }, { j + 1, i + 1 }, { j + 1 + 1, i + 1 }, { j + 1 + 2, i + 1 } }, filter) / 4;
            val = clamp(val, weightedBayerAvg({ j, i + 1 }), weightedBayerAvg({ j + 2, i + 1 }));
            green[1]->element(i + 1, j + 1) = clip(val);
        }
    }

    RGBColor48 pixel;

    // interpolate red and blue first vertically, then horizontally
    for (int d = 0; d < 2; d++)
    {
        for (int i = 1; i < mBayer->h - 1; i += 2)
        {
            for (int j = 1; j < mBayer->w - 1; j += 2)
            {
                for (int k = 0; k < 2; k++)
                {
                    for (int l = 0; l < 2; l++)
                    {
                        pixel[1] = outG(green[d]->element(i + k, j + l));

                        uint8_t color = colorFromBayerPos(i + k, j + l, false);

                        if (color == 1)
                        {
                            uint8_t rowColor = colorFromBayerPos(i + k, j + l + 1, false);

                            // C = G + LP(C' - G'), where C is sought colour
                            // LP is in fact the average
                            val = pixel[1] + ((weightedBayerAvg({ j + l - 1, i + k }) - green[d]->element(i + k, j + l - 1)
                                             + weightedBayerAvg({ j + l + 1, i + k }) - green[d]->element(i + k, j + l + 1)) / 2);
                            
                            // logically, this should be pixel[rowColor], but our pixels are BGR, so this is inverted
                            pixel[2 - rowColor] = clip(val * mScaleMul[rowColor]);

                            val = pixel[1] + ((weightedBayerAvg({ j + l, i + k - 1 }) - green[d]->element(i + k - 1, j + l)
                                             + weightedBayerAvg({ j + l, i + k + 1 }) - green[d]->element(i + k + 1, j + l)) / 2);
                            pixel[rowColor] = clip(val * mScaleMul[2 - rowColor]);
                        }
                        else
                        {
                            // known colour: inverted (same as above)
                            pixel[2 - color] = clip(mBayer->element(i + k, j + l) * mScaleMul[color]);

                            // interpolate colour using greens diagonally
                            // this is not intuitive, but directly follows from the aforementioned equation
                            val = pixel[1] +
                                    ((weightedBayerAvg({ j + l - 1, i + k - 1 }) - green[d]->element(i + k - 1, j + l - 1)
                                    + weightedBayerAvg({ j + l + 1, i + k - 1 }) - green[d]->element(i + k + 1, j + l + 1)
                                    + weightedBayerAvg({ j + l - 1, i + k + 1 }) - green[d]->element(i + k - 1, j + l + 1)
                                    + weightedBayerAvg({ j + l + 1, i + k + 1 }) - green[d]->element(i + k + 1, j + l - 1)) / 4);

                            pixel[color] = clip(val * mScaleMul[2 - color]);
                        }
                        rgb[d]->element(i + k, j + l) = pixel;
                        LabConverter::rgb2Lab(pixel, Lab[d][(i + k) * mBayer->w + j + l]);
                    }
                }
            }
        }
    }

    // free some memory
    delete_safe(green[0]);
    delete_safe(green[1]);

    float *homo[2] = {
        new float[mBayer->h * mBayer->w],
        new float[mBayer->h * mBayer->w]
    };

    // build homogeneity maps using cielab metric
    for (int i = 0; i < mBayer->h; i++)
    {
        for (int j = 0; j < mBayer->w; j++)
        {
            int offset = i * mBayer->w + j;

            // luminance difference in 4 directions for 2 images
            float dl[2][4] = {0};

            // chrominance difference in 4 directions for 2 images
            float dc[2][4] = {0};

            for (int d = 0; d < 2; d++)
            {
                int idx = 0;
                for (int k = -1; k < 2; k += 2, idx++)
                {
                    int shiftA = k * mBayer->w;
                    if (offset + shiftA < 0 || offset + shiftA >= mBayer->h * mBayer->w)
                        continue;
                    dl[d][idx] = abs(Lab[d][offset][0] - Lab[d][offset + shiftA][0]);
                    dc[d][idx] = pow(Lab[d][offset][1] - Lab[d][offset + shiftA][1], 2) + pow(Lab[d][offset][2] - Lab[d][offset + shiftA][2], 2);
                }
                for (int l = -1; l < 2; l += 2, idx++)
                {
                    int shiftB = l;
                    if (offset + shiftB < 0 || offset + shiftB >= mBayer->h * mBayer->w)
                        continue;
                    dl[d][idx] = abs(Lab[d][offset][0] - Lab[d][offset + shiftB][0]);
                    dc[d][idx] = pow(Lab[d][offset][1] - Lab[d][offset + shiftB][1], 2) + pow(Lab[d][offset][2] - Lab[d][offset + shiftB][2], 2);
                }
            }

            // min { max { DIFF(x, neighbor) } } for all vertical/horizontal immediate neighbors from B(x, 1)
            // the luminance and chrominance deviations are defined as maximal deviation in any direction
            // we must choose minimal deviations to count homogenous pixels

            // VERSION A - proposed by Hirakawa & Parks, produces noticeable artifacts on cm_lighthouse.pgm
            float epsL = min(max(dl[0][0], dl[0][1]), max(dl[1][2], dl[1][3]));
            float epsC = min(max(dc[0][0], dc[0][1]), max(dc[1][2], dc[1][3]));

            // VERSION B - extended, produces less artifacts on cm_lighthouse.pgm, but instead produces weak zipper on test_debayer.pgm
            //float epsL = min(max(max(dl[0][0], dl[0][1]), max(dl[0][2], dl[0][3])), max(max(dl[1][0], dl[1][1]), max(dl[1][2], dl[1][3])));
            //float epsC = min(max(max(dc[0][0], dc[0][1]), max(dc[0][2], dc[0][3])), max(max(dc[1][0], dc[1][1]), max(dc[1][2], dc[1][3])));

            for (int d = 0; d < 2; d++)
            {
                int homo_val = 0;
                for (int j = 0; j < 4; j++)
                {
                    if (dl[d][j] <= epsL && dc[d][j] <= epsC)
                    {
                        homo_val++;
                    }
                }
                homo[d][offset] = homo_val;
            }
        }
    }

    int32_t *rgbDiff[4] = {
        new int32_t[mBayer->h * mBayer->w], // R - G
        new int32_t[mBayer->h * mBayer->w], // B - G
    };

    // select homogeneous pixels
    int homo_cur[2];
    for (int i = 0; i < mBayer->h; i++)
    {
        for (int j = 0; j < mBayer->w; j++)
        {
            int offset = i * mBayer->w + j;
            homo_cur[0] = homo_cur[1] = 0;
            if (i <= 1 || j <= 1 || i >= mBayer->h - 2 || j >= mBayer->w - 2)
            {
                result->element(i, j) = (rgb[0]->element(i, j) + rgb[1]->element(i, j)) / 2;
            }
            else
            {
                for (int k = i - 1; k <= i + 1; k++)
                    for (int l = j - 1; l <= j + 1; l++)
                    {
                        int offset_l = k * mBayer->w + l;
                        for (int d = 0; d < 2; d++)
                            homo_cur[d] += homo[d][offset_l];
                    }

                if (homo_cur[0] > homo_cur[1])
                    result->element(i, j) = rgb[0]->element(i, j);
                else if (homo_cur[0] < homo_cur[1])
                    result->element(i, j) = rgb[1]->element(i, j);
                else
                    result->element(i, j) = (rgb[0]->element(i, j) + rgb[1]->element(i, j)) / 2;
            }
            rgbDiff[0][offset] = result->element(i, j).r() - result->element(i, j).g();
            rgbDiff[1][offset] = result->element(i, j).b() - result->element(i, j).g();
        }
    }


    deletearr_safe(Lab[0]);
    deletearr_safe(Lab[1]);
    deletearr_safe(homo[0]);
    deletearr_safe(homo[1]);
    delete_safe(rgb[0]);
    delete_safe(rgb[1]);

    // apply median filter to rgb result
    // filter radius
    // radius of 1 gives nice results
    // radius of 2 gives less false color artifacts for some images, but the colors become somewhat degraded and blurred for other images
    const int radius = 1;
    // filter size - do not change
    const int size = (2 * radius + 1) * (2 * radius + 1);
    // median filter pass count, no difference except for running time observed between 1 and 2, more than 2 is redundant
    const int passes = 3;

    for (int p = 0; p < passes; p++)
    {
        for (int i = radius; i < mBayer->h - radius; i++)
        {
            for (int j = radius; j < mBayer->w - radius; j++)
            {
                int32_t window[2][size];
                int offset = i * mBayer->w + j;
                for (int c = 0; c < 2; c++)
                {
                    int idx = 0;
                    for (int k = i - radius; k <= i + radius; k++)
                        for (int l = j - radius; l <= j + radius; l++)
                            window[c][idx++] = rgbDiff[c][k * mBayer->w + l];
                    qsort(window[c], size, sizeof(window[c][0]), compare);
                }

                uint32_t r = window[0][4] + result->element(i, j).g();
                uint32_t b = window[1][4] + result->element(i, j).g();
                uint32_t g = (r + b - window[0][4] - window[1][4]) / 2;
                result->element(i, j) = RGBColor48(clip(r), clip(g), clip(b));

                rgbDiff[0][offset] = r - g;
                rgbDiff[1][offset] = b - g;
            }
        }
    }
    borderInterpolate(radius, result);

    deletearr_safe(rgbDiff[0]);
    deletearr_safe(rgbDiff[1]);
    deletearr_safe(rgbDiff[2]);
    deletearr_safe(rgbDiff[3]);
}

void Debayer::borderInterpolate(int radius, RGB48Buffer *result)
{
    // process border using Nearest Neighbor interpolation
    // for green color, linear interpolation was used
    uint32_t r = 0;
    uint32_t b = 0;
    uint32_t g = 0;

    // jPartial means interpolate cols [0 through R) and [W - R, W)
    int jPartial[4] = { 0, radius, mBayer->w - radius, mBayer->w };
    // jFull means interpolate cols [0 through W). -1 is for the condition to fail on second pass
    int jFull[4] = { 0, mBayer->w, 0, -1 };

    if (mBayer->h > 2 && mBayer->w > 2)
    {
        for (int i = 0; i < mBayer->h; i++)
        {
            int* jBounds;

            if (i >= radius && i < mBayer->h - radius)
                jBounds = jPartial;
            else
                jBounds = jFull;

            int j = 0;
            for (int pass = 0; pass < 2; pass++)
            {
                for (j = jBounds[2 * pass]; j < jBounds[2 * pass + 1]; j++)
                {
                    r = g = b = 0;
                    int c = colorFromBayerPos(i, j);
                    int g_div = 0;
                    switch (c)
                    {
                    case 0:
                        r = mBayer->element(i, j);
                        if (j < mBayer->w - 1)
                        {
                            g += mBayer->element(i, j + 1);
                            if (i < mBayer->h - 1)
                                b = mBayer->element(i + 1, j + 1);
                            else
                                b = mBayer->element(i - 1, j + 1);

                            g_div++;
                        }
                        else
                        {
                            g += mBayer->element(i, j - 1);
                            if (i < mBayer->h - 1)
                                b = mBayer->element(i + 1, j - 1);
                            else
                                b = mBayer->element(i - 1, j - 1);

                            g_div++;
                        }
                        g /= g_div;
                        break;

                    case 1:
                        g = mBayer->element(i, j);

                        if (j < mBayer->w - 1)
                        {
                            r = mBayer->element(i, j + 1);
                        }
                        if (j > 0)
                        {
                            r = mBayer->element(i, j - 1);
                        }

                        if (i < mBayer->h - 1)
                            b = mBayer->element(i + 1, j);
                        else
                            b = mBayer->element(i - 1, j);
                        break;

                    case 2:
                        g = mBayer->element(i, j);

                        if (j < mBayer->w - 1)
                        {
                            b = mBayer->element(i, j + 1);
                        }
                        if (j > 0)
                        {
                            b = mBayer->element(i, j - 1);
                        }

                        if (i < mBayer->h - 1)
                            r = mBayer->element(i + 1, j);
                        else
                            r = mBayer->element(i - 1, j);
                        break;

                    case 3:
                        b = mBayer->element(i, j);
                        if (j < mBayer->w - 1)
                        {
                            g += mBayer->element(i, j + 1);
                            if (i < mBayer->h - 1)
                                r = mBayer->element(i + 1, j + 1);
                            else
                                r = mBayer->element(i - 1, j + 1);
                            g_div++;
                        }
                        else
                        {
                            g += mBayer->element(i, j - 1);
                            if (i < mBayer->h - 1)
                                r = mBayer->element(i + 1, j - 1);
                            else
                                r = mBayer->element(i - 1, j - 1);
                            g_div++;
                        }
                        g /= g_div;
                        break;
                    }

                    result->element(i, j) = RGBColor48(r, g, b);
                }
            }
        }
    }

}

double F(int i, int j) {
    const double b = 4. / 9;
    const double a = 1. / 4;
    double f[4][4] = {
        {  224 + b,  84,     -25 + b, 2     },
        {  84,       26 + a, -14,     1 + a },
        { -25 + b,  -14,     -1 + b,  0     },
        {  2,        1 + a,   0,      a     },
    };

    if (i < 3)
        i = 3 - i;
    else
        i = i - 3;

    if (j < 3)
        j = 3 - j;
    else
        j = j - 3;
    
    return f[i][j];
}

void Debayer::getYChannel(AbstractBuffer<double, int> * output)
{
    const int sz = 3;
    const double divisor = 464.0;

    double sum;
    for (int i = 3; i < mBayer->h - 3; i++)
    {
        for (int j = 3; j < mBayer->w - 3; j++)
        {
            sum = 0;
            for (int k = -sz; k <= sz; k++)
            {
                for (int l = -sz; l <= sz; l++)
                {
                    sum += mBayer->element(i + k, j + l) * F(k + 3, l + 3);
                }
            }

            output->element(i, j) = sum / divisor;
        }
    }
}

void Debayer::fourier(RGB48Buffer *result)
{
#ifndef WITH_FFTW
    CORE_UNUSED(result);
    SYNC_PRINT(("FFT-based demosaicing is not supported by this version of debayer, consider using AHD instead."));
#else
    // this method is for research and test purposes only
    uint h = mBayer->h;
    uint w = mBayer->w;

    fftw_complex* in_r, *in_g, *in_b, *out_r, *out_g, *out_b;

    in_r = new fftw_complex[h * w];
    in_g = new fftw_complex[h * w];
    in_b = new fftw_complex[h * w];

    memset(in_r, 0, h*w*sizeof(fftw_complex));
    memset(in_g, 0, h*w*sizeof(fftw_complex));
    memset(in_b, 0, h*w*sizeof(fftw_complex));

    out_r = new fftw_complex[h * w];
    out_g = new fftw_complex[h * w];
    out_b = new fftw_complex[h * w];

    for (uint i = 0; i < h; i++)
        for (uint j = 0; j < w; j++)
        {
            uint offset = i * w + j;
            uint c = colorFromBayerPos(i, j, false);
            if (c == 0)
                in_r[offset][0] = mBayer->element(i, j);
            if (c == 1)
                in_g[offset][0] = mBayer->element(i, j);
            if (c == 2)
                in_b[offset][0] = mBayer->element(i, j);
            in_r[offset][1] = in_g[offset][1] = in_b[offset][1] = 0;
        }

    FFTW fftw;

    fftw.transform2D(h, w, in_r, out_r, FFTW::Forward);
    fftw.transform2D(h, w, in_g, out_g, FFTW::Forward);
    fftw.transform2D(h, w, in_b, out_b, FFTW::Forward);

    double coeff = 5.1 / 12;
    for (int i = 0; (uint)i < h; i++)
        for (int j = 0; (uint)j < w; j++)
        {
            int disty = abs(i - (int)h / 2);
            int distx = abs(j - (int)w / 2);

            int rad = 1000;

            bool sphere1 = (pow(i, 2) + pow(j, 2)) < rad;       // TODO: speed up this as pow() works via double!!!
            bool sphere2 = (pow(i, 2) + pow(w - j, 2)) < rad;
            bool sphere3 = (pow(h - i, 2) + pow(j, 2)) < rad;
            bool sphere4 = (pow(h - i, 2) + pow(w - j, 2)) < rad;

            bool sphere5 = (pow(h / 2 - i, 2) + pow(j, 2)) < rad;
            bool sphere6 = (pow(i, 2) + pow(w / 2 - j, 2)) < rad;
            bool sphere7 = (pow(h - i, 2) + pow(w / 2 - j, 2)) < rad;
            bool sphere8 = (pow(h / 2 - i, 2) + pow(w - j, 2)) < rad;

            double mul = 1.0 / (h*w);
            if (distx > w * coeff || disty > h * coeff || sphere1 || sphere2 || sphere3 || sphere4 || sphere5 || sphere6 || sphere7 || sphere8)
            {
                mul = 0;
            }
            out_r[i*w + j][0] *= mul;
            out_r[i*w + j][1] *= mul;

            out_g[i*w + j][0] *= mul;
            out_g[i*w + j][1] *= mul;

            out_b[i*w + j][0] *= mul;
            out_b[i*w + j][1] *= mul;
        }
    
    fftw.transform2D(h, w, out_r, in_r, FFTW::Backward);
    fftw.transform2D(h, w, out_g, in_g, FFTW::Backward);
    fftw.transform2D(h, w, out_b, in_b, FFTW::Backward);

    double *rgbDiff[2] = {
        new double[h*w],
        new double[h*w]
    };

    double white = 0;
    double *val_r = new double[h * w],
        *val_g = new double[h * w],
        *val_b = new double[h * w];

    for (uint i = 0; i < h; i++)
        for (uint j = 0; j < w; j++)
        {
            uint offset = i * w + j;
            val_r[offset] = abs(in_r[offset][0]) + abs(in_r[offset][1]);
            val_g[offset] = abs(in_g[offset][0]) + abs(in_g[offset][1]);
            val_b[offset] = abs(in_b[offset][0]) + abs(in_b[offset][1]);
            rgbDiff[0][offset] = val_r[offset] - val_g[offset];
            rgbDiff[1][offset] = val_b[offset] - val_g[offset];
        }

    int32_t window[25];
    // calc median maximum
    for (uint i = 2; i < h - 2; i++)
        for (uint j = 2; j < w - 2; j++)
        {
            uint idx = 0;
            for (uint k = i - 2; k <= i + 2; k++)
                for (uint l = j - 2; l <= j + 2; l++)
                {
                    uint offset2 = k * w + l;
                    window[idx++] = max(val_r[offset2], max(val_g[offset2] / 2, val_b[offset2]));
                }
            qsort(window, 25, sizeof(window[0]), compare);
            if (white < window[12])
                white = window[12];

        }

    double ampl = 1 * 255.0 / white;
    for (uint i = 0; i < h; i++)
        for (uint j = 0; j < w; j++)
        {
            int offset = i * w + j;
            result->element(i, j) = {
                clip(val_r[offset] * ampl),
                uint16_t(clip(val_g[offset] * ampl / 2)),
                clip(val_b[offset] * ampl),
            };
        }

    deletearr_safe(in_r);
    deletearr_safe(in_g);
    deletearr_safe(in_b);

    deletearr_safe(in_r);
    deletearr_safe(in_g);
    deletearr_safe(in_b);

    deletearr_safe(out_r);
    deletearr_safe(out_g);
    deletearr_safe(out_b);

    deletearr_safe(val_r);
    deletearr_safe(val_g);
    deletearr_safe(val_b);

    deletearr_safe(rgbDiff[0]);
    deletearr_safe(rgbDiff[1]);

#endif // WITH_FFTW
}

void Debayer::scaleCoeffs()
{
    for (int i = 0; i < 3; i++)
        mScaleMul[i] = 1;

    // check if metadata available
    if (mMetadata == nullptr || mMetadata->empty())
        return;

    // alias for ease of use
    MetaData &metadata = *mMetadata;
    
    // scale colors to the desired bit-depth
    double factor = 1.0;
    if (mScale)
    {
        if (!metadata["white"].empty())
            factor = mMaximum / metadata["white"][0];
        else if (!metadata["bits"].empty())
            factor = mMaximum / ((1 << int(metadata["bits"][0])) - 1);
    }

    // check if metadata valid
    if ((metadata["cam_mul"].empty() || metadata["cam_mul"][0] == 0 || metadata["cam_mul"][2] == 0) &&
        (metadata["pre_mul"].empty() || metadata["pre_mul"][0] == 0 || metadata["pre_mul"][2] == 0))
    {
        // if white balance is not available, do only scaling
        // white should be calculated before calling scaleCoeffs()
        for (int i = 0; i < 3; i++) {
            mScaleMul[i] = factor;
        }

        // TODO: maybe apply the gray world hypothesis instead of doing nothing
        return;
    }

    unsigned c;
    double dmin;

    // if cam_mul coefficients are available directly, use them in future
    // if not, use pre_mul when available

    if (metadata["pre_mul"].empty())
        metadata["pre_mul"] = MetaValue(4, 1.0);

    if (!metadata["cam_mul"].empty() && metadata["cam_mul"][0] != -1)
    {
        for (int i = 0; i < 3; i++)
            metadata["pre_mul"][i] = metadata["cam_mul"][i];
    }
    // if green scale coefficient is not available, set it to 1
    if (metadata["pre_mul"][1] == 0)
        metadata["pre_mul"][1] = 1;

    // black must be subtracted from the image, so we adjust maximum white level here
    if (!metadata["white"].empty() && !metadata["black"].empty())
        metadata["white"][0] -= metadata["black"][0];

    // normalize pre_mul
    for (dmin = std::numeric_limits<double>::max(), c = 0; c < 3; c++)
    {
        if (dmin > metadata["pre_mul"][c])
            dmin = metadata["pre_mul"][c];
    }

    for (int c = 0; c < 3; c++)
        mScaleMul[c] = (metadata["pre_mul"][c] /= dmin) * factor;

    // black frame adjustment, not currently used as we don't have the black frame in metadata (use black level instead)
    // the black level is usually almost equal for all channels with difference less than 1 bit in 12-bit case
    // TODO: perform tests and prove the previous statement right or wrong (and maybe start using 3-channel black level)
    /*
    if (metadata->filters > 1000 && (metadata->cblack[4] + 1) / 2 == 1 && (metadata->cblack[5] + 1) / 2 == 1)
    {
        for (int c = 0; c < 4; c++)
        metadata->cblack[FC(c / 2, c % 2, metadata->filters)] +=
        metadata->cblack[6 + c / 2 % cblack[4] * cblack[5] + c % 2 % cblack[5]];
        metadata->cblack[4] = metadata->cblack[5] = 0;
    }*/
}

void Debayer::gammaCurve(uint16_t *curve, int imax)
{
    // this code is taken from LibRaw
    // TODO: rewrite it?

    // initialize curve as a straight line (no correction)
    for (int i = 0; i < 0x10000; i++)
        curve[i] = i;

    if (mMetadata == nullptr)
        return;

    // if no gamma coefficients are present (or valid), return no transform
    MetaValue& gammData = (*mMetadata)["gamm"];
    if (gammData.empty() ||
        ((gammData.size() == 5) &&
            !(gammData[0] == 0.0 ||
              gammData[1] == 0.0 ||
              gammData[2] == 0.0 ||
              gammData[3] == 0.0 ||
              gammData[4] == 0.0 ))) {
        return;
    }

    int i;
    double r, g[6], bnd[2] = { 0, 0 };

    g[0] = gammData[0];
    g[1] = gammData[1];
    g[2] = g[3] = g[4] = 0;
    bnd[g[1] >= 1] = 1;

    if (g[1] && (g[1] - 1) * (g[0] - 1) <= 0)
    {
        for (i = 0; i < 48; i++)
        {
            g[2] = (bnd[0] + bnd[1]) / 2;
            if (g[0])
                bnd[(pow(g[2] / g[1], -g[0]) - 1) / g[0] - 1 / g[2] > -1] = g[2];
            else
                bnd[g[2] / exp(1 - 1 / g[2]) < g[1]] = g[2];
        }
        g[3] = g[2] / g[1];
        if (g[0])
            g[4] = g[2] * (1 / g[0] - 1);
    }
    if (g[0])
        g[5] = 1 / (g[1] * (g[3] * g[3]) / 2 - g[4] * (1 - g[3]) + (1 - pow(g[3], 1 + g[0]))*(1 + g[4]) / (1 + g[0])) - 1;
    else
        g[5] = 1 / (g[1] * (g[3] * g[3]) / 2 + 1 - g[2] - g[3] - g[2] * g[3] * (log(g[3]) - 1)) - 1;

    for (i = 0; i < 0x10000; i++)
    {
        curve[i] = mMaximum;
        if ((r = (double)i / imax) < 1)
            curve[i] = (1 << mDepth) * (r < g[3] ? r * g[1] : (g[0] ? pow(r, g[0]) * (1 + g[4]) - g[4] : log(r) * g[2] + 1));
    }
}

int Debayer::toRGB48(DebayerMethod::DebayerMethod method, RGB48Buffer *output)
{
    preprocess();

    if (output == nullptr)
        return -1;

    switch (method)
    {
        case DebayerMethod::NEAREST:   nearest(output); break;
        default:
        case DebayerMethod::BILINEAR:  linear(output);  break;
        case DebayerMethod::AHD:       ahd(output);     break;
        case DebayerMethod::FOURIER:   fourier(output); break;
    }
    return 0;
}

int Debayer::toRGB24(DebayerMethod::DebayerMethod method, RGB24Buffer *out)
{
    std::unique_ptr<RGB48Buffer> outputDraft(new RGB48Buffer(out->getSize(), false));
    int result = toRGB48(method, outputDraft.get());

    cint shift = mDepth - 8;        // 16: 8, 12: 4, 10: 2
    ConvertRgb48toRgb24(outputDraft.get(), out, shift);

    return result;
}

void Debayer::preprocess(bool overwrite)
{
    // recalculate white balance coefficients
    scaleCoeffs();

    int white = 0;

    if (mMetadata != nullptr && !mMetadata->empty() && (overwrite || mCurve == nullptr))
    {
        MetaData &md = *mMetadata;
        int bits  = md["bits"][0] ? md["bits"][0] : mDepth;
        int shift = bits - mDepth;

        mBlack = !md["black"  ].empty() && md["black"  ][0] ?      md["black"  ][0] : 0;
        white  = !md["white"  ].empty() && md["white"  ][0] ?      md["white"  ][0] : mMaximum;
        white  = !md["t_white"].empty() && md["t_white"][0] ? (int)md["t_white"][0] : white;
        white  = (shift < 0 ? white << -shift : white >> shift);
    }

    // (re)calculate gamma correction
    if (overwrite || mCurve == nullptr) {
        deletearr_safe(mCurve);
        mCurve = new uint16_t[0x10000];
        gammaCurve(mCurve, white);
    }
}

inline int32_t Debayer::weightedBayerAvg(const Vector2d32& coords)
{
    int32_t x = coords.x(),
            y = coords.y();
    if (x < 0)
        x = -x;

    if (y < 0)
        y = -y;

    if (x >= mBayer->w)
    {
        x = 2 * mBayer->w - coords.x() - 1;
    }

    if (y >= mBayer->h)
    {
        y = 2 * mBayer->h - y - 1;
    }
    
    if (x >= 0 && y >= 0 && x < mBayer->w && y < mBayer->h)
        return mBayer->element(y, x);
    else
        return 0;
}

inline int32_t Debayer::weightedBayerAvg(const vector<Vector2d32>& coords, const vector<int>& coeffs)
{
    int32_t result = 0;
    uint16_t div = 0;

    bool useCoeffs = coeffs.size() >= coords.size();

    for (size_t i = 0; i < coords.size(); i++)
    {
        if (coords[i].x() >= 0 && coords[i].y() >= 0 && coords[i].x() < mBayer->w && coords[i].y() < mBayer->h)
        {
            div++;
            if (useCoeffs)
                result += weightedBayerAvg(coords[i]) * coeffs[i];
            else
                result += weightedBayerAvg(coords[i]);
        }
    }

    return (div == 0) ? 0 : result / div;
}

uint8_t Debayer::colorFromBayerPos(uint i, uint j, bool rggb)
{
    if (rggb)   // r, g1, g2, b
        return   ((j ^ (mBayerPos & 1)) & 1) | (((i ^ ((mBayerPos & 2) >> 1)) & 1) << 1);
    else        // r, g, b
        return ((((j ^ (mBayerPos & 1)) & 1) | (((i ^ ((mBayerPos & 2) >> 1)) & 1) << 1)) + 1) >> 1;
}

void Debayer::fromRgb(RGB48Buffer *inRgb)
{
    if (inRgb->h != mBayer->h || inRgb->w != mBayer->w)
        return;

    for (int i = 0; i < mBayer->h; i++)
    {
        for (int j = 0; j < mBayer->w; j++)
        {
            uint8_t c = colorFromBayerPos(i, j, false);
            mBayer->element(i, j) = inRgb->element(i, j)[2 - c];
        }
    }
}

} //namespace corecvs
