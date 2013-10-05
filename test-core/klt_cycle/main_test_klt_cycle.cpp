/**
 * \file main_test_klt_cycle.cpp
 * \brief Add Comment Here
 *
 * \date Aug 5, 2010
 * \author alexander
 */

#include <iostream>
#include "g12Buffer.h"
#include "kltGenerator.h"
#include "bufferFactory.h"
#include "bmpLoader.h"

using namespace corecvs;


int testKLT ( void )
{
    /* Ok prepare a test buffer 8 by 8
              0  1  2  3  4  5  6  7
          ___________________________
      0   |  10 10 10 10  0  0  0  0
      1   |  10 10 10 10  0  0  0  0
      2   |  10 10 10 10  0  0  0  0
      3   |  10 10 10 10  0  0  0  0
      4   |  20 20 20<20>10 10 10 10
      5   |  20 20 20 20 10 10 10 10
      6   |  20 20 20 20 10 10 10 10
      7   |  20 20 20 20 10 10 10 10
     */


    cout << "First Image\n";
    G12Buffer *first = new G12Buffer(8,8);
    first->fillRectangleWith(0,0,4,4, 10);
    first->fillRectangleWith(4,0,4,4, 20);
    first->fillRectangleWith(4,4,4,4, 10);
    cout << *first << "\n";

    /* Second buffer
                0  1  2  3  4  5  6  7
            ___________________________
        0   |  10 10 10 10  5  0  0  0
        1   |  10 10 10 10  5  0  0  0
        2   |  10 10 10 10  5  0  0  0
        3   |  15 15 15 15 10  5  5  5
        4   |  20 20 20<20>15 10 10 10
        5   |  20 20 20 20 15 10 10 10
        6   |  20 20 20 20 15 10 10 10
        7   |  20 20 20 20 15 10 10 10
   */
    cout << "Second Image\n";
    G12Buffer *second = new G12Buffer(8,8);
    second->fillRectangleWith(0,0,4,4, 10);
    second->fillRectangleWith(3, 0, 1, 4, 15);
    second->fillRectangleWith(0,4,3,1, 5);
    second->fillRectangleWith(4,0,4,4, 20);
    second->fillRectangleWith(4,4,4,4, 10);
    second->fillRectangleWith(3,4,1,4, 5);
    second->fillRectangleWith(3,4,1,1, 10);
    second->fillRectangleWith(4,4,4,1, 15);
    cout << *second << "\n";

    SpatialGradient *sg = new SpatialGradient(first);

    KLTCalculationContext context;
    context.first  = first;
    context.second = second;
    context.gradient = new SpatialGradientIntegralBuffer(sg);


    KLTGenerator<BilinearInterpolator> generator(Vector2d32(2,2), 5);
    Vector2dd guess(0,0);
    generator.kltIteration(context, Vector2d32(3,3), &guess, 2.0);

    Vector2dd guessSubpixel(0,0);
    generator.kltIterationSubpixel(context, Vector2dd(3,3), &guessSubpixel, 2.0);

    cout << "Result shift is " << guess.x() << ":" << guess.y() << "\n";
    cout << "Result shift is " << guessSubpixel.x() << ":" << guessSubpixel.y() << "\n";



    delete sg;

    return 0;
}




int main (void)
{
    testKLT();
    return 0;
}
