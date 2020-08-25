#ifndef HTTPSERVERMODULE_H
#define HTTPSERVERMODULE_H

#include <memory>

#include "httpContent.h"
#include "core/utils/utils.h"

class HttpServerModule
{
public:
    bool mShouldWrap;
    std::string mPrefix;

public:
    HttpServerModule(bool shouldWrap = true) :
        mShouldWrap(shouldWrap)
    {}

    void setPrefix(std::string string) {
        mPrefix = string;
    }

    /* Rewrite input */
    virtual bool checkAndRewrite(std::string &url)
    {
        if (!mPrefix.empty())
        {
            if (!corecvs::HelperUtils::startsWith(url, mPrefix))
            {
                return false;
            }
            url = std::string("/") + url.substr(mPrefix.length());
        }
        return true;
    }

    virtual bool shouldProcess(std::string url)
    {
        if (!checkAndRewrite(url))
        {
                return false;
        }
        SYNC_PRINT(("Prefix is ok. Checking rest url\n"));
        return shouldProcessURL(url);
    }

#if 0
    virtual bool shouldWrap(QUrl url)
    {
        if (!checkAndRewrite(url))
        {
                return false;
        }
        return shouldWrapURL(url);
    }
#endif

    virtual std::shared_ptr<HttpContent> getContent(std::string url)
    {
        if (!checkAndRewrite(url))
        {
                return std::shared_ptr<HttpContent>();
        }
        return getContentByUrl(url);
    }

protected:
    /* Functions to be overloaded */
    virtual bool shouldProcessURL(std::string /*url*/)
    {
        return false;
    }

    virtual bool shouldWrapURL(std::string /*url*/)
    {
        return mShouldWrap;
    }

    virtual std::shared_ptr<HttpContent> getContentByUrl(std::string url) = 0;


};

#endif // HTTPSERVERMODULE_H
