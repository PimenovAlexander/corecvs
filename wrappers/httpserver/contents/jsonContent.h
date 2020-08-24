#ifndef   JSON_CONTENT_H_
#define JSON_CONTENT_H_

#include "httpContent.h"

class JSONContent : public HttpContent {
public:
    JSONContent();

    virtual ~JSONContent();

    void setResult(bool res);


    virtual std::vector<uint8_t> getContent();
    virtual std::string getContentType()
    {
        return "application/json";
    }

};

#endif // #ifndef JSON_CONTENT_H_
