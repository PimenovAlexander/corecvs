#include <iostream>

#include "core/utils/utils.h"
#include "httpUtils.h"

using namespace corecvs;
using std::string;
using std::vector;


std::vector<std::pair<std::string, std::string> > HttpUtils::parseParameters(std::string &url)
{
    SYNC_PRINT(("HttpUtils::parseParameters(%s): called\n", url.c_str()));
    std::vector<std::pair<std::string, std::string> > toReturn;

    vector<string> split1 = HelperUtils::stringSplit(url, '?');

    if (split1.size() != 2)
    {
        return toReturn;
    }

    vector<string> split2 = HelperUtils::stringSplit(split1[1], '&');
    for (size_t i = 0; i < split2.size(); i++)
    {
        vector<string> split3 = HelperUtils::stringSplit(split2[i], '=');

        if (split3.size() == 1) {
            toReturn.push_back(std::pair<std::string, std::string>(split3[0], ""));
        } else if (split3.size() == 2) {
            toReturn.push_back(std::pair<std::string, std::string>(split3[0], split3[1]));
        }
    }

    std::cout << "Parsed input:" << std::endl;
    for (size_t i = 0; i < toReturn.size(); i++)
    {
        std::cout << toReturn[i].first << " - " << toReturn[i].second << std::endl;
    }


    return toReturn;
}
