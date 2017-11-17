#include "core/patterndetection/circlePatternGenerator.h"
#include "core/math/matrix/homographyReconstructor.h"
#include "core/math/mathUtils.h"

using corecvs::Vector3dd;
using corecvs::Vector2dd;
using corecvs::Matrix33;


CirclePatternGenerator::CirclePatternGenerator(CirclePatternGeneratorParams params) : CirclePatternGeneratorParams(params)
{
}

void CirclePatternGenerator::studentize(DpImage &image) const
{
    double mean, stdev;
    studentize(image, mean, stdev);
}

void CirclePatternGenerator::studentize(DpImage &image, double &mean, double &stdev) const
{
    double s = 0.0, ssq = 0.0, N = image.w * image.h;
    image.touchOperationElementwize([&](int, int, double v) { s += v; ssq += v * v; });
    s /= N;
    ssq = std::sqrt(ssq / N - s * s);
    mean = s;
    stdev = ssq;
    if (stdev > 0.0)
        image.mapOperationElementwize([&](double v) { return (v - mean) / stdev; });
    else
        image.mapOperationElementwize([&](double) { return 0.0; });
}

void CirclePatternGenerator::addToken(int token, double r, const std::vector<corecvs::Vector2dd> &centers)
{
    r = r * patternSize;
    DpImage pattern(patternSize, patternSize);
    for (auto& c: centers)
    {
        corecvs::Vector2dd csc = c * (1.0 * patternSize);
        int _t = 0;
        int _d = patternSize-1;
        int _l = 0;
        int _r = patternSize-1;

        for (int y = _t; y <= _d; ++y)
        {
            for (int x = _l; x <= _r; ++x)
            {
                double dx = x - csc[0], dy = y - csc[1];
                if ((dx * dx + dy * dy) < r * r)
                    pattern.element(y, x) = 1.0;
            }
        }
    }
    studentize(pattern);
    
    std::vector<DpImage> flips;
    flips.resize(8);
    flips[0] = pattern;
    for (int i = 0; i < 8; ++i)
    {
        createFlip(pattern, flips[i], i & 3, (i >> 2) != 0);
    }
    patterns.emplace_back(std::move(flips));
    tokens.push_back(token);
}

inline int convert(double d)
{
    return corecvs::roundSign(d);
}

void CirclePatternGenerator::createFlip(DpImage &source, DpImage &destination, int rotation, bool flip)
{
    destination = DpImage(patternSize, patternSize);
    Matrix33 tform = getFlipMatrix(rotation, flip);

    corecvs::Vector3dd dx(1.0, 0.0, 1.0), dy(0.0, 1.0, 1.0);
    corecvs::Vector3dd dxc = tform * dx, dyc = tform * dy, oc = tform * corecvs::Vector3dd(0.0, 0.0, 1.0);
    dxc /= dxc[2]; dyc /= dyc[2]; oc /= oc[2];
    dxc -= oc; dyc -= oc;

    CORE_ASSERT_TRUE_S(-1.5 < dxc[0] && dxc[0] < 1.5);
    CORE_ASSERT_TRUE_S(-1.5 < dxc[1] && dxc[1] < 1.5);
    CORE_ASSERT_TRUE_S(-1.5 < dyc[0] && dyc[0] < 1.5);
    CORE_ASSERT_TRUE_S(-1.5 < dyc[1] && dyc[1] < 1.5);
    CORE_ASSERT_TRUE_S(-1.5 <  oc[0] && oc[0] < 1.5);
    CORE_ASSERT_TRUE_S(-1.5 <  oc[1] && oc[1] < 1.5);
    int xdx = convert(dxc[0]), xdy = convert(dyc[0]), ydx = convert(dxc[1]), ydy = convert(dyc[1]);

    int xa = convert(oc[0]), ya = convert(oc[1]);
    xa = xa * (patternSize - 1); ya = ya * (patternSize - 1);

    for (int i = 0; i < patternSize; ++i)
    {
        for (int j = 0; j < patternSize; ++j)
        {
            int x = xa + xdx * j + xdy * i;
            int y = ya + ydx * j + ydy * i;
            CORE_ASSERT_TRUE_S(y >= 0 && x >= 0 && x < patternSize && y < patternSize);
            destination.element(y, x) = source.element(i, j);
        }
    }
}

int CirclePatternGenerator::getBestToken(DpImage &query, double &score, Matrix33 &orientation) const
{
    double s = 0.0, ssq = 0.0;
    CORE_ASSERT_TRUE_S(query.w == patternSize && query.h == patternSize);
    studentize(query, s, ssq);
    
    double maxC = 0.0;
    double N = patternSize * patternSize;
    int maxIdx = -1;
    int maxRI = -1;
//    std::cout << "Selecting 1 from " << patterns.size() << " patterns. (stdev: " << ssq  << ", mean = " << s << ")" << std::endl;
    if (ssq > stdevThreshold)
    {
        maxIdx = -10;
        for (size_t id = 0; id < patterns.size(); id++)
        {
            for (int idp = 0; idp < 8; ++idp)
            {
                double corscore = 0.0;
                for (int y = 0; y < patternSize; ++y)
                    for (int x = 0; x < patternSize; ++x)
                    {
                        corscore += patterns[id][idp].element(y, x) * query.element(y, x);
                    }
                corscore /= N;
//                std::cout << corscore << std::endl;
                if (corscore < maxC && -corscore > corrThreshold)
                {
                    maxC   = corscore;
                    maxIdx = (int)id;
                    maxRI  = idp;
                }
            }
        }
    }
    orientation = getFlipMatrix(maxRI & 3, (maxRI & 4) != 0);
    score = maxC;
    return maxIdx;
}

