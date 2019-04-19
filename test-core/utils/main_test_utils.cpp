/**
 * \file main_test_utils.cpp
 * \brief This is the main file for the test utils 
 *
 * \date мар 16, 2017
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <set>
#include <unordered_map>
#include <string>

#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"
#include "core/utils/utils.h"

using namespace std;
using namespace corecvs;

/**
   JSON Escapes

    Backspace is replaced with \b
    Form feed is replaced with \f
    Newline is replaced with \n
    Carriage return is replaced with \r
    Tab is replaced with \t
    Double quote is replaced with \"
    Backslash is replaced with \\

 **/

TEST(utils, testEscape)
{
    std::unordered_map<char,char> replace = {
        {'\b', 'b'}, {'\f', 'f'}, {'\n', 'n'}, {'\r', 'r'}, {'\t', 't'}, {'\"', '"'}, {'\\', '\\'}};

    std::unordered_map<char,char> replace_back = {
        {'b', '\b'}, {'f', '\f'}, {'n', '\n'}, {'r', '\r'}, {'t', '\t'}, {'"', '\"'}, {'\\', '\\'}};

    std::string s("Test\b\f\n\r\t\"\\");
    std::string res = HelperUtils::escapeString(s, replace, "\\");
    cout << "Escaped <" << res << ">" << endl;

    std::string back = HelperUtils::unescapeString(res, replace_back, '\\');

    CORE_ASSERT_TRUE(s.size() == back.size(), "Escape failed. Different size");
    for (string::size_type i = 0; i < min(s.size(), back.size()); i++)
    {
        SYNC_PRINT(("%d %d\n", (int)s[i], (int)back[i]));
    }

    CORE_ASSERT_TRUE(s == back, "Escape failed");
}


TEST(utils, splitString)
{
    std::string name = "/media/workarea/work/flow/data/kitti_scene//training/image_2/000002_10.png";
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    std::vector<string> metaVector = HelperUtils::stringSplit(name, ',');
    cout << metaVector.size() << std::endl;
}

TEST(utils, splitStringMulti)
{
    {
        std::string points = "220,10 300,210 170,250 123,234";
        std::vector<string> pointsVector = HelperUtils::stringSplit(points, " ,");

        SYNC_PRINT(("%s\n", points.c_str()));
        for(size_t i = 0; i < pointsVector.size(); i++)
           SYNC_PRINT(("<%s>  ", pointsVector[i].c_str()));
        SYNC_PRINT(("\n"));
        cout << pointsVector.size() << std::endl;
        CORE_ASSERT_TRUE(pointsVector.size() == 8, "splitString failed");
    }

    {
        std::string points = "220,10 300.8,210    170.3,250.2 123,234";
        std::vector<string> pointsVector = HelperUtils::stringSplit(points, " ,");

        SYNC_PRINT(("%s\n", points.c_str()));
        for(size_t i = 0; i < pointsVector.size(); i++)
           SYNC_PRINT(("<%s>  ", pointsVector[i].c_str()));
        SYNC_PRINT(("\n"));
        cout << pointsVector.size() << std::endl;
        CORE_ASSERT_TRUE(pointsVector.size() == 8, "splitString failed");
    }


}
