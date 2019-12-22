#include "core/filesystem/tempFolder.h"
#include "core/utils/log.h"
#include "core/utils/utils.h"
#include "core/filesystem/folderScanner.h"

//#include <mutex>

using std::vector;
using std::string;

namespace corecvs {

static string envBuildNumber = HelperUtils::getEnvVar("BUILD_NUMBER");
static string envBuildJob    = HelperUtils::getEnvVar("JOB_NAME");

string TempFolder::LocalTempPath(const string &projectEnviromentVariable)
{
    string res;
#ifdef WIN32
    res = HelperUtils::getEnvVar("TEMP");
    if (res.empty())
        res = HelperUtils::getEnvVar("TMP");
    if (res.empty())
    {
        L_ERROR_P("The <TEMP> enviroment variable is not set.");
        res = ".";
    }
#else
    res = "/tmp";
#endif
    if (!STR_HAS_SLASH_AT_END(res)) {
        res += PATH_SEPARATOR;                  // add slash if need
    }
    res += projectEnviromentVariable;
    if (STR_HAS_SLASH_AT_END(res)) {
        res.resize(res.length() - 1);           // kill the last slash
    }

    return res;
}

string TempFolder::TempFolderPath(const string &projectEnviromentVariable, bool clear, bool useLocal)
{
    static vector<string> clearedFolders;

    string res;
    if (useLocal)
    {
        res = LocalTempPath(projectEnviromentVariable);
        if (!envBuildJob.empty())
        {
            res += PATH_SEPARATOR;                      // add slash
            res += envBuildJob;
        }
    }
    else
    {
        res = UniqueBuildPath(projectEnviromentVariable) + PATH_SEPARATOR + "temp";
    }

    bool createFolder = false;
    if (FolderScanner::isDirectory(res))                  // dir exists: clear it if others have created it
    {
        if (clear)                                  // clear it only if others have created it, we're not
        {
            if (!contains(clearedFolders, projectEnviromentVariable))
            {                                       // delete folder to create it later
                FolderScanner::pathRemove(res);
                createFolder = true;
            }
        }
    }
    else
    {
        createFolder = true;                        // it doesn't exist, we need to create it
    }

    if (createFolder)
    {
        FolderScanner::createDir(res);              // folder creation with its subfolders

        // the created folder is automatically considered cleared
        clearedFolders.push_back(projectEnviromentVariable);
    }

    return res;
}

std::string TempFolder::UniqueBuildPath(const std::string &projectEnviromentVariable, const std::string &subfolderRelPathJen)
{
    string res = ".";
    if (projectEnviromentVariable.empty())
    {
        L_ERROR_P("The 'projectEnviromentVariable' is empty.");
        return res;
    }

    string projectPath = HelperUtils::getEnvVar(projectEnviromentVariable.c_str());
    if (projectPath.empty())
    {
        L_ERROR_P("The <%s> enviroment variable is not set.", projectEnviromentVariable.c_str());
        return res;
    }

    res = projectPath;
    if (STR_HAS_SLASH_AT_END(res))
        res.resize(res.length() - 1);               // kill slash if there's any

    if (!subfolderRelPathJen.empty())
    {
        if (subfolderRelPathJen[0] != PATH_SEPARATOR[0])
            res += PATH_SEPARATOR;

        res += HelperUtils::toNativeSlashes(subfolderRelPathJen);

        if (STR_HAS_SLASH_AT_END(res))
            res.resize(res.length() - 1);           // kill slash if there's any
    }

    if (!envBuildNumber.empty() && !envBuildJob.empty())
    {
        res += PATH_SEPARATOR + envBuildJob + "_" + envBuildNumber;
    }

    return res;
}

} // namespace corecvs
