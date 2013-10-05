/**
 * \file main_test_logger.cpp
 * \brief This is the main file for the test logger 
 *
 * \date Sep 26, 2012
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>

#ifndef ASSERTS
#define ASSERTS
#endif

#ifndef TRACE
#define TRACE
#endif

#include "global.h"

#include "log.h"


using namespace std;

int main (int /*argC*/, char ** /*argV*/)
{
 //   L_ERROR("Test with place");

  //  Log().info() << "Hello";

    Log logger;
    Log::MessageScoped(&logger, Log::LEVEL_ERROR, "A", 0, "F");

    cout << Log::formatted("Here we go %d\n", 1, 2, "three");

    cout << "PASSED" << endl;
    return 0;
}
