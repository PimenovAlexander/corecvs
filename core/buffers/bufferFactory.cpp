/**
 * \file bufferFactory.cpp
 * \brief Holds the implementation of the factory for the loadable buffer
 *
 * \ingroup cppcorefiles
 * \date Jun 22, 2010
 * \author alexander
 */
#include "core/buffers/bufferFactory.h"
#include "core/fileformats/ppmLoader.h"
#include "core/fileformats/rawLoader.h"
#include "core/fileformats/bmpLoader.h"

//#if __cplusplus > 199711L
#if defined(WIN32) && (_MSC_VER < 1900) // we need a threadsafety singleton initialization described in paragraph 6.7.4 of the C++11 standard, msvc2013 doesn't support it fully... Don't care about gcc-versions
#include <mutex>
#include <atomic>
static std::mutex mtx;
std::atomic<corecvs::BufferFactory *> instance(nullptr);
#endif

namespace corecvs {

//static
BufferFactory* BufferFactory::getInstance()
{
#if defined(WIN32) && (_MSC_VER < 1900) // we need a threadsafety singleton initialization
    auto *p = instance.load(std::memory_order_acquire);
    if (!p)
    {
        std::lock_guard<std::mutex> lck(mtx);
        p = instance.load(std::memory_order_relaxed);
        if (!p)
        {
            p = new BufferFactory;
            instance.store(p, std::memory_order_release);
        }
    }
    return p;
#else
    static BufferFactory instance;
    return &instance;
#endif
}

BufferFactory::BufferFactory()
{
    registerLoader(new PPMLoaderG12());
    registerLoader(new RAWLoaderG12());
    registerLoader(new BMPLoaderG12());
    registerLoaderG16(new PPMLoaderG16());     // specific reader to load 16bits data PPMs

    registerLoader(new PPMLoaderRGB24());
    registerLoader(new BMPLoaderRGB24());
    registerLoader(new RAWLoaderRGB24());

    registerSaver(new BMPSaverRGB24());
}

void BufferFactory::printCaps()
{
    BufferFactory *factory = BufferFactory::getInstance();

    cout << "BufferFactory knows:" << endl;

    cout << "  G12 loader" << endl;
    for (auto it : factory->mLoadersG12) cout << "\t" << it->name() << endl;

    cout << "  G16 loader" << endl;
    for (auto it : factory->mLoadersG16) cout << "\t" << it->name() << endl;

    cout << "  RGB24 loader" << endl;
    for (auto it : factory->mLoadersRGB24) cout << "\t" << it->name() << endl;

    cout << "  RuntimeType loader" << endl;
    for (auto it : factory->mLoadersRuntime) cout << "\t" << it->name() << endl;

    cout << "  RGB24 saver" << endl;
    for (auto it : factory->mSaversRGB24) cout << "\t" << it->name() << endl;
}

template<typename BufferType>
BufferType *loadBuffer(string name, vector<BufferLoader<BufferType> *> &loaders, const string &loaderHint)
{
    vector<size_t> idxs;
    for (size_t i = 0; i < loaders.size(); ++i)
    {
        if (!(loaders[i]->acceptsFile(name)))
            continue;
        idxs.push_back(i);
    }
    if (!idxs.size())
    {
        SYNC_PRINT(("BufferFactory::load(%s): no loaders for it!\n", name.c_str()));
        return NULL;
    }

    for (auto idx : idxs)
    {
        //SYNC_PRINT(("BufferFactory::load(%s): loader <%s>\n", name.c_str(), loaders[idx]->name().c_str()));
        CORE_ASSERT_TRUE_S((loaders[idx]->acceptsFile(name)));

        try {
            BufferType *result = loaders[idx]->load(name);
            if (result)
                return result;
        }
        catch (std::exception &ex)
        {
            SYNC_PRINT(("BufferFactory::load(): loader <%s> has thrown exception: <%s>\n", loaders[idx]->name().c_str(), ex.what()));
            if (idxs.size() == 1)
                throw ex;                           // there's no other readers, nothing to do further, throw ex outside
        }
        SYNC_PRINT(("BufferFactory::load(%s): loader <%s> agreed to load, but failed\n", name.c_str(), loaders[idx]->name().c_str()));
    }

    return NULL;
}

template<typename BufferType>
bool saveBuffer(BufferType &buffer, const std::string &name, const std::string &preferedProvider, vector<BufferSaver<BufferType> *> &savers)
{
    SYNC_PRINT(("BufferFactory::save(%s, preffered:%s)\n", name.c_str(), preferedProvider.c_str()));

    vector<size_t> idxs;
    for (size_t i = 0; i < savers.size(); ++i)
    {
        if (!(savers[i]->acceptsFile(name)))
            continue;

        if (savers[i]->name() == preferedProvider)
        {
            idxs.resize(1); idxs[0] = i; break;
        }
        idxs.push_back(i);
    }
    if (!idxs.size())
    {
        SYNC_PRINT(("BufferFactory::save(%s): no savers for it!\n", name.c_str()));
        return false;
    }

    for (auto i : idxs)
    {
        CORE_ASSERT_TRUE_S((savers[i]->acceptsFile(name)));

        try {
            bool result = savers[i]->save(buffer, name);
            if (result)
                return result;
        }
        catch (std::exception &ex)
        {
            SYNC_PRINT(("BufferFactory::save(): saver <%s> has thrown exception: <%s>\n", savers[i]->name().c_str(), ex.what()));
            if (idxs.size() == 1)
                throw ex;                           // there's no other savers, nothing to do further, throw ex outside
        }
        SYNC_PRINT(("BufferFactory::save(%s): saver <%s> agreed to load, but failed\n", name.c_str(), savers[i]->name().c_str()));
    }

    SYNC_PRINT(("failed on save(%s): no working saver!\n", name.c_str()));
    return false;
}

G12Buffer *BufferFactory::loadG12Bitmap(string name, const string &loaderHint)
{
    return loadBuffer(name, mLoadersG12, loaderHint);
}

G12Buffer *BufferFactory::loadG16Bitmap(string name, const string &loaderHint)
{
    return loadBuffer(name, mLoadersG16, loaderHint);
}

bool BufferFactory::saveRGB24Bitmap(RGB24Buffer &buffer, const std::string &name, const std::string &saverHint)
{
    return saveBuffer(buffer, name, saverHint, mSaversRGB24);
}

RGB24Buffer *BufferFactory::loadRGB24Bitmap(string name, const string &loaderHint)
{
    return loadBuffer(name, mLoadersRGB24, loaderHint);
}

RuntimeTypeBuffer *BufferFactory::loadRuntimeTypeBitmap(std::string name, const string &loaderHint)
{
    return loadBuffer(name, mLoadersRuntime, loaderHint);
}

std::vector<std::string> BufferFactory::extentionsRGB24()
{
    return std::vector<std::string>();
}

std::vector<std::string> BufferFactory::extentionsG12()
{
    return std::vector<std::string>();
}

std::vector<std::string> BufferFactory::extentionsRuntimeType()
{
    return std::vector<std::string>();
}

BufferLoader<RGB24Buffer> *BufferFactory::getLoaderRGB24ByName(const std::string &name)
{
     for (auto it : mLoadersRGB24)
     {
         if (it->name() == name) {
             return it;
         }
     }
}

BufferFactory::~BufferFactory()
{
    // delete all registered loaders

    for (auto it : mLoadersG12) { delete_safe(it); }
    mLoadersG12.clear();

    for (auto it : mLoadersG16) { delete_safe(it); }
    mLoadersG16.clear();

    for (auto it : mLoadersRGB24) { delete_safe(it); }
    mLoadersRGB24.clear();

    for (auto it : mLoadersRuntime) { delete_safe(it); }
    mLoadersRuntime.clear();

    for (auto it : mSaversRGB24) { delete_safe(it); }
    mSaversRGB24.clear();

    //printf("BufferFactory has been destroyed.\n");
}

} //namespace corecvs
