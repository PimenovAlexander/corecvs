#ifndef REFLECTIONLISTMODULE_H
#define REFLECTIONLISTMODULE_H

#include "modules/httpServerModule.h"
#include "core/reflection/lockableObject.h"


class ReflectionModuleDAO {
public:
    virtual std::vector<std::string>  getReflectionNames() = 0;
    virtual corecvs::LockableObject *getReflectionObject(std::string name) = 0;
};

class ReflectionModuleHashDAO : public ReflectionModuleDAO {
public:
    std::map<std::string, corecvs::LockableObject *> mReflectionsList;

    virtual vector<std::string> getReflectionNames() {

        vector<std::string> names;
        names.reserve(mReflectionsList.size());
        for (auto it : mReflectionsList) {
            names.push_back(it.first);
        }
        return names;
    }

    virtual corecvs::LockableObject *getReflectionObject(std::string name)
    {
        if (mReflectionsList.count(name) == 0)
            return NULL;
        return mReflectionsList[name];
    }
};


class ReflectionListModule : public HttpServerModule
{
public:

    ReflectionModuleDAO* mReflectionsDAO = NULL;

    virtual bool shouldProcessURL(std::string url);
    virtual bool shouldWrapURL(std::string url);
    virtual std::shared_ptr<HttpContent> getContentByUrl(std::string url);


    ReflectionListModule();
};

#endif // REFLECTIONLISTMODULE_H
