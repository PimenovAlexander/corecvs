#include "core/filesystem/folderScanner.h"
#include "core/utils/log.h"
#include "core/utils/utils.h"

#include <iostream>

#if !defined(WITH_STD_FILESYSTEM)
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#endif

namespace corecvs {

#if defined(WITH_STD_FILESYSTEM)

bool FolderScanner::isDir(const string &path)
{
    fs::path p(path);
    return fs::exists(p) && fs::is_directory(p);
}

bool FolderScanner::createDir(const string &path, bool allowRecursive)
{
    if (isDir(path))
        return true;

    std::cout << "creating dir <" << path << ">" << std::endl;

    bool res;
    try {
        fs::path p(path);
        res = allowRecursive ? fs::create_directories(p) : fs::create_directory(p);
    }
    catch (...) {
        L_ERROR_P("couldn't create dir <%s>", path.c_str());
        res = false;
    }
    return res;
}

bool FolderScanner::scan(const string &path, vector<string> &children, bool findFiles)
{
    if (!isDir(path))
    {
        L_ERROR_P("<%s> does not exist or not a directory", path.c_str());
        return false;
    }

    fs::path p(path);
    for (fs::directory_iterator it = fs::directory_iterator(p); it != fs::directory_iterator(); ++it)
    {
        fs::path pathChild(*it);            // pathChild has linux style slashes inside

        bool isDir = fs::is_directory(pathChild);

        //L_DDEBUG_P("%s contains\t%s\tas a %s", p.string().c_str(), pathChild.string().c_str(), (isDir ? "dir" : "file"));

        if (!(findFiles ^ isDir))
            continue;

        // win32: bugfix state:
        //  pathChild.string() - has linux   style slashes everywhere
        //  (string)pathChild  - has windows style slashes on vc12, but it's obsolete in vc14
#ifdef _MSC_VER
        childs.push_back(corecvs::HelperUtils::toNativeSlashes(pathChild.string()));
#else
        children.push_back(pathChild);
#endif
    }

    return true;
}

std::string FolderScanner::getDirectory(const std::string &absoluteFilePath)
{
    CORE_ASSERT_TRUE_S(!absoluteFilePath.empty());

    fs::path filePath(absoluteFilePath);
    return fs::absolute(filePath.parent_path()).string();
}

std::string corecvs::FolderScanner::getFileName(const std::string &fileName)
{
    fs::path filePath(fileName);
    return filePath.filename().string();
}

std::string FolderScanner::concatPath(const std::string &path1, const std::string &path2)
{
    return (fs::path(path1) / fs::path(path2)).string();
}

bool FolderScanner::isAbsolutePath(const std::string &path)
{
    return fs::path(path).is_absolute();
}

bool FolderScanner::pathExists(const std::string &path)
{
    return fs::exists(path);
}

bool FolderScanner::pathRemove(const std::string &path)
{
    fs::path p(path);
    if (fs::exists(p))
        return fs::remove(p);
    return false;
}

std::string FolderScanner::getFileNameIfExist(const std::string &fileName, const std::string &relativePath)
{
    fs::path filePath(fileName);
    if (fs::exists(filePath))
        return fileName;

    fs::path infoNew = fs::path(relativePath) / fs::path(fileName); /* this is concatenation */
    if (fs::exists(infoNew))
        return fs::absolute(infoNew).string();

    std::cout << "couldn't locate <" << fileName << "> with relativePath:" << relativePath << std::endl;
    return "";
}

bool isDirectory(const std::string &path)
{
    fs::path filePath(path);
    return fs::is_directory(filePath);
}

#else /* WITH_STD_FILESYSTEM */

std::string FolderScanner::concatPath(const std::string &path1, const std::string &path2)
{
    return path1 + PATH_SEPARATOR + path2;
}

bool FolderScanner::isAbsolutePath(const std::string &path)
{
    return HelperUtils::startsWith(path, PATH_SEPARATOR);
}

std::string FolderScanner::getDirectory(const std::string &absoluteFilePath)
{
    size_t found = absoluteFilePath.find_last_of(PATH_SEPARATOR);
    return absoluteFilePath.substr(0,found);
}

std::string corecvs::FolderScanner::getBaseName(const std::string &fileName)
{
    size_t found = fileName.find_last_of(PATH_SEPARATOR);
    return fileName.substr(found+1);
}

bool FolderScanner::isDirectory(const string &path)
{
    DIR *dp = opendir(path.c_str());
    if (dp == NULL)
        return false;

    closedir(dp);
    return true;
}

bool FolderScanner::createDir(const string &path, bool allowRecursive)
{
    if (isDirectory(path))
        return true;

    std::cout << "creating dir <" << path << ">" << std::endl;

    std::system(("mkdir " + path).c_str());

    if (!isDirectory(path))
    {
        if (!allowRecursive) {
            L_ERROR_P("couldn't create dir <%s>", path.c_str());
            return false;
        }
        L_INFO_P("creating subfolders of <%s>", path.c_str());

        auto subfolders = HelperUtils::stringSplit(path, PATH_SEPARATOR[0]);
        string p;
        for (auto& subfolder : subfolders)
        {
            if (!p.empty()) p += PATH_SEPARATOR;
            p += subfolder;

            if (!createDir(p, false))
                return false;
        }
    }
    return true;
}

bool FolderScanner::scan(const string &path, vector<string> &childs, bool findFiles)
{
    if (!isDirectory(path))
    {
        L_ERROR_P("<%s> does not exist or not a directory", path.c_str());
        return false;
    }

    DIR *dp = opendir(path.c_str());  CORE_ASSERT_TRUE_S(dp != NULL);

    struct dirent *ep;
    while ((ep = readdir(dp)) != NULL)
    {
        /* We need to form path */
        string childPath = path + PATH_SEPARATOR + ep->d_name;

        /* Ok there are devices, pipes, links... I don't know... */
        bool dir = (ep->d_type != DT_REG) && (ep->d_type != DT_LNK);
        if (ep->d_type == DT_UNKNOWN)
        {
            dir = isDirectory(childPath);
        }

        //L_DDEBUG_P("%s contains\t%s\tas a %s (d_type:0x%x)", path.c_str(), ep->d_name, (dir ? "dir" : "file"), ep->d_type);

        if (!(findFiles ^ dir))
            continue;

        childs.push_back(childPath);
    }

    closedir(dp);
    return true;
}

#endif

/**
 * This is a temporary lazy implementation used untill stupid C++ would have std::filesystem de facto integrated
 **/
bool FolderScanner::pathRemove(const string &path)
{
#ifdef WIN32
    std::system(("rd /s /q " + path).c_str());
#else
    int result = std::system(("rm -rf " + path).c_str());
    CORE_UNUSED(result);
#endif
    L_INFO_P("The <%s> path is deleted.", path.c_str());
    return true;
}

bool FolderScanner::isAccessible(const string &path)
{
    auto p = path;
    if (!STR_HAS_SLASH_AT_END(p))
        p += PATH_SEPARATOR;
    p += "checkFolderScanner.tmp";

    std::ofstream f(p, std::ios::app);
    if (f.is_open())
    {
        f.close();
        pathRemove(p);
        return true;
    }
    return false;
}

std::string FolderScanner::addFileExtIfNotExist(const std::string &fileName, const std::string &ext)
{
    return HelperUtils::endsWith(fileName, ext) ? fileName : fileName + ext;
}

bool FolderScanner::pathExists(const std::string &path)
{
    struct stat info;
    if( stat( path.c_str(), &info ) != 0 ) {
        return false;
    }

    return true;
}

} // namespace corecvs

