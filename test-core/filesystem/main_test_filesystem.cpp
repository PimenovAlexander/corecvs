/**
 * \file main_test_filesystem.cpp
 * \brief This is the main file for the test filesystem
 *
 * \date Mar 11, 2016
 * \author sf
 * \ingroup autotest
 */
#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/filesystem/folderScanner.h"

using namespace corecvs;

TEST(Filesystem, scanDirCheckKnownFiles)
{
    const string pathDir = ".";
    vector<string> childs;
    vector<string> files = {
          pathDir + PATH_SEPARATOR + "main.cpp"
        , pathDir + PATH_SEPARATOR + "cvs.pro"
        , pathDir + PATH_SEPARATOR + "cvs-open.pro"
    };

    bool res = FolderScanner::scan(pathDir, childs);
    CORE_ASSERT_TRUE_S(res);

    bool isOk = false;

    std::cout << "current folder has " << childs.size() << " files:" << std::endl;
    for (string& child : childs)
    {
        std::cout << child << std::endl;

        if (contains(files, child))
            isOk = true;
    }

    CORE_ASSERT_TRUE(isOk, "current folder must have one of the files: main.cpp or cvs.pro!");
}

TEST(Filesystem, isDir)
{
    bool res = FolderScanner::isDirectory(".");
    CORE_ASSERT_TRUE(res, "current folder must be a dir");
}
