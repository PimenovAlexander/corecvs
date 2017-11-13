#ifndef HISTORGAM_H_
#define HISTORGAM_H_

/**
 * \file histogram.h
 * \brief The histogram of the integer based buffers
 *
 * \ingroup cppcorefiles
 * \date Mar 1, 2010
 * \author alexander
 */


#include <vector>

#include "core/utils/global.h"

#include "core/buffers/g12Buffer.h"
#include "core/buffers/commonMappers.h"
namespace corecvs {

using std::vector;
/**
 *  Histogram class.
 *  This class holds the histogram with the integer argument.
 *  The min and max values are inclusive
 *
 *  So far template is not needed and is not implemented for a while.
 *
 **/
class Histogram
{
protected:
    int maxValue;
    int totalSum;
public:
    vector<unsigned> data;
    int min;
    int max;

    Histogram(int min, int max)
    {
        _init(min, max);
    }

    Histogram(G12Buffer *buffer, int min, int max)
    {
        _init(min, max);
        IntervalMapper mapper(min, max);

        /** \todo TODO: Use TBB here... */
        for (int i = 0; i < buffer->h; i++)
        {
            for (int j = 0; j < buffer->w; j++)
            {
                int result = mapper.operator ()(buffer->element(i, j));
                data[result]++;
            }
        }
        totalSum = buffer->h * buffer->w;
    }

    Histogram(G12Buffer *buffer)
    {
        CORE_ASSERT_TRUE(buffer != NULL, "Input buffer should not be NULL");

        _init(0, G12Buffer::BUFFER_MAX_VALUE);
        for (int i = 0; i < buffer->h; i++)
        {
            for (int j = 0; j < buffer->w; j++)
            {
                int result = buffer->element(i, j);
                CORE_ASSERT_TRUE(result <= G12Buffer::BUFFER_MAX_VALUE, "G12Buffer has overflows");
                data[result]++;
            }
        }
        totalSum = buffer->h * buffer->w;
    }


    inline void inc(int argument)
    {
        if (argument >= min && argument <= max)
        {
            data[argument - min]++;
            totalSum++;
        }
    }

    inline void add(const Histogram *other)
    {
        int newMin = std::min(min, other->min);
        int newMax = std::max(max, other->max);

        int newSize = newMax - newMin + 1;
        if (newSize != (int)data.size())
        {
            int s = (int)data.size();
            data.resize(newSize);
            for (; s >= 0 ; s--) {
                data[s + min - newMin] = data[s];
            }
        }
        min = newMin;
        max = newMax;

        for (int i = other->min; i <= other->max; i++)
        {
            {
                data[i - min] += other->getData(i);
            }
        }
        totalSum +=  other->totalSum;
    }

    inline void set(int argument, int value)
    {
        if (argument >= min && argument <= max)
        {
            totalSum -= data[argument - min];
            data[argument - min] = value;
            totalSum += value;
        }
    }

    inline int getArgumentMax()
    {
        return max;
    }

    inline int getArgumentMin()
    {
        return min;
    }


    /*void addBuffer (G12Buffer *buffer)
    {

    }*/

    int getMaximum (bool useMargins = true)
    {
        unsigned max = 0;
        unsigned int start = useMargins ? 0 : 1;
        unsigned int end   = (unsigned int)(data.size() - start);

        for (unsigned int i = start; i < end; i++)
        {
            if (max < data[i])
                max = data[i];
        }
        return max;
    }

    int getLowerPersentile(double persentile);
    int getHigherPersentile(double persentile);

    int getUnderExpo()
    {
        return data[0];
    }

    int getOverExpo()
    {
        return data[data.size() - 1];
    }

    double getUnderExpoRel()
    {
        return (double)data[0] / totalSum;
    }

    double getOverExpoRel()
    {
        return (double)data[data.size() - 1] / totalSum;
    }


    int getData(int value) const
    {
        return data[value - min];
    }

    int getTotalSum() const
    {
        return totalSum;
    }

    int getIntervalSum(int intMin, int intMax) const
    {
        int sum = 0;
        intMin = std::max(min, intMin);
        intMax = std::min(max, intMax);

        for (int i = intMin - min; i <= intMax - min; i++)
        {
            sum += data[i];
        }

        return sum;
    }

    int getOtsuThreshold();
    int getMeanThreshold();
    int getMedianThreshold();


    double getMeanValue(double *dev);

    /**
     *   Having an input histogram we search for an interval that satisfies
     *   following criteria:
     *
     *
     *
     *   \code
     *    minWeight = 30
     *
     *                      >minWeight
     *                      ________
     *     ^               /        \
     *     |
     *     |                    *
     *     |                    **
     *     |                    **
     *     |       *            ***
     *     |- - - -*-*- *** - - *****- - -   <-- startLevel
     *     |   *   *** *****  *******
     *     |  *** **** ****** **********
     *     +------------------------------
     *                     ^        ^
     *                     |        |
     *                   start     end
     *
     *   \endcode
     *
     *
     **/
    void getTopInterval (int startLevel, int minWeight, int *start, int *end)
    {
        int i;
        for (i = (int)data.size() - 1; i >= 0; i--)
        {
            if (((int)data.at(i)) >= startLevel)
                break;
        }

        *end = i + min;

        int weight = 0;
        for (; i >= 0; i--)
        {
            weight += data.at(i);
            if (weight >= minWeight)
                break;
        }
        *start = i + min;
    }

    void getBottomInterval (int startLevel, int minWeight, int *start, int *end)
    {
        int i;
        for (i = 0; i < (int)data.size(); i++)
        {
            if (((int)data.at(i)) >= startLevel)
                break;
        }

        *start = i + min;

        int weight = 0;
        for (; i < (int)data.size(); i++)
        {
            weight += data.at(i);
            if (weight >= minWeight)
                break;
        }
        *end = i + min;
    }

    virtual ~Histogram();

    friend ostream& operator << (ostream &out, Histogram &h)
    {
        for (int i = h.getArgumentMin(); i <= h.getArgumentMax(); i++)
        {
            out << i << " -> " << h.getData(i) << endl;
        }
        return out;
    }


private:
    void _init(int min, int max)
    {
        this->min = min;
        this->max = max;
        // This also zerofies the vector
        data.resize(max + 1 - min);
        totalSum = 0;
    }
};



} //namespace corecvs
#endif /* HISTORGAM_H_ */

