#ifndef REFLECTION_CONTENT_H_
#define REFLECTION_CONTENT_H_

#include "httpContent.h"
#include "core/reflection/lockableObject.h"


class ReflectionListContent : public HttpContent
{
public:
    ReflectionListContent(std::vector<std::string> names);
    virtual std::vector<uint8_t> getContent();
    virtual std::string getContentType();

private:
    std::vector<std::string> mNames;
};

class ReflectionContent : public HttpContent
{
public:
    bool mSimple;

    ReflectionContent(corecvs::LockableObject *reflection);
    virtual std::vector<uint8_t> getContent();
    virtual std::string getContentType();

    bool changeValue(const QUrl &url, QVariant &realValue);

    corecvs::LockableObject *mReflection;
};

#endif // REFLECTION_CONTENT_H_
