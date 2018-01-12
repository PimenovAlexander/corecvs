#ifndef FOLDERSCANNER_H
#define FOLDERSCANNER_H
/**
 * \file folderScanner.h
 *
 * \date Mar 11, 2016
 **/

#include <vector>
#include <string>

#include "core/utils/global.h"

using std::vector;
using std::string;

namespace corecvs {

class FolderScanner {

public:
    static bool scan(const string &path, vector<string> &childs, bool findFiles = true);

    static bool isDir(const string &path);

    static bool createDir(const string &path, bool allowRecursive = true);

    static void emptyDir(const string &path);       // delete folder to create it later probably

    static bool isAccessible(const string &path);
};

} // namespace corecvs

#endif // FOLDERSCANNER_H
