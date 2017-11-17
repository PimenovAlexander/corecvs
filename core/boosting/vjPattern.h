/**
 * \file vjPattern.h
 * \brief This file holds data for Viola-Jones pattern features.
 *
 * \ingroup cppcorefiles
 * \date Jun 22, 2010
 * \author alexander
 */

#ifndef VJPATTERN_H_
#define VJPATTERN_H_

#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <limits>

#include "core/utils/global.h"

#include "core/tbbwrapper/tbbWrapper.h"
#include "core/math/mathUtils.h"
#include "core/buffers/integralBuffer.h"
#include "core/math/vector/vector3d.h"
#include "core/math/vector/vector2d.h"
#include "core/buffers/mipmapPyramid.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/boosting/adaBoost.h"

using std::vector;

namespace corecvs {

/**
 *  The data that is needed for fast classification
 *
 **/
class VJInputImage
{
public:
    G12Buffer *buffer;
    G12IntegralBuffer *integral;
    VJInputImage (G12Buffer * _buffer, G12IntegralBuffer *_integral) :
        buffer(_buffer), integral(_integral){};
};

/**
 * A part of the pattern
 **/
class PatternCorner
{
public:
    Vector2d32 point;
    int multiplier;

    /* TODO: don't use this syntax */
    PatternCorner(Vector2d32 _point, int _multiplier) : point(_point), multiplier(_multiplier) {};
    PatternCorner(int x, int y, int _multiplier) : point(Vector2d32(x,y)), multiplier(_multiplier) {};

};

/**
 * Viola-Jones generic Haar wavelet
 **/
class VJPattern
{
public:
    vector<PatternCorner> points;
    VJPattern();
    virtual ~VJPattern();

    int applyToPoint(G12IntegralBuffer *buffer, int y, int x)
    {
        int sum = 0;
        for (unsigned i = 0; i < points.size(); i++)
        {
            PatternCorner *pc = &(points[i]);
            sum += pc->multiplier * buffer->element(pc->point + Vector2d32(x,y));
        }
        return sum;
    }

    void addPoint(const PatternCorner &corner)
    {
        vector<PatternCorner>::iterator it;
        for (it = points.begin(); it != points.end(); it++)
        {
            if ((*it).point == corner.point)
            {
                (*it).multiplier += corner.multiplier;
                return;
            }
        }
        points.push_back(corner);
    }

    /*TODO: Make y come first like everywhere*/
    void addRectangle(int x1, int y1, int x2, int y2, int multipiler)
    {
        int tmp;
        if (x1 > x2) {tmp = x1; x1 = x2; x2 = tmp;};
        if (y1 > y2) {tmp = y1; y1 = y2; y2 = tmp;};
        addPoint(PatternCorner(x2, y2,  multipiler));
        addPoint(PatternCorner(x1, y1,  multipiler));
        addPoint(PatternCorner(x2, y1, -multipiler));
        addPoint(PatternCorner(x1, y2, -multipiler));
    }


    friend ostream& operator << (ostream &out, VJPattern &toSave)
    {
        //const vector<PatternCorner>::iterator it;
        out << toSave.points.size() << "\n";
        for (unsigned i = 0; i <= toSave.points.size(); i++)
        {
            PatternCorner *corner = &(toSave.points[i]);
            out << corner->point.x() << " "<< corner->point.y() << "  " << corner->multiplier << "\n";
        }
        return out;
    }

    void print ()
    {
        cout << this;
    }

    friend istream & operator >>  (istream &in, VJPattern &toLoad)
    {
        toLoad.points.clear();
        unsigned int size;
        in >> size;
        for (unsigned i = 0; i < size; i++)
        {
            int x;
            int y;
            int mul;
            in >> x >> y >> mul;
            toLoad.addPoint(PatternCorner(x,y,mul));
        }
        return in;
    }

