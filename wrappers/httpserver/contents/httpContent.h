#ifndef HTTP_CONTENT_H_
#define HTTP_CONTENT_H_

#include "stdint.h"

#include <string>
#include <vector>

class HttpContent
{
public:
    bool mRewritable;
    bool mIsLongpoll;

    HttpContent() :
        mRewritable(false),
        mIsLongpoll(false)
    {}

    virtual std::vector<uint8_t> getContent() = 0;
    virtual std::string getContentType()
    {
        return "text/html";
    }
    virtual ~HttpContent() {}
};

#endif //HTTP_CONTENT_H_
