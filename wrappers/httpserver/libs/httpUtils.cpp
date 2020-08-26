#include <iostream>

#include "core/utils/utils.h"
#include "httpUtils.h"

using namespace corecvs;
using std::string;
using std::vector;
using std::pair;



vector<pair<string, string> > HttpUtils::parseParameters(std::string &url)
{
    SYNC_PRINT(("HttpUtils::parseParameters(%s): called\n", url.c_str()));
    vector<pair<string, string> > toReturn;

    string paramString = getParameters(url);

    vector<string> split2 = HelperUtils::stringSplit(paramString, '&');
    for (size_t i = 0; i < split2.size(); i++)
    {
        vector<string> split3 = HelperUtils::stringSplit(split2[i], '=');

        if (split3.size() == 1) {
            toReturn.push_back(pair<string, string>(split3[0], ""));
        } else if (split3.size() == 2) {
            toReturn.push_back(pair<string, string>(split3[0], split3[1]));
        }
    }

    std::cout << "Parsed input:" << std::endl;
    for (size_t i = 0; i < toReturn.size(); i++)
    {
        std::cout << toReturn[i].first << " - " << toReturn[i].second << std::endl;
    }


    return toReturn;
}

std::string HttpUtils::getPath(const std::string &url)
{
    std::size_t found = url.find_last_of("?");
    return url.substr(0, found);
}

std::string HttpUtils::getParameters(const std::string &url)
{
    std::size_t found = url.find_last_of("?");
    return url.substr(found + 1);
}

std::string HttpUtils::extentionToMIME(const std::string &path)
{
    std::string name(path);
    if (corecvs::HelperUtils::endsWith(name, ".js")) {
        return "text/javascript";
    }

    if (corecvs::HelperUtils::endsWith(name, ".bmp")) {
        return "image/bmp";
    }

    return "text/html";
}
