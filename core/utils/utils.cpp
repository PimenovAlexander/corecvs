/**
 * \file utils.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Apr 14, 2010
 * \author alexander
 */

#if !defined( DSP_TARGET ) && !defined( WIN32 ) && !defined( WIN64 )
#include <iostream>
#include <exception>
#include <cstdlib>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <execinfo.h>
#endif
#include <sstream>

#include "core/utils/utils.h"
namespace corecvs {

namespace HelperUtils {

istream &getlineSafe(istream &is, string &str)
{
    istream& stream = getline(is, str);

    if (str.length() != 0 &&  *str.rbegin() == '\r') {
        str.erase(str.length() - 1);
    }

    return stream;
}

std::string removeLeading(const string &str, const string &symbols)
{
    const auto strBegin = str.find_first_not_of(symbols);
    if (strBegin == std::string::npos)
        return std::string(); // no content

    return str.substr(strBegin);
}

bool startsWith(const string &str, const string &prefix)
{
    return (str.compare(0, prefix.size(), prefix) == 0);
}

bool endsWith(const std::string &str, const std::string &postfix)
{
    if (str.length() < postfix.length())
        return false;
    return (str.compare(str.length() - postfix.length(), postfix.length(), postfix) == 0);
}


string getEnvDirPath(cchar *envVarName)
{
    string toReturn = getEnvVar(envVarName);
    if (toReturn.empty()) {
        CORE_ASSERT_FAIL_P(("Missed environment variable %s", envVarName));
        return "";
    }

    if (!STR_HAS_SLASH_AT_END(toReturn)) {
        toReturn += PATH_SEPARATOR;
    }
    return toReturn;
}

string getEnvVar(cchar *envVarName)
{
    cchar* var = std::getenv(envVarName);
    if (var == NULL || var[0] == 0) {
        return "";
    }
    return var;
}

static string replaceSlashes(const string& str, const string& oldStr, const string& newStr)
{
    size_t pos = 0;
    string s = str;
    while ((pos = str.find(oldStr, pos)) != string::npos)
    {
        s.replace(pos, oldStr.length(), newStr);
        pos += newStr.length();
    }
    return s;
}

string toNativeSlashes(const string& str)
{
#ifdef WIN32
    return replaceSlashes(str, "/", PATH_SEPARATOR);
#else
    return replaceSlashes(str, "\\", PATH_SEPARATOR);
#endif
}

void stringSplit(const string &s, char delim, std::vector<string> &elems)
{
    std::stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

std::vector<string> stringSplit(const string &s, char delim)
{
    std::vector<string> elems;
    stringSplit(s, delim, elems);
    return elems;
}

std::string stringCombine(std::vector<std::string> parts, char delim)
{
    std::string toReturn;
    for(size_t i = 0; i < parts.size(); i++)
    {
        if (i != 0) {
            toReturn += delim;
        }
        toReturn.append(parts[i]);
    }
    return toReturn;
}

string getFullPath(const string& envDirPath, cchar* path, cchar* filename)
{
    CORE_ASSERT_TRUE_S(path != NULL);

    if (filename == NULL)
        filename = "";

    if (envDirPath.empty())
        return filename;

    string res(envDirPath);

    if (STR_HAS_SLASH_AT_END(res) && IS_SLASH_SYMBOL(path[0]))   // check for doubled slash in given components
        res.resize(res.length() - 1);

    res += path;

    if (*filename)
    {
        if (!STR_HAS_SLASH_AT_END(res) && !IS_SLASH_SYMBOL(filename[0]))   // check for absent slash in given components
            res += PATH_SEPARATOR;

        res += filename;
    }

    return toNativeSlashes(res);
}

string escapeString(const string &s, const std::unordered_map<char, char> &symbols, const string &escape)
{
     std::ostringstream out;

     for (const char &symbol : s)
     {

         auto p = symbols.find(symbol);
         if (p != symbols.end())
         {
             out << escape;
             out << p->second;
         } else {
             out << symbol;
         }
     }
     return out.str();
}

string unescapeString(const string &s, const std::unordered_map<char, char> &symbols, char guard)
{
    std::ostringstream out;
    for (size_t n = 0; n < s.size(); n++)
    {
        if (s[n] == guard && n < s.size() - 1)
        {
            auto p = symbols.find(s[n + 1]);
            if (p != symbols.end())
            {
                out << p->second;
            } else {
                out << guard;
            }
            n++;
        } else {
            out << s[n];
        }
    }
    return out.str();
}

std::string addFileExtIfNotExist(const std::string &fileName, const std::string &ext)
{
    return endsWith(fileName, ext) ? fileName : fileName + ext;
}

std::string getDirectory(const std::string &absoluteFilePath)
{
    CORE_ASSERT_TRUE_S(!absoluteFilePath.empty());

    fs::path filePath(absoluteFilePath);
    return fs::absolute(filePath.parent_path()).string();

/*    QFileInfo info(absoluteFilePath);
    return info.dir().absolutePath();*/
}

std::string getFileName(const std::string &fileName)
{
    fs::path filePath(fileName);
    return filePath.filename().string();

    /*QFileInfo info(fileName);
    return info.fileName();*/
}

std::string concatPath(const std::string &path1, const std::string &path2)
{
   return (fs::path(path1) / fs::path(path2)).string();
}

bool isAbsolutePath(const std::string &path)
{
   return fs::path(path).is_absolute();
}

bool pathExists(const std::string &path)
{
    return fs::exists(path);
}

std::string getFileNameIfExist(const std::string &fileName, const std::string &relativePath)
{
    //std::cout << fileName.toStdString() << std::endl;

    fs::path filePath(fileName);
    if (fs::exists(filePath))
        return fileName;

    fs::path infoNew = fs::path(relativePath) / fs::path(fileName); /* this is concatenation */
    if (fs::exists(infoNew))
        return fs::absolute(infoNew).string();
    std::cout << "couldn't locate <" << fileName << "> with relativePath:" << relativePath << std::endl;
    return "";

#if 0
    QFileInfo info(fileName);
    if (info.exists())
        return fileName;

    QFileInfo infoNew(relativePath + PATH_SEPARATOR + info.fileName());
    if (infoNew.exists())
        return infoNew.absoluteFilePath();

    std::cout << "couldn't locate <" << fileName.toStdString() << "> with relativePath:" << relativePath.toStdString() << std::endl;
    return "";
#endif
}

} // namespace HelperUtils

} //namespace corecvs


#if defined(DSP_TARGET) || defined(WIN32) || defined(WIN64)

// It is possible but quite hard and usually not needed to print stack trace on Win32.
// Debugging shall be done with debugger when possible, if not, minidump is better than stack trace
// http://stackoverflow.com/questions/105659/how-can-one-grab-a-stack-trace-in-c/127012#127012
//
void setStdTerminateHandler() {}

#else

static void stdTerminateHandler()
{
    printStackTrace();
    exit(1);
}

void setStdTerminateHandler()
{
    std::set_terminate(stdTerminateHandler);
}

#endif // DSP_TARGET
