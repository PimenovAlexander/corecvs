#ifndef HTTP_UTILS_H
#define HTTP_UTILS_H

#include <vector>
#include <string>
#include <utility>

#include "core/utils/utils.h"

class HttpUtils {
public:
    static std::vector<std::pair<std::string, std::string>> parseParameters(std::string &url);

    /* Throw away parameters*/
    static std::string getPath(const std::string &url);

    /* Get parameter part*/
    static std::string getParameters(const std::string &url);

    static std::string extentionToMIME(const std::string &path);
};


#endif //HTTP_UTILS_H
