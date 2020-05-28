/**
 * \file tileGrid.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Apr 11, 2010
 * \author alexander
 */

#include "segmentation/tileGrid.h"
namespace corecvs {


/**
 * Unite information form hxw blocks of tiles
 * Todo - make this tunable
 **/
void TileGrid::selectWithThreshold (int threshold, int h, int w)
{

    for (int i = 0; i < this->h - 1; i++)
    {
        for (int j = 0; j < this->w - 1; j++)
        {
            int sum = 0;

            for (int dy = 0; dy < h; dy++ )
                for (int dx = 0; dx < w; dx++ )
                    sum += this->element(i + dy,j + dx).nums;

            /* If there are too few points tiles are not of interest*/
            if (sum < threshold)
                continue;

            for (int dy = 0; dy < h; dy++ )
                for (int dx = 0; dx < w; dx++ )
                    this->element(i + dy,j + dx).selected = 1;
        }
    }
}


} //namespace corecvs

