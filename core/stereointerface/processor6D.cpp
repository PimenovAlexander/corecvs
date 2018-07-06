#include "core/stereointerface/processor6D.h"

namespace corecvs {

/** -- **/
std::unique_ptr<Processor6DFactoryHolder> Processor6DFactoryHolder::sThis(new Processor6DFactoryHolder);


Processor6DFactoryHolder::~Processor6DFactoryHolder()
{
    for (Processor6DFactory *provider : mProviders)
    {
        delete_safe(provider);
    }
}

Processor6DFactoryHolder *Processor6DFactoryHolder::getInstance()
{
    CORE_ASSERT_TRUE(sThis.get() != NULL, "Out of memory!");
    return sThis.get();
}

void Processor6DFactoryHolder::printCaps()
{
    cout << "Processor6DFactoryHolder has providers:" << endl;
    Processor6DFactoryHolder *holder = getInstance();
    for (Processor6DFactory *provider : holder->mProviders)
    {
        cout << "   " << provider->getName() << endl;
    }
}

bool Processor6DFactoryHolder::hasProvider(const std::string &name)
{
    for (Processor6DFactory *provider : mProviders)
    {
        if (provider->getName() == name)
            return true;
    }
    return false;
}

Processor6D *Processor6DFactoryHolder::getProcessor(const std::string &name)
{
    Processor6DFactoryHolder *holder = getInstance();
    for (Processor6DFactory *provider : holder->mProviders)
    {
        if (provider->getName() == name) {
            return provider->getProcessor();
        }
    }
    return NULL;
}

vector<std::string> Processor6DFactoryHolder::getHints()
{
    vector<std::string> hints;

    Processor6DFactoryHolder *holder = getInstance();
    for (Processor6DFactory *provider : holder->mProviders)
    {
        hints.push_back(provider->getName());
    }
    return hints;
}

} //namespace
