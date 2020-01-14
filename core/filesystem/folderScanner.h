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
    static bool scan(const string &path, vector<string> &children, bool findFiles = true);

    static bool isDirectory(const string &path);

    static bool createDir(const string &path, bool allowRecursive = true);

    //static void emptyDir(const string &path);       // delete folder to create it later probably
    static bool pathRemove(const std::string &path);

    static bool isAccessible(const string &path);

    static std::string addFileExtIfNotExist(const std::string &fileName, const std::string &ext);

    static std::string getDirectory(const std::string &absoluteFilePath);

    static std::string getBaseName(const std::string &fileName);

    static std::string concatPath(const std::string &path1, const std::string &path2);

    static bool isAbsolutePath(const std::string &path);

    static bool pathExists(const std::string &path);

    std::string getFileNameIfExist(const std::string &fileName, const std::string &relativePath);
};


} // namespace corecvs

#endif // FOLDERSCANNER_H
