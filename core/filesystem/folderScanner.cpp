#include "core/filesystem/folderScanner.h"
#include "core/utils/log.h"
#include "core/utils/utils.h"

#include <iostream>

namespace corecvs {

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

bool FolderScanner::scan(const string &path, vector<string> &childs, bool findFiles)
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
        childs.push_back(pathChild);
#endif
    }

    return true;
}

#if defined(FILESYSTEM_WORKAROUND)

bool FolderScanner::isDir(const string &path)
{
    DIR *dp = opendir(path.c_str());
    if (dp == NULL)
        return false;

    closedir(dp);
    return true;
}

bool FolderScanner::createDir(const string &path, bool allowRecursive)
{
    if (isDir(path))
        return true;

    std::cout << "creating dir <" << path << ">" << std::endl;

    std::system(("mkdir " + path).c_str());

    if (!isDir(path))
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
    if (!isDir(path))
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
            dir = isDir(childPath);
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

void FolderScanner::emptyDir(const string &path)
{
#ifdef WIN32
    std::system(("rd /s /q " + path).c_str());
#else
    int result = std::system(("rm -rf " + path).c_str());
    CORE_UNUSED(result);
#endif
    L_INFO_P("The <%s> folder is deleted.", path.c_str());
}


} // namespace corecvs


/*
int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "Second arg is path" << std::endl;
        exit(-1);
    }
    fs::path p(argv[1]);
    if (!fs::exists(p))
    {
        std::cout << p << " does not exist" << std::endl;
        exit(-2);
    }
    if (!fs::is_directory(p))
    {
        std::cout << p << " is not a directory" << std::endl;
        exit(-3);
    }
    fs::directory_iterator d(p);

    for (fs::directory_iterator d = fs::directory_iterator(p); d != fs::directory_iterator(); ++d)
    {
        fs::path p_child(*d);
        std::cout << p << " contains " << p_child << " as a " << (fs::is_directory(p_child) ? "dir" : "file") << std::endl;
    }
    return 0;
}
*/
