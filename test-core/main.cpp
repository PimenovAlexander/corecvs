#include <stdlib.h>
#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/math/vector/fixedArray.h"

using namespace std;
using namespace corecvs;

static bool checkFileExist(const string& dirPath, const string& filePath)
{
    string path = dirPath;
    if (dirPath.length() > 0 && dirPath[dirPath.length() - 1] != PATH_SEPARATOR[0]) {
        path += PATH_SEPARATOR;
    }
    path += filePath;
    cout << "checking for the file <" << path << ">" << endl;
    return access(path.c_str(), 0) == 0;
}

static bool checkFileExist(const char* dirName, const string& filePath)
{
    cchar* dir = getenv(dirName);
    if (dir == NULL) {
        //FAIL();
        CORE_ASSERT_FAIL_P(("Missed environment variable %s", dirName));
        return false;
    }
    //SUCCEED();
    cout << dirName << "=" << dir << endl;

    return checkFileExist(string(dir), filePath);
}

// Check existance for the "data/pair/image0001_c0.pgm"
TEST(EnvTest, CheckCurrentDirTest)
{
    string filePath = string("data")
        + PATH_SEPARATOR + "pair"
        + PATH_SEPARATOR + "image0001_c0.pgm";

    CORE_ASSERT_TRUE(checkFileExist(string("."), filePath), "Missed expected repo DB at the current folder");
}

TEST(VectorTest, MulAllElements)
{
    const int LENGTH = 8;
    FixedArray<int> arr(LENGTH);
    ASSERT_EQ(arr.size(), LENGTH);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
