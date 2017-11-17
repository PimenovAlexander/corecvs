#ifndef CIRCLEPATTERNGENERATOR
#define CIRCLEPATTERNGENERATOR

#include <vector>
#include <array>
#include <algorithm>
#include <unordered_map>

#include "core/buffers/abstractBuffer.h"
#include "core/math/vector/vector2d.h"
#include "core/math/matrix/matrix33.h"
#include "core/buffers/convolver/convolver.h"    // corecvs::DpImage

#ifdef WITH_TBB
#include <tbb/reader_writer_lock.h>
#endif

using corecvs::DpImage;


namespace std
{
    template<>
    struct hash<std::array<corecvs::Vector2dd, 4>>
    {
        size_t operator() (const std::array<corecvs::Vector2dd, 4>& x) const
        {
            return
                std::hash<double>() (x[0][0]) ^
               (std::hash<double>() (x[0][1]) * 3) ^
               (std::hash<double>() (x[1][0]) * 7) ^
               (std::hash<double>() (x[1][1]) * 31) ^
               (std::hash<double>() (x[2][0]) * 127) ^
               (std::hash<double>() (x[2][1]) * 8191) ^
               (std::hash<double>() (x[3][0]) * 131071) ^
               (std::hash<double>() (x[3][1]) * 524287);
        }
    };
}

struct CirclePatternGeneratorParams
{
    int     patternSize     = 128;
    double  stdevThreshold  = 0.05;
    double  corrThreshold   = 0.5;
};

class CirclePatternGenerator : public CirclePatternGeneratorParams
{
public:
    CirclePatternGenerator(CirclePatternGeneratorParams params = CirclePatternGeneratorParams());

    void    addToken(int token, double r, const std::vector<corecvs::Vector2dd> &centers);
    int     getBestToken(DpImage &query, double &score, corecvs::Matrix33 &orientation) const;
    int     getBestToken(const DpImage &query, const std::array<corecvs::Vector2dd, 4> &cell, double &score, corecvs::Matrix33 &orientation, corecvs::Matrix33 &homography) const;

    void flushCache();

    std::vector<std::vector<DpImage>> patterns;
    std::vector<int>                  tokens;

protected:
    mutable std::unordered_map<std::array<corecvs::Vector2dd, 4>, std::tuple<double, corecvs::Matrix33, corecvs::Matrix33, int>> cache;
    bool inCache(const std::array<corecvs::Vector2dd, 4> &k, double &score, corecvs::Matrix33 &orientation, corecvs::Matrix33 &homography, int &token) const;
    void putCache(const std::array<corecvs::Vector2dd, 4> &k, const std::tuple<double, corecvs::Matrix33, corecvs::Matrix33, int> &v) const;

#ifdef WITH_TBB
    mutable tbb::reader_writer_lock rw_lock;
#endif

    void studentize(DpImage &image) const;
    void studentize(DpImage &image, double &mean, double &stdev) const;
    void createFlip(DpImage &source, DpImage &destination, int rotation, bool flip);
    corecvs::Matrix33 getFlipMatrix(int rotation, bool flip) const;
};

#endif
