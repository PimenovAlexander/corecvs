
#ifndef CORECVS_CONTENTPROVIDER_H
#define CORECVS_CONTENTPROVIDER_H

#include "memory"
#include "httpContent.h"

class ContentProvider
{
public:
    virtual std::shared_ptr<HttpContent> getContent(const std::string&) = 0;
};

#endif //CORECVS_CONTENTPROVIDER_H
