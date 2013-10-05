/**
 * \file align_nonlinear.c
 * \brief This file normalizes the camera distortions.
 *
 * We use the Heikkika model described in
 * http://www.vision.caltech.edu/bouguetj/calib_doc/papers/heikkila97.pdf
 *
 * \ingroup corefiles
 * \date May 20, 2009
 * \author Alexander Pimenov
 */

#include <math.h>
#include <stdio.h>

#include "global.h"
#include "g12buffer.h"
#include "align_nonlinear.h"

/**
 * \brief Apply the lens distortion correction to the single point
 *
 * \param input
 * \param params
 * \param output
 *
 **/

void getCorrectionForPoint(vector2Dd input, LensCorrectionParametres *params, vector2Dd *output)
{
        double dx = input.x - params->center.x;
        double dy = input.y - params->center.y;
        double dxsq = dx * dx;
        double dysq = dy * dy;
        double dxdy = dx * dy;

        double rsq = dxsq + dysq;
        double radialCorrection = params->k1 * rsq +  params->k2 * rsq * rsq;

        double radialX = (double)dx * radialCorrection;
        double radialY = (double)dy * radialCorrection;

        double tangentX = 2 * params->p1 * dxdy + params->p2 * ( rsq + 2 * dxsq );
        double tangentY = params->p1 * (rsq + 2 * dysq) + 2 * params->p2 * dxdy;

        output->x = radialX + tangentX;
        output->y = radialY + tangentY;
}

/**
 * \brief Apply the lens distortion correction.
 *
 * For details see LensCorrectionParametres
 *
 * \param input the buffer to be corrected
 * \param params the intrinsic parameters of the lens
 * \return A corrected buffer
 *
 */
G12Buffer *correctLens (G12Buffer *input, LensCorrectionParametres* params)
{
        int h = input->h;
        int w = input->w;
        int i,j;
        G12Buffer *toReturn = g12BufferCreate(h, w);
        for (i = 0; i < h; i++)
                for (j = 0; j < w; j++)
                {
                        vector2Dd corrector;
                        getCorrectionForPoint(vector2DdCreate((double)j,(double) i), params, &corrector);

                        int x = (double)j - corrector.x;
                        int y = (double)i - corrector.y;

                        if (x < 0 || x > input->w - 2  || y < 0 || y > input->h - 2)
                        {
                                toReturn->data[i * w + j] = 0x80 << 4;
                                continue;
                        }

                        int a = G12ELEM(input, (int)x    , (int)y    );
                        int b = G12ELEM(input, (int)x + 1, (int)y    );
                        int c = G12ELEM(input, (int)x    , (int)y + 1);
                        int d = G12ELEM(input, (int)x + 1, (int)y + 1);

                        double k1 = x - (int)(x);
                        double k2 = y - (int)(y);

                        int result = (a * (1 - k1) + k1 * b) * (1 - k2) +
                                                 (c * (1 - k1) + k1 * d) * k2;
                        toReturn->data[i * w + j] = result;
                }

        return toReturn;

}




