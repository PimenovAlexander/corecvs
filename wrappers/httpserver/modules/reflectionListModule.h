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

    vector<std::string> getReflectionNames() override {
        vector<std::string> names;
        names.reserve(mReflectionsList.size());
        for (const auto& it : mReflectionsList) {
            names.push_back(it.first);
        }
        return names;
    }

    corecvs::LockableObject *getReflectionObject(std::string name) override
    {
        if (mReflectionsList.count(name) == 0)
            return nullptr;
        return mReflectionsList[name];
    }
};


class ReflectionListModule : public HttpServerModule
{
public:

    ReflectionModuleDAO* mReflectionsDAO = nullptr;

    bool shouldProcessURL(const std::string& url) override;
    bool shouldWrapURL(const std::string& url) override;
    std::shared_ptr<HttpContent> getContentByUrl(const std::string& url) override;


    ReflectionListModule();
};

#endif // REFLECTIONLISTMODULE_H