    VJPattern *scale(double scale)
    {
        VJPattern *toReturn = new VJPattern();
        /**
         * TODO: A correction should be applied.
         **/
        for (unsigned i = 0; i < points.size(); i++)
        {
            PatternCorner *oldCorner = &(points.at(i));
            int x = fround(oldCorner->point.x() * scale);
            int y = fround(oldCorner->point.y() * scale);
            PatternCorner corner(Vector2d32(x,y), oldCorner->multiplier);
            toReturn->points.push_back(corner);
        }

        return toReturn;
    }
};

class VJSimpleClassifier : public Classifier<VJInputImage>
{
public:
    VJPattern *pattern;
    int thershold;
    int leftMargin;
    int topMargin;
    int bottomMargin;
    int rightMargin;

    VJSimpleClassifier()
    {
        memset(this, 0 , sizeof (VJSimpleClassifier));
    }


    VJSimpleClassifier(
                VJPattern *pattern,
                int _thersthod,
                int _leftMargin,
                int _topMargin,
                int _bottonMargin,
                int _rightMargin ) :
            pattern(pattern),
            thershold(_thersthod),
            leftMargin(_leftMargin),
            topMargin(_topMargin),
            bottomMargin(_bottonMargin),
            rightMargin(_rightMargin)
    {

    }

    bool classify (const VJInputImage &input)
    {
        return applyToPoint(input.integral, 0, 0);
    }

    bool applyToPoint(G12IntegralBuffer *buffer, int y, int x)
    {
        if (y < topMargin  || y > buffer->h - bottomMargin ||
            x < leftMargin || x > buffer->w - rightMargin)
        {
            /** In fact in this case should return something more intelligent*/
                return false;
        }
        int result = pattern->applyToPoint(buffer, y, x);
        if (result > thershold)
            return true;
        return false;
    }

    friend ostream& operator << (ostream &out, VJSimpleClassifier &toSave)
    {
        out << toSave.leftMargin << " " << toSave.topMargin << " ";
        out << toSave.rightMargin << " " << toSave.bottomMargin << "\n";
        out << toSave.thershold << "\n";
        out << toSave.pattern << "\n";
        return out;
    }

    void print ()
    {
        cout << this;
    }


    friend istream & operator >>  (istream &in, VJSimpleClassifier &toLoad)
    {
        in >> toLoad.leftMargin >> toLoad.topMargin;
        in >> toLoad.rightMargin >> toLoad.bottomMargin;
        in >> toLoad.thershold;
        toLoad.pattern = new VJPattern();
        in >> (*toLoad.pattern);
        return in;
    }

    RGB24Buffer *drawPattern ()
    {
        int h = bottomMargin - topMargin;
        int w = rightMargin - leftMargin;
        G12Buffer *canvas = new G12Buffer(h, w, false);
        canvas->fillWith(G12Buffer::BUFFER_MAX_VALUE >> 2);

        for (unsigned l = 0; l < pattern->points.size(); l++)
        {
            PatternCorner *point = &(pattern->points.at(l));
            for (int i = 0; i < point->point.y() + topMargin; i++)
            {
                for (int j = 0; j < point->point.x() + leftMargin; j++)
                {
                    canvas->element(i,j) += point->multiplier * 256 * 2;
                }
            }
        }


        return new RGB24Buffer(canvas);

    }

    VJSimpleClassifier *scale(double scale)
    {
        VJSimpleClassifier *toReturn = new VJSimpleClassifier();
        toReturn->thershold    = fround(thershold * scale * scale);
        toReturn->leftMargin   = fround(leftMargin * scale);
        toReturn->topMargin    = fround(leftMargin * scale);
        toReturn->bottomMargin = fround(leftMargin * scale);
        toReturn->rightMargin  = fround(leftMargin * scale);
        toReturn->pattern = pattern->scale(scale);
        return toReturn;
    }

};



/**
 *   Base class for pattern generator
 *
 **/
class VJPatternGenerator
{
public:
    int imageH;
    int imageW;
    int granule;

    VJPatternGenerator(int _imageH, int _imageW)
    {
        this->imageH = _imageH;
        this->imageW = _imageW;
        granule = 4;
    }