void CameraModel(double input[], double X, double Y, double Z, double *x, double *y)
{
        // The input layout
        double k1 = input[0];
        double k2 = input[1];

        double p1 = input[2];
        double p2 = input[3];

        double a = input[4];
        double b = input[5];
        double c = input[6];
        double d = input[7];

        double X0 = input[8];
        double Y0 = input[9];
        double Z0 = input[10];

        double f = input[11];

        double sx = input[12];
        double sy = input[13];

        double u0 = input[14];
        double v0 = input[15];

    //
        double len = sqrt (a * a + b * b + c * c + d * d);
        a /= len;
        b /= len;
        c /= len;
        d /= len;

        /**
         *
         * [-d^2-c^2+b^2+a^2, 2*b*c-2*a*d,          2*b*d+2*a*c]
         * [2*b*c+2*a*d,      -d^2+c^2-b^2+a^2,     2*c*d-2*a*b]
         * [2*b*d-2*a*c,      2*c*d+2*a*b,      d^2-c^2-b^2+a^2])
         */

         float da, db, dc, aa, bb, bc, ab, ac, cc, a2, b2, c2;
         float s  = 2.0f/len;  // 4 mul 3 add 1 div
                a2 = a * s;    b2 = b * s;    c2 = c * s;
                aa = a * a2;   ab = a * b2;   ac = a * c2;
                bb = b * b2;   bc = b * c2;   cc = c * c2;
                da = d * a2;   db = d * b2;   dc = d * c2;

         double RX  = (1.0 - (bb + cc)) * X +          (ab - dc) * Y +          (ac + db) * Z;
         double RY  =       ( ab + dc ) * X + (1.0f - (aa + cc)) * Y +          (bc - da) * Z;
         double RZ  =         (ac - db) * X +          (bc + da) * Y + (1.0f - (aa + bb)) * Z;


        /*double RX = (-(d*d)-(c*c)+(b*b)+(a*a)) * X +              (2*b*c-2*a*d) * Y +             (2*b*d+2*a*c) * Z;
        double RY =              (2*b*c+2*a*d) * X + (-(d*d)+(c*c)-(b*b)+(a*a)) * Y +             (2*c*d-2*a*b) * Z;
        double RZ =                     (2*b*d-2*a*c) * X +              (2*c*d+2*a*b) * Y + ((d*d)-(c*c)-(b*b)+(a*a)) * Z;
*/
        double MX = RX + X0;
        double MY = RY + Y0;
        double MZ =        RZ + Z0;
        //printf (" Len1: %lf Len2: %lf\n", sqrt(X*X + Y*Y + Z*Z), sqrt(RX*RX + RY*RY + RZ*RZ));

        double PX = (f / MZ) * MX;
        double PY = (f / MZ) * MY;

        vector2Dd uncorrected = {PX * sx + u0, PY * sy + v0};
        LensCorrectionParametres lcp = { k1, k2, p1, p2, {u0, v0}};

        vector2Dd corrector;
        getCorrectionForPoint(uncorrected, &lcp, &corrector);

        *x = uncorrected.x + corrector.x;
        *y = uncorrected.y + corrector.y;

}




void FmodelSimple(double input[], double X, double Y, double Z, double *x, double *y)
{
        double a = input[0];
        double b = input[1];
        double c = input[2];
        double d = input[3];

        double X0 = input[4];
        double Y0 = input[5];
        double Z0 = input[6];

        double f = input[7];

        double sx = input[8];
        double sy = input[9];

        double u0 = input[10];
        double v0 = input[11];

    //
        double len = sqrt (a * a + b * b + c * c + d * d);
        a /= len;
        b /= len;
        c /= len;
        d /= len;

        /**
         *
         * [-d^2-c^2+b^2+a^2, 2*b*c-2*a*d,          2*b*d+2*a*c]
         * [2*b*c+2*a*d,      -d^2+c^2-b^2+a^2,     2*c*d-2*a*b]
         * [2*b*d-2*a*c,      2*c*d+2*a*b,      d^2-c^2-b^2+a^2])
         */


        //

        double RX = (-(d*d)-(c*c)+(b*b)+(a*a)) * X +              (2*b*c-2*a*d) * Y +             (2*b*d+2*a*c) * Z;
        double RY =              (2*b*c+2*a*d) * X + (-(d*d)+(c*c)-(b*b)+(a*a)) * Y +             (2*c*d-2*a*b) * Z;
        double RZ =                     (2*b*d-2*a*c) * X +              (2*c*d+2*a*b) * Y + ((d*d)-(c*c)-(b*b)+(a*a)) * Z;

        double MX = RX + X0;
        double MY = RY + Y0;
        double MZ =        RZ + Z0;
        //printf (" Len1: %lf Len2: %lf\n", sqrt(X*X + Y*Y + Z*Z), sqrt(RX*RX + RY*RY + RZ*RZ));

        double PX = (f / MZ) * MX;
        double PY = (f / MZ) * MY;

        double dx = PX * sx;
        double dy = PY * sy;

        *x = dx + u0;
        *y = dy + v0;
}

void getCorrectionForPointDummy(vector2Dd input, LensCorrectionParametres *params, vector2Dd *output)
{
        output->x = input.x / 3;
        output->y = input.y / 3;
}

#ifdef DEPRICATED_TO_PORT
DisplacementBuffer *displacementBufferFromCamera(LensCorrectionParametres *params, int h, int w)
{

        int i,j;
        DisplacementBuffer *toReturn = displacementBufferCreate(h, w);
        for (i = 0; i < h; i++)
                for (j = 0; j < w; j++)
                {
                        vector2Dd corrector;
                        getCorrectionForPoint(vector2DdCreate((double)j,(double) i), params, &corrector);
                        displacementBufferSetDisplacement(toReturn, j,i, corrector.x, corrector.y);
                }

        return toReturn;

}



