/**
 * \file main_test_camerafixture.cpp
 * \brief This is the main file for the test camerafixture 
 *
 * \date дек. 30, 2015
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include <sstream>
#include "gtest/gtest.h"

#include "core/joystick/playbackJoystickInterface.h"

using namespace corecvs;
using namespace std;

TEST(Joystick, LoadSave)
{    
    JoystickFileFormat loaderSaver;


    const char *data =
    "8 11 \n"
    "2017839883 0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0 10 0 11 0 12 0 13 0 14 1 15 0 16 0 17 0 18 0 \n"
    "2021836568 1 1554 \n"
    "2025837201 1 1036 \n"
    "2025847759 4 7770 \n"
    "2029826509 1 777 \n"
    "2029836169 4 7511 \n"
    "2033836986 1 -2 \n"
    "2033853409 4 6993 \n";

    istringstream ss(data);

    loaderSaver.load(ss);
    ASSERT_TRUE(loaderSaver.configuration.axisNumber   == 8);
    ASSERT_TRUE(loaderSaver.configuration.buttonNumber == 11);

    cout << "States:" << loaderSaver.states.size() << endl;
    //ASSERT_TRUE(loaderSaver.states.size() == 7);


    cout << "====================" << endl;
    cout << data;
    cout << "====================" << endl;
    loaderSaver.save(cout);

}