    virtual VJPattern *pattern(){return NULL;};
    virtual ~VJPatternGenerator(){};
};

/**
 *  HorizontalPattern2
 *
 *  generates a random pattern of the form below
 *
 *  /code
 *     +-----+-----+
 *     |     |     |
 *     |     |     |
 *     | -1  |  1  |
 *     |     |     |
 *     |     |     |
 *     +-----+-----+
 *  /code
 *
 **/
class HorizontalPattern2 : public VJPatternGenerator
{
public:
    HorizontalPattern2(int _imageH, int _imageW) : VJPatternGenerator(_imageH, _imageW) {};

    virtual VJPattern *pattern()
    {
        VJPattern *result = new VJPattern();
        int sizeH = rand() % ((imageH - granule)) + granule;
        int sizeW = rand() % ((imageW - granule)) + granule;
        int x = rand() % (imageW - sizeW);
        int y = rand() % (imageH - sizeH);

        if (sizeW % 2) sizeW -= 1;
        int v = (rand() % 2) ? -1 : 1;
        result->addRectangle(x            , y, x + sizeW / 2, y + sizeH, v);
        result->addRectangle(x + sizeW / 2, y, x + sizeW    , y + sizeH, -v);
        return result;
    }
};

/**
 *  HorizontalPattern3
 *
 *  generates a random pattern of the form below
 *
 *  /code
 *     +-----+-----+-----+
 *     |     |     |     |
 *     |     |     |     |
 *     | -1  |  2  | -1  |
 *     |     |     |     |
 *     |     |     |     |
 *     +-----+-----+-----+
 *  /code
 *
 **/
class HorizontalPattern3 : public VJPatternGenerator
{
public:
    HorizontalPattern3(int _imageH, int _imageW) : VJPatternGenerator(_imageH, _imageW) {};

    virtual VJPattern *pattern()
    {
        VJPattern *result = new VJPattern();
        int sizeH = rand() % ((imageH - granule)) + granule;
        int sizeW = rand() % ((imageW - granule)) + granule;
        int x = rand() % (imageW - sizeW);
        int y = rand() % (imageH - sizeH);

        if (sizeW % 3) sizeW -= 1;
        if (sizeW % 3) sizeW -= 1;

        int v = (rand() % 2) ? -1 : 1;
        result->addRectangle(x                , y, x +     sizeW / 3, y + sizeH,       v);
        result->addRectangle(x +     sizeW / 3, y, x + 2 * sizeW / 3, y + sizeH, - 2 * v);
        result->addRectangle(x + 2 * sizeW / 3, y, x +     sizeW    , y + sizeH,       v);

        return result;
    }
};


/**
 *  VerticalPattern2
 *
 *  generates a random pattern of the form below
 *
 *  /code
 *     +----------+
 *     |          |
 *     |    -1    |
 *     +----------+
 *     |    +1    |
 *     |          |
 *     +----------+
 *  /code
 *
 **/
class VerticalPattern2 : public VJPatternGenerator
{
public:
    VerticalPattern2(int _imageH, int _imageW) : VJPatternGenerator(_imageH, _imageW) {};

    virtual VJPattern *pattern()
    {
        VJPattern *result = new VJPattern();
        int sizeH = rand() % ((imageH - granule)) + granule;
        int sizeW = rand() % ((imageW - granule)) + granule;
        int x = rand() % (imageW - sizeW);
        int y = rand() % (imageH - sizeH);

        if (sizeH % 2) sizeH -= 1;
        int v = (rand() % 2) ? -1 : 1;
        result->addRectangle(x,             y, x + sizeW, y + sizeH / 2, -v);
        result->addRectangle(x, y + sizeH / 2, x + sizeW, y + sizeH    ,  v);
        return result;
    }
};

/**
 *  VerticalPattern3
 *
 *  generates a random pattern of the form below
 *
 *  /code
 *     +----------+
 *     |          |
 *     |    -1    |
 *     +----------+
 *     |    +2    |
 *     |          |
 *     +----------+
 *     |    -1    |
 *     |          |
 *     +----------+
 *
 *  /code
 *
 **/
class VerticalPattern3 : public VJPatternGenerator
{
public:
    VerticalPattern3(int _imageH, int _imageW) : VJPatternGenerator(_imageH, _imageW) {};