/**
 *
 *  Simulating the distortion as precise as possible
 *
 *  The solution is as follows - assuming that derivative of the mapping function is close to 1
 *  we do the forward transformation of 4 points, then iterate throw all the points inside the transformed rectangle
 *  that have integer coordinates, after that we do the liner interpolation of the backward map
 *
 *  \param params
 *  \param h
 *  \param w
 *
 **/
DisplacementBuffer *displacementBufferFromInverseCamera(LensCorrectionParametres *params, int h, int w)
{
        int i,j;
        DisplacementBuffer *toReturn = displacementBufferCreate(h, w);
        for (i = 0; i < h; i++)
        {
                for (j = 0; j < w; j++)
                {
                        vector2Dd input11 = vector2DdCreate((double)j - 0.5,(double) i - 0.5);
                        vector2Dd input12 = vector2DdCreate((double)j - 0.5,(double) i + 0.5);
                        vector2Dd input21 = vector2DdCreate((double)j + 0.5,(double) i - 0.5);
                        vector2Dd input22 = vector2DdCreate((double)j + 0.5,(double) i + 0.5);

                        vector2Dd corrector11;
                        vector2Dd corrector12;
                        vector2Dd corrector21;
                        vector2Dd corrector22;

                        getCorrectionForPoint(input11, params, &corrector11);
                        getCorrectionForPoint(input12, params, &corrector12);
                        getCorrectionForPoint(input21, params, &corrector21);
                        getCorrectionForPoint(input22, params, &corrector22);

                        vector2DdIncrement(&corrector11, &input11);
                        vector2DdIncrement(&corrector12, &input12);
                        vector2DdIncrement(&corrector21, &input21);
                        vector2DdIncrement(&corrector22, &input22);

                        TriangleIterator part1 = triangleIteratorCreate(corrector11, corrector12, corrector21);
                        TriangleIterator part2 = triangleIteratorCreate(corrector22, corrector12, corrector21);

                        double dx1, dy1, dx2, dy2;
                        double Dmain;
                        dx1 = corrector12.x - corrector11.x;
                        dy1 = corrector12.y - corrector11.y;
                        dx2 = corrector21.x - corrector11.x;
                        dy2 = corrector21.y - corrector11.y;
                        Dmain = dx1 * dy2 - dx2 * dy1;

                        vector2Ds32 point;
                        while (triangleIteratorNext(&part1, &point))
                        {
                                if (point.x < 0 || point.x >= w || point.y < 0 || point.y >= h)
                                {
//                                        DOTRACE(("Rejected upper (%d %d)\n", point.x, point.y));
                                        continue;
                                }

                                double x = point.x - corrector11.x;
                                double y = point.y - corrector11.y;

                                /* solve the system with Kramer method*/
                                double v = (dy2 * x - dx2 * y) / Dmain;
                                double u = (dx1 * y - dy1 * x) / Dmain;

//                                DOTRACE(("Accepted upper (%d %d)=(%lf %lf)\n", point.x, point.y, resultx, resulty));
                                displacementBufferSetCoordinates(toReturn, point.x, point.y, (u + j - 0.5), (v + i - 0.5));
                        }

                        dx1 = corrector12.x - corrector22.x;
                        dy1 = corrector12.y - corrector22.y;
                        dx2 = corrector21.x - corrector22.x;
                        dy2 = corrector21.y - corrector22.y;
                        Dmain = dx1 * dy2 - dx2 * dy1;

                        while (triangleIteratorNext(&part2, &point))
                        {
                                if (point.x < 0 || point.x >= w || point.y < 0 || point.y >= h)
                                {
//                                        DOTRACE(("Rejected lower (%d %d)\n", point.x, point.y));
                                        continue;
                                }

                                double x = point.x - corrector22.x;
                                double y = point.y - corrector22.y;

                                /* solve the system with Kramer method*/
                                double u = (dy2 * x - dx2 * y) / Dmain;
                                double v = (dx1 * y - dy1 * x) / Dmain;

//                                DOTRACE(("Accepted lower (%d %d)=(%lf %lf)\n", point.x, point.y, resultx, resulty));
                                displacementBufferSetCoordinates(toReturn, point.x, point.y, ( - u + j + 0.5), ( - v + i + 0.5));
                        }
                }
        }
        return toReturn;
}
#endif
