/**
 * \file test_convolve.c
 * \brief Add Comment Here
 *
 * \date Feb 25, 2010
 * \author alexander
 */

#include <stdint.h>
#include <stdio.h>

#include "global.h"

#include "vector3d.h"
//#include "vector2d.h"

#include "g12Buffer.h"

#include "gaussian.h"
#include "mipmapPyramid.h"
#include "kltGenerator.h"
#include "bufferFactory.h"

using namespace corecvs;

int main ( int /*argV*/, char * /*argC*/[])
{
    G12Buffer *buffer = BufferFactory::getInstance()->loadG12Bitmap("data/pair/image0001_c0.pgm");

/*    int numLevels = 4;
    AbstractMidmapPyramid<G12Buffer> *pyr = new AbstractMidmapPyramid<G12Buffer>(buffer, numLevels);
    for (int i = 0; i < numLevels; i++)
    {
        char outName[256];
        snprintf2buf(outName, "pyr%d.bmp", i);

        G12Buffer *level = pyr->levels[i];

        G12Buffer *toSave = g12BufferCreate(level->h, level->w);
        memcpy(toSave->data, level->data, sizeof (uint16_t) * level->h * level->w);

        SDL_Surface *tmp;
        tmp = SDL_CreateRGBSurface(0,toSave->w,toSave->h, 32, 0,0,0,0);
        drawG12BufferIntoSDL(toSave, tmp, 0, 0);
        printf("Writing to %s\n", outName);
        SDL_SaveBMP(tmp, outName);
        SDL_FreeSurface(tmp);
    }*/

    SpatialGradient *sg = new SpatialGradient(buffer);
    SpatialGradientIntegralBuffer *gradient = new SpatialGradientIntegralBuffer(sg);

    for (int i = 1; i < gradient->h; i++)
    {
        for (int j = 1; j < gradient->w; j++)
        {
            double *grad1 = (double *)(&gradient->element(i,j));
            double *grad2 = (double *)(&gradient->element(i - 1,j));
            double *grad3 = (double *)(&gradient->element(i,j - 1));
            double *grad4 = (double *)(&gradient->element(i - 1,j - 1));

            double val1  = *grad1 + *grad4 - (*grad2 + *grad3);
            double *val = (double *)(&sg->element(i,j));
            if (*val != val1)
                printf("Problem");
        }
    }

    return 1;
}

/*
int main ( int argV, char * argC[])
{
    uint16_t data1[36] = {
                        1,3,7,8,1,3,
                        1,1,7,8,1,1,
                        1,3,7,8,1,3,
                        1,1,7,8,1,1,
                        1,3,7,8,1,3,
                        4,4,4,4,4,4
                     };
    G12Buffer *base1 = new G12Buffer(6,6,data1);
    base1->print();

    G12Buffer *filtered = base1->doConvolve1<double, uint32_t>(Gaussian3x3::instance);
    filtered->print();

    AbstractMidmapPyramid<G12Buffer> *pyr = new AbstractMidmapPyramid<G12Buffer>(base1, 2);

    printf("Levels:\n");
    pyr->levels[0]->print();
    pyr->levels[1]->print();
    return 0;
}*/