    virtual VJPattern *pattern()
    {
        VJPattern *result = new VJPattern();
        int sizeH = rand() % ((imageH - granule)) + granule;
        int sizeW = rand() % ((imageW - granule)) + granule;
        int x = rand() % (imageW - sizeW);
        int y = rand() % (imageH - sizeH);

        if (sizeH % 3) sizeH -= 1;
        if (sizeH % 3) sizeH -= 1;

        int v = (rand() % 2) ? -1 : 1;
        result->addRectangle(x , y                , x + sizeW , y +     sizeH / 3,       v);
        result->addRectangle(x , y +     sizeH / 3, x + sizeW , y + 2 * sizeH / 3, - 2 * v);
        result->addRectangle(x , y + 2 * sizeH / 3, x + sizeW , y +     sizeH    ,       v);

        return result;
    }
};

/**
 *  SquarePattern2
 *
 *  generates a random pattern of the form below
 *
 *  /code
 *     +----------+----------+
 *     |          |          |
 *     |    -1    |    +1    |
 *     +----------+----------+
 *     |    +1    |    -1    |
 *     |          |          |
 *     +----------+----------+
 *  /code
 *
 **/
class SquarePattern2 : public VJPatternGenerator
{
public:
    SquarePattern2(int _imageH, int _imageW) : VJPatternGenerator(_imageH, _imageW) {};

    virtual VJPattern *pattern()
    {
        VJPattern *result = new VJPattern();
        int sizeH = rand() % ((imageH - granule)) + granule;
        int sizeW = rand() % ((imageW - granule)) + granule;
        int x = rand() % (imageW - sizeW);
        int y = rand() % (imageH - sizeH);

        if (sizeH % 2) sizeH -= 1;
        if (sizeW % 2) sizeW -= 1;
        int v = (rand() % 2) ? -1 : 1;

        int halfH = sizeH / 2;
        int halfW = sizeW / 2;

        result->addRectangle(x        , y        , x + halfW, y + halfH,  v);
        result->addRectangle(x        , y + halfH, x + halfW, y + sizeH, -v);
        result->addRectangle(x + halfW, y        , x + sizeW, y + halfH, -v);
        result->addRectangle(x + halfW, y + halfH, x + sizeW, y + sizeH,  v);

        return result;
    }
};

/**
 *  AnyPattern2
 *
 *  generates a random pattern of the form below
 *
 *  /code
 *     +----------+----------+
 *     |          |          |
 *     |    -1    |    +1    |
 *     +----------+----------+
 *     |    +1    |    -1    |
 *     |          |          |
 *     +----------+----------+
 *  /code
 *
 **/
/*
class AnyPattern2 : public VJPatternGenerator
{
public:
    AnyPattern2(int _imageH, int _imageW) : VJPatternGenerator(_imageH, _imageW) {};

    virtual VJPattern *pattern()
    {
        VJPattern *result = new VJPattern();
        int sizeH = rand() % (((imageH / 2) - granule)) + granule;
        int sizeW = rand() % (((imageW / 2) - granule)) + granule;
        int x = rand() % (imageW - 2 * sizeW);
        int y = rand() % (imageH - 2 * sizeH);

        if (sizeH % 2) sizeH -= 1;
        if (sizeW % 2) sizeW -= 1;
        int v = (rand() % 2) ? -1 : 1;

        int halfH = sizeH / 2;
        int halfW = sizeW / 2;

        result->addRectangle(x        , y        , x + halfW, y + halfH,  v);
        result->addRectangle(x        , y + halfH, x + halfW, y + sizeH, -v);
        result->addRectangle(x + halfW, y        , x + sizeW, y + halfH, -v);
        result->addRectangle(x + halfW, y + halfH, x + sizeW, y + sizeH,  v);

        return result;
    }
};
*/

/**
 *  A helper class for best classifier search
 **/
class VJResult
{
public:
    int value;
    const AdaBoostInputData<VJInputImage> *inputData;

