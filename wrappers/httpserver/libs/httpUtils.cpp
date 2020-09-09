#include <iostream>

#include "core/utils/utils.h"
#include "httpUtils.h"

using namespace corecvs;
using std::string;
using std::vector;
using std::pair;



vector<pair<string, string> > HttpUtils::parseParameters(const std::string &url)
{
    SYNC_PRINT(("HttpUtils::parseParameters(%s): called\n", url.c_str()));
    vector<pair<string, string> > toReturn;

    string paramString = getParameters(url);

    vector<string> items = HelperUtils::stringSplit(paramString, '&');
    for (const string& item: items)
    {
        vector<string> args = HelperUtils::stringSplit(item, '=');

        if (args.size() == 1)
            toReturn.emplace_back(args[0], "");
        if (args.size() == 2)
            toReturn.emplace_back(args[0], args[1]);
    }

#define PRINT_HTTPUTILS_LOGS
#ifdef PRINT_HTTPUTILS_LOGS
    std::cout << "Parsed input:" << std::endl;
    for (const auto& args: toReturn)
    {
        std::cout << "   " << args.first << " - " << args.second << std::endl;
    }
#endif

    return toReturn;
}

std::string HttpUtils::getPath(const std::string &url)
{
    std::size_t found = url.find_last_of('?');
    return url.substr(0, found);
}

std::string HttpUtils::getParameters(const std::string &url)
{
    std::size_t found = url.find_last_of('?');
    return url.substr(found + 1);
}

std::string HttpUtils::extentionToMIME(const std::string &path)
{
    if (corecvs::HelperUtils::endsWith(path, ".js")) {
        return "text/javascript";
    }

    if (corecvs::HelperUtils::endsWith(path, ".bmp")) {
        return "image/bmp";
    }

    return "text/html";
}
