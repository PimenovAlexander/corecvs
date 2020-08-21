#ifndef REFLECTIONLISTMODULE_H
#define REFLECTIONLISTMODULE_H

#include "modules/httpServerModule.h"
#include "core/reflection/lockableObject.h"


class ReflectionModuleDAO {
public:
    virtual QList<QString>  getReflectionNames() = 0;
    virtual LockableObject *getReflectionObject(QString name) = 0;
};

class ReflectionModuleHashDAO : public ReflectionModuleDAO {
public:
    QHash<QString, LockableObject *> mReflectionsList;

    virtual QList<QString> getReflectionNames() {
        return mReflectionsList.keys();
    }

    virtual LockableObject *getReflectionObject(QString name)
    {
        if (!mReflectionsList.contains(name))
            return NULL;
        return mReflectionsList[name];
    }
};


class ReflectionListModule : public HttpServerModule
{
public:

    ReflectionModuleDAO* mReflectionsDAO;

    virtual bool shouldProcessURL(QUrl url);
    virtual bool shouldWrapURL(std::string url);
    virtual QSharedPointer<HttpContent> getContentByUrl(QUrl url);


    ReflectionListModule();
};

#endif // REFLECTIONLISTMODULE_H
