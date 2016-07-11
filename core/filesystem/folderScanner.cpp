#include "folderScanner.h"
#include "log.h"

#include <iostream>

#ifdef __GNUC__
#ifdef CORE_UNSAFE_DEPS
# include <experimental/filesystem>
  namespace fs = std::experimental::filesystem;  
#else
    #include <sys/types.h>
    #include <dirent.h>
#endif
#endif


#ifdef _MSC_VER
# include <filesystem>
  namespace fs = std::tr2::sys;
#endif

namespace corecvs {


#if defined(CORE_UNSAFE_DEPS) || defined (_MSC_VER)

bool FolderScanner::scan(const string &path, vector<string> &childs, bool findFiles)
{
    fs::path p(path);
    if (!fs::exists(p))
    {
        L_ERROR_P("<%s> does not exist", p.string().c_str());
        return false;
    }
    if (!fs::is_directory(p))
    {
        L_ERROR_P("<%s> is not a directory", p.string().c_str());
        return false;
    }

    for (fs::directory_iterator it = fs::directory_iterator(p); it != fs::directory_iterator(); ++it)
    {
        fs::path pathChild(*it);            // pathChild has linux style slashes inside
        bool isDir = fs::is_directory(pathChild);

        L_DDEBUG_P("%s contains\t%s\tas a %s", p.string().c_str(), pathChild.string().c_str(), (isDir ? "dir" : "file"));

        if (!(findFiles ^ isDir))
            continue;

        childs.push_back(pathChild);        // string(pathChild) has native platform style slashes inside
    }

    return true;
}

#else

bool FolderScanner::scan(const string &path, vector<string> &childs, bool findFiles)
{
     DIR *dp;
     struct dirent *ep;

     dp = opendir (path.c_str());
     if (dp == NULL) {
         return false;
     }


     while ((ep = readdir (dp)) != NULL) {

       /*Ok there are devices, pipes, links... I don't know... */
       bool isDir = (ep->d_type != DT_REG) && (ep->d_type != DT_LNK);
       if (isDir ^ findFiles) {
           //SYNC_PRINT(("found path: %s\n", ep->d_name));

           /*Do we need to form path? */
           string result = path + PATH_SEPARATOR + ep->d_name;
           childs.push_back(result);
       }
     }

     closedir (dp);
     return true;
}
#endif


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
