/**
 * \file main_test_geometry.cpp
 * \brief This is the main file for the test geometry 
 *
 * \date Apr 10, 2011
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include "global.h"
#include "polygons.h"
#include "vector2d.h"

using namespace std;
using namespace corecvs;

int main (int /*argC*/, char ** /*argV*/)
{
        Poligon p;
        p.push_back(Vector2dd(0.0,0.0));
    p.push_back(Vector2dd(0.0,1.0));
    p.push_back(Vector2dd(1.0,0.0));

    const unsigned TEST_POINTS = 5;

    Vector2dd tests[TEST_POINTS] = {
            Vector2dd( -0.1,  0.5),
            Vector2dd(  1.0,  1.0),
            Vector2dd( 0.25,  0.25),
            Vector2dd( 0.51,  0.5),
            Vector2dd( 0.49,  0.5)
    };

    bool results[TEST_POINTS] = {false, false, true, false, true};

    for (unsigned i = 0; i < TEST_POINTS; i++)
    {
        cout << tests[i] << " should be " << results[i] << " is " << p.isInside(tests[i]) << endl;
    }
        return 0;
}
