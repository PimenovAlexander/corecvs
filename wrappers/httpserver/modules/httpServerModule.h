#ifndef HTTPSERVERMODULE_H
#define HTTPSERVERMODULE_H

#include <memory>

#include "httpContent.h"

class HttpServerModule
{
protected:
    bool mShouldWrap;
    std::string mPrefix;

public:
    HttpServerModule(bool shouldWrap = true) :
        mShouldWrap(shouldWrap)
    {}

    void setPrefix(std::string string) {
        mPrefix = string;
    }

#if 0
    /* Rewrite input */
    virtual bool checkAndRewrite(QUrl &url)
    {
        if (!mPrefix.isEmpty())
        {
            if (!url.path().startsWith(mPrefix))
            {
                return false;
            }
            url.setPath(QString("/") + url.path().right(mPrefix.length()));
        }
        return true;
    }

    virtual bool shouldProcess(QUrl url)
    {
        if (!checkAndRewrite(url))
        {
                return false;
        }
        return shouldProcessURL(url);
    }

    virtual bool shouldWrap(QUrl url)
    {
        if (!checkAndRewrite(url))
        {
                return false;
        }
        return shouldWrapURL(url);
    }

    virtual QSharedPointer<HttpContent> getContent(QUrl url)
    {
        if (!checkAndRewrite(url))
        {
                return QSharedPointer<HttpContent>();
        }
        return getContentByUrl(url);
    }

#endif
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
