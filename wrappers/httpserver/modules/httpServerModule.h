#ifndef HTTPSERVERMODULE_H
#define HTTPSERVERMODULE_H

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

protected:
    /* Functions to be overloaded */
    virtual bool shouldProcessURL(QUrl /*url*/)
    {
        return false;
    }

    virtual bool shouldWrapURL(QUrl /*url*/)
    {
        return mShouldWrap;
    }

    virtual QSharedPointer<HttpContent> getContentByUrl(QUrl url) = 0;

#endif

};

#endif // HTTPSERVERMODULE_H