    VJResult(int _value, const AdaBoostInputData<VJInputImage> * _inputData) : value(_value), inputData(_inputData)
    {};

    inline bool operator < (const VJResult &that) const
    {
        return this->value < that.value;
    }
};

class VJSimpleClassifierGenerator : public WeakHypothesisGenerator<VJSimpleClassifier>
{
public:
#ifdef WITH_TBB
    tbb::spin_mutex VJSimpleClassifierGeneratorMutex;
#endif
    double minEpsilonGlobal;
    VJPattern *bestPattern;
    int bestZ;

    int globalCount;

public:
    vector<VJPatternGenerator *> generators;
    unsigned patternNumber;
    unsigned h;
    unsigned w;



    VJSimpleClassifierGenerator(unsigned  h, unsigned  w, unsigned _patternNumber)
    {
        this->h = h;
        this->w = w;
        generators.push_back(new HorizontalPattern2(h,w));
        generators.push_back(new VerticalPattern2(h,w));
        generators.push_back(new SquarePattern2(h,w));
        generators.push_back(new VerticalPattern3(h,w));
        generators.push_back(new HorizontalPattern3(h,w));

        patternNumber = _patternNumber;
    }


    VJSimpleClassifier *getBestClassifier(const AdaBoostInput<VJInputImage> &inputs, double *cost);

    ~VJSimpleClassifierGenerator()
    {
        for (unsigned i = 0; i < generators.size(); i++)
        {
            delete generators[i];
            generators[i] = NULL;
        }
    }


};

class VJAdaBoostedClassifier : public AdaBoostedClassifier<VJSimpleClassifier>
{
public:
    int leftMargin;
    int topMargin;
    int bottomMargin;
    int rightMargin;

    void initLimits()
    {
        leftMargin = std::numeric_limits<int>::max();
        topMargin = std::numeric_limits<int>::max();

        rightMargin = std::numeric_limits<int>::min();
        bottomMargin = std::numeric_limits<int>::min();

        for (unsigned i = 0; i < children.size(); i++)
        {
            if (leftMargin > children[i]->leftMargin)
                leftMargin = children[i]->leftMargin;
            if (topMargin > children[i]->topMargin)
                topMargin = children[i]->topMargin;

            if (rightMargin < children[i]->rightMargin)
                rightMargin = children[i]->rightMargin;
            if (bottomMargin < children[i]->bottomMargin)
                bottomMargin = children[i]->bottomMargin;
        }

    }

    bool applyToPoint(G12IntegralBuffer *buffer, int y, int x)
    {
        double result = 0;
        for (unsigned i = 0; i < children.size(); i++)
        {
            VJSimpleClassifier *child = children[i];
            result += weights[i] * (child->applyToPoint(buffer, y, x) ?  1 : -1);
        }
        return (result > 0);
    }

    friend ostream& operator << (ostream &out, VJAdaBoostedClassifier &toSave)
    {
        out << toSave.children.size() << "\n";
        for (unsigned i = 0; i < toSave.children.size(); i++)
        {
            out << toSave.weights[i] << "\n";
            out << toSave.children.at(i);
            out << "\n";
        }
        return out;
    }

    void print ()
    {
        cout << this;
    }

    /**
     *  Redesign this ASAP
     * */
    friend istream & operator >>  (istream &in, VJAdaBoostedClassifier &toLoad)
    {
        unsigned size;
        in >> size;
        for (unsigned i = 0; i < size; i++)
        {
            double weight;
            in >> weight;
            VJSimpleClassifier *classifier = new VJSimpleClassifier();
            in >> (*classifier);
            toLoad.children.push_back(classifier);
            toLoad.weights.push_back(weight);
        }
        return in;
    }

    VJAdaBoostedClassifier *scale(double scale)
    {
        VJAdaBoostedClassifier *toReturn = new VJAdaBoostedClassifier();
        for(unsigned i = 0; i < children.size(); i++)
        {
            toReturn->weights[i] = weights[i];
            toReturn->children[i] = children[i]->scale(scale);
        }
        return toReturn;
    }
};

} //namespace corecvs
#endif /* VJPATTERN_H_ */

