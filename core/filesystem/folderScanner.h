#ifndef FOLDERSCANNER_H
#define FOLDERSCANNER_H
/**
 * \file folderScanner.h
 *
 * \date Mar 11, 2016
 **/

#include <vector>
#include <string>

#include "global.h"

using std::vector;
using std::string;

namespace corecvs {

class FolderScanner {

public:
    static bool scan(const string &path, vector<string> &childs, bool findFiles = true);

};

} // namespace corecvs

#endif // FOLDERSCANNER_H