void CirclePatternGenerator::flushCache()
{
#ifdef WITH_TBB
    tbb::reader_writer_lock::scoped_lock writelock(rw_lock);
#endif
    cache.clear();
}

void CirclePatternGenerator::putCache(const std::array<Vector2dd, 4> &k, const std::tuple<double, Matrix33, Matrix33, int> &v) const
{
#ifdef WITH_TBB
    tbb::reader_writer_lock::scoped_lock writelock(rw_lock);
#endif
    cache[k] = v;
}

bool CirclePatternGenerator::inCache(const std::array<Vector2dd, 4> &k, double &score, Matrix33 &orientation, Matrix33 &homography, int &token) const
{
#ifdef WITH_TBB
    tbb::reader_writer_lock::scoped_lock_read readlock(rw_lock);
#endif
    if (!cache.count(k))
        return false;
    auto t = cache[k];
    score = std::get<0>(t);
    orientation = std::get<1>(t);
    homography = std::get<2>(t);
    token = std::get<3>(t);
    return true;
}

int CirclePatternGenerator::getBestToken(const DpImage &image, const std::array<corecvs::Vector2dd, 4> &cell, double &score, Matrix33 &orientation, Matrix33 &homography) const
{
    int token;
    if (inCache(cell, score, orientation, homography, token))
        return token;

    corecvs::Vector2dd A = cell[0], B = cell[1], C = cell[2], D = cell[3];

    corecvs::HomographyReconstructor c2i;
    c2i.addPoint2PointConstraint(corecvs::Vector2dd(0.0, 0.0), A);
    c2i.addPoint2PointConstraint(corecvs::Vector2dd(1.0, 0.0), B);
    c2i.addPoint2PointConstraint(corecvs::Vector2dd(0.0, 1.0), C);
    c2i.addPoint2PointConstraint(corecvs::Vector2dd(1.0, 1.0), D);

    Matrix33 AA, BB;
    c2i.normalisePoints(AA, BB);

    homography = c2i.getBestHomographyLSE();
//    homography = c2i.getBestHomographyLM(homography);
    homography = BB.inv() * homography * AA;

    DpImage query(patternSize, patternSize);
    for (int y = 0; y < patternSize; ++y)
    {
        for (int x = 0; x < patternSize; ++x)
        {
            Vector3dd pt = homography * Vector3dd(x * (1.0 / patternSize), y * (1.0 / patternSize), 1.0);
            pt = pt * (1.0 / pt[2]);
            int xx = (int)pt[0], yy = (int)pt[1];
            if (xx >= 0 && xx < image.w && yy >= 0 && yy < image.h)
            {
                // TODO: maybe we may benefit from some interpolation here
                query.element(y, x) = image.element(yy, xx);
            }
        }
    }

    token = getBestToken(query, score, orientation);
    putCache(cell, std::make_tuple(score, orientation, homography, token));
    return token;
}

Matrix33 CirclePatternGenerator::getFlipMatrix(int rotation, bool flip) const
{
    // flip = transpose
    // rotation = 0..3 * pi/2 => all matrix entries should be integer
    
    Matrix33 flipMatrix = flip ? Matrix33(0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0) : Matrix33(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
    double phi = M_PI / 2.0 * rotation;
    Matrix33 rotationMatrix = Matrix33(cos(phi), -sin(phi), 0.0, sin(phi), cos(phi), 0.0, 0.0, 0.0, 1.0);
    Matrix33 shiftMatrix = Matrix33(1.0, 0.0, -0.5, 0.0, 1.0, -0.5, 0.0, 0.0, 1.0);
    Matrix33 unShiftMatrix = Matrix33(1.0, 0.0, 0.5, 0.0, 1.0, 0.5, 0.0, 0.0, 1.0);
    
    Matrix33 res = unShiftMatrix * rotationMatrix * shiftMatrix * flipMatrix;
    Matrix33 ss = res;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            double a = std::abs(res.a(i, j));
            int ra = a + 0.5;
            res.a(i, j) = res.a(i, j) > 0.0 ? ra : -ra;
        }
    }
    double diff = 0.0;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            double d = res.a(i, j) - ss.a(i, j);
            diff += d * d;
        }
    }
//    std::cout << "DIFF: " << diff << std::endl;
//    std::cout << "FRM: " << std::endl << res << std::endl;
    return res;
}
