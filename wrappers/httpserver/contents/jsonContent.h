#ifndef   JSON_CONTENT_H_
#define JSON_CONTENT_H_

#include "httpContent.h"

class JSONContent : public HttpContent {
public:
    /* So far only boolean */
    bool result;
    std::string value;

    JSONContent();
    virtual ~JSONContent();

    void setResult(bool res);
    void setValue(std::string val);


    virtual std::vector<uint8_t> getContent();
    virtual std::string getContentType()
    {
        return "application/json";
    }

};

#endif // #ifndef JSON_CONTENT_H_
