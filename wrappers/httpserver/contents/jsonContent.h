#ifndef   JSON_CONTENT_H_
#define JSON_CONTENT_H_

#include "httpContent.h"

class JSONContent : public HttpContent {
public:
    JSONContent();
    JSONContent(QVariantMap vars);


    virtual ~JSONContent();

    void setResult(bool res);
    void setValue(QVariant const &value);


    virtual std::vector<uint8_t> getContent();
    virtual std::string getContentType()
    {
        return "application/json";
    }

private:
    QVariantMap mVariants;
};

#endif // #ifndef JSON_CONTENT_H_
