/**
 * \file g8Buffer.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Jul 6, 2010
 * \author alexander
 */

#include "g8Buffer.h"
#include "g12Buffer.h"

namespace corecvs {

G8Buffer* G8Buffer::FromG12Buffer(G12Buffer *input)
{
    G8Buffer* result = new G8Buffer(input->h, input->w, false);
    for (int i = 0; i < input->h; i++)
    {
        for (int j = 0; j < input->w; j++)
        {
            result->element(i,j) = (input->element(i,j) >> 4);
        }
    }
    return result;
}


G12Buffer* G8Buffer::toG12Buffer(corecvs::G8Buffer *input)
{
    G12Buffer* result = new G12Buffer(input->getSize(), false);
    for (int i = 0; i < input->h; i++)
    {
        for (int j = 0; j < input->w; j++)
        {
            result->element(i,j) = (input->element(i,j) << 4);
        }
    }
    return result;
}


} //namespace corecvs

