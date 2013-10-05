/**
 * \file cornermarker.c
 * \brief This file helps to highlight corners and interesting points to track on the image
 *
 * \ingroup corefiles
 * \date Jun 3, 2009
 * \author Alexander Pimenov
 */
#include "cornermarker.h"
#include "g12buffer.h"
#include "global.h"

#include <stdlib.h>
#include <math.h>


#define APPERTURE 2
double weight[2 * APPERTURE + 1] =
        {1.0 / 10,
         2.0 / 10,
         4.0 / 10,
         2.0 / 10,
         1.0 / 10 };


/**
 * \brief Highlights corners and interesting points to track on the image
 *
 * The algorithm is used form here -
 *
 *   http://en.wikipedia.org/wiki/Corner_detection#The_Harris_.26_Stephens_.2F_Plessey_corner_detection_algorithm
 *
 *  This thing is only needed for recognizing calibration patterns. So no optimizations are done
 *
 *  \param[in] input this parameter stores an input image
 *  \return will hold the resulting image with highlighted corners. The more is the value the more the pixel looks like a corner.
 *
 * */

G12Buffer* findCornerPoints(G12Buffer *input)
{
        int h = input->h;
        int w = input->w;
        int i,j,k;

        /*double *dx   = (double *)calloc(h * w, sizeof(double));
        double *dy   = (double *)calOtherAlgebraloc(h * w, sizeof(double));*/
        double *dxdx = (double *)calloc(h * w, sizeof(double));
        double *dydy = (double *)calloc(h * w, sizeof(double));
        double *dxdy = (double *)calloc(h * w, sizeof(double));

        double *sumhdxdx = (double *)calloc(h * w, sizeof(double));
        double *sumhdydy = (double *)calloc(h * w, sizeof(double));
        double *sumhdxdy = (double *)calloc(h * w, sizeof(double));

        double *sumdxdx = (double *)calloc(h * w, sizeof(double));
        double *sumdydy = (double *)calloc(h * w, sizeof(double));
        double *sumdxdy = (double *)calloc(h * w, sizeof(double));


        G12Buffer *result = g12BufferCreate(h,w);


        for (i = 1; i < h; i++)
        {
                for (j = 1; j < w; j++)
                {
                        double cdx;
                        double cdy;
                        cdx =   ( - (double)input->data[(i - 1) * w + j - 1] + input->data[(i - 1) * w + j + 1]
                                      + 2 * ( - input->data[      i * w + j - 1] + input->data[      i * w + j + 1])
                                                      - input->data[(i + 1) * w + j - 1] + input->data[(i + 1) * w + j + 1]) / 8.0;
                        cdy =   ( - (double)input->data[(i - 1) * w + j - 1] + input->data[(i + 1) * w + j - 1]
                                         + 2 * ( - input->data[(i - 1) * w + j    ] + input->data[(i + 1) * w + j    ])
                                          - input->data[(i - 1) * w + j + 1] + input->data[(i + 1) * w + j + 1]) / 8.0;
                        /*dx  [i * w + j] = cdx;
                        dy  [i * w + j] = cdy;*/
                        dxdx[i * w + j] = cdx * cdx;
                        dydy[i * w + j] = cdy * cdy;
                        dxdy[i * w + j] = cdx * cdy;
                }
        }



        for (i = 1; i < h - 1; i++)
        {
                for (j = APPERTURE ; j < w - APPERTURE ; j++)
                {
                        int wcount = 0;
                        for (k = -APPERTURE ; k <= APPERTURE ; k++, wcount++)
                        {
                                sumhdxdx[i * w + j] += (dxdx[i * w + j + k] / ( 2 * APPERTURE + 1)) * weight[wcount];
                                sumhdydy[i * w + j] += (dydy[i * w + j + k] / ( 2 * APPERTURE + 1)) * weight[wcount];
                                sumhdxdy[i * w + j] += (dxdy[i * w + j + k] / ( 2 * APPERTURE + 1)) * weight[wcount];
                        }
                }
        }

        free (dxdx);
        free (dydy);
        free (dxdy);


        for (i = APPERTURE; i < h - APPERTURE ; i++)
        {
                for (j = 1 ; j < w - 1; j++)
                {
                        int wcount = 0;
                        for (k = -APPERTURE  ; k <= APPERTURE ; k++, wcount++)
                        {
                                sumdxdx[i * w + j] += (sumhdxdx[(i + k) * w + j] / (2 * APPERTURE + 1)) * weight[wcount];
                                sumdydy[i * w + j] += (sumhdydy[(i + k) * w + j] / (2 * APPERTURE + 1)) * weight[wcount];
                                sumdxdy[i * w + j] += (sumhdxdy[(i + k) * w + j] / (2 * APPERTURE + 1)) * weight[wcount];
                        }
                }
        }

        free (sumhdxdx);
        free (sumhdydy);
        free (sumhdxdy);



        for (i = 5; i < h - 5; i++)
        {
                for (j = 5; j < w - 5; j++)
                {
                        // Counting eigenvalues
                        double a = 1;
                        double b = - (sumdxdx[i * w + j] + sumdydy[i * w + j]);
                        double c = sumdxdx[i * w + j] * sumdydy[i * w + j] - sumdxdy[i * w + j] * sumdxdy[i * w + j];

                        /* TODO: solve quadric to other class*/
                        double D = b * b - 4 * a * c;
                        if (D < 0)
                        {
                                result->data[i * w + j] = 0;
                        }
                        else
                        {
                                double lambda1 = (-b + sqrt(D)) / (2 * a);
                                double lambda2 = (-b - sqrt(D)) / (2 * a);
                                if (lambda1 > 2000 && lambda2 > 2000)
                                {
                                        result->data[i * w + j] = (lambda1 * lambda2)  / (1000.0 * 1000.0);
                                }
                        }
                }
        }

        free (sumdxdx);
        free (sumdydy);
        free (sumdxdy);
        return result;

}
