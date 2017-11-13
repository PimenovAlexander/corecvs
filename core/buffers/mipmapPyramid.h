#ifndef MIPMAPPYRAMID_H_
#define MIPMAPPYRAMID_H_
/**
 * \file mipmapPyramid.h
 * \brief a header for MidmapPyramid.cpp
 *
 *
 * \ingroup cppcorefiles
 * \date Feb 23, 2010
 * \author: alexander
 */

#include <vector>

#include "core/utils/global.h"

#include "core/buffers/abstractBuffer.h"
#include "core/buffers/kernels/gaussian.h"

namespace corecvs {

using std::vector;

/**
 *  \brief Template class that holds the pyramid of the reduced size buffers.
 *  For more details please see - http://en.wikipedia.org/wiki/Mipmap
 **/
template<typename BufferType>
class AbstractMipmapPyramid
{
public:
    int levelNumber;
    vector<BufferType *> levels;

    AbstractMipmapPyramid(BufferType *input , int _levelNumber)
    {
        levelNumber = _levelNumber;
        levels.resize(levelNumber);
        levels[0] = new BufferType(input);
        for (int i = 1; i < _levelNumber; i++)
        {
            levels[i] = downsample(levels[i-1]);
        }
    }

    AbstractMipmapPyramid(BufferType *input , int _levelNumber, double factor)
    {
        levelNumber = _levelNumber;
        levels.resize(levelNumber);
        levels[0] = new BufferType(input);
        for (int i = 1; i < _levelNumber; i++)
        {
            levels[i] = downsample(levels[i-1], factor);
        }
    }


    static BufferType *downsample(BufferType *input)
    {
        BufferType* toReturn = new BufferType(input->h / 2, input->w / 2);
        for (int32_t i = 0; i < toReturn->h; i++)
        {
            for (int32_t j = 0; j < toReturn->w; j++)
            {
                toReturn->element(i,j) = Gaussian3x3int::instance->multiplyAtPoint(input, 2*i, 2*j);
            }
        }

        return toReturn;
    }

    /**
     * This is designed for small factors of about 1.1-1.6
     *
     * */
    static BufferType *downsample(BufferType *input, double factor)
    {
        BufferType* toReturn = new BufferType(input->h / factor, input->w / factor, false);
        for (int32_t i = 0; i < toReturn->h; i++)
        {
            for (int32_t j = 0; j < toReturn->w; j++)
            {
                //toReturn->element(i,j) = Gaussian3x3int::instance->multiplyAtPoint(input, (int)(factor*i), (int)(factor*j));
                toReturn->element(i,j) = input->elementBl(factor * i, factor * j);
            }
        }

        return toReturn;
    }


};


} //namespace corecvs
#endif  //MIPMAPPYRAMID_H_

