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
