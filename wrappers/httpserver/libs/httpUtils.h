#ifndef HTTP_UTILS_H
#define HTTP_UTILS_H

#include <vector>
#include <string>
#include <utility>

#include "core/utils/utils.h"

class HttpUtils {
public:
    static std::vector<std::pair<std::string, std::string>> parseParameters(std::string &url);

};


#endif //HTTP_UTILS_H
