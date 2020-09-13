#ifndef HTTPSERVERMODULE_H
#define HTTPSERVERMODULE_H

#include <memory>
#include <utility>

#include "httpContent.h"
#include "core/utils/utils.h"
#include "contentProvider.h"

class HttpServerModule : ContentProvider
{
private:
    std::string mPrefix;
    bool mShouldWrap;

public:
    HttpServerModule(bool shouldWrap = true) :
        mShouldWrap(shouldWrap)
    {}

    std::string getPrefix() {
        return mPrefix;
    }

    void setPrefix(std::string prefix) {
        mPrefix = std::move(prefix);
    }

    /* Rewrite input */
    bool checkAndRewrite(std::string& url)
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

    bool shouldProcess(const std::string& url)
    {
        std::string urlPath(url);
        if (!checkAndRewrite(urlPath))
        {
            return false;
        }
        SYNC_PRINT(("Prefix is ok. Checking the rest of the url\n"));
        return shouldProcessURL(urlPath);
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

    std::shared_ptr<HttpContent> getContent(const std::string& url) override
    {
        std::string urlPath(url);
        if (!checkAndRewrite(urlPath))
        {
            return std::shared_ptr<HttpContent>();
        }
        return getContentByUrl(urlPath);
    }

    bool shouldPoll(const std::string& url)
    {
        std::string urlPath(url);
        if (checkAndRewrite(urlPath))
            return shouldPollURL(urlPath);
        return false;
    }

protected:
    /* Functions to be overloaded */
    virtual bool shouldProcessURL(const std::string& /*url*/)
    {
        return false;
    }

    virtual bool shouldPollURL(const std::string& /*url*/)
    {
        return false;
    }

    virtual bool shouldWrapURL(const std::string& /*url*/)
    {
        return mShouldWrap;
    }

    virtual std::shared_ptr<HttpContent> getContentByUrl(const std::string& url) = 0;

};

#endif // HTTPSERVERMODULE_H
