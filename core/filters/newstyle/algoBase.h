#ifndef ALGO_BASE_H
#define ALGO_BASE_H

#include <iostream>
#include <vector>
#include <string>
#include <type_traits>

#include "utils/global.h"

// This interface is necessary to make possible class implementing both FeatureDetector/DescriptorExtractor interfaces
class AlgoBase
{
public:
	virtual double getProperty(const std::string &name) const = 0;
	virtual void   setProperty(const std::string &name, const double &value) = 0;

	virtual bool   isParallelable() { return true; }

    virtual ~AlgoBase() {}
};


/**
 * Algorithm container (library) can provide multiple subalgorithms
 **/
class AlgoNaming
{
public:
    /**
     * Checks if the library can implement requested subalgorithm
     **/
    // virtual bool provides(const PipelineAlgoType &type) = 0;
    /**
     * This is a convenience method to produce human readable name
     **/
    virtual std::string name() { return "unknown"; }
    /**
     * This is a convenience method to provide subalgorithms names.
     * This should not return all implemented methods
     **/
    virtual std::vector<std::string> provideHints() { return std::vector<std::string>(); }
};


template<typename RealTypeName>
class AlgoCollectionNaming
{
//    static_assert(std::is_base_of<AlgoNaming, RealTypeName>::value, "RealTypeName should be child of AlgoNaming");
public:
    std::vector<RealTypeName *> providers;

    void add(RealTypeName *provider)
    {
        providers.push_back(provider);
    }

    virtual ~AlgoCollectionNaming()
    {
        for (typename std::vector<RealTypeName*>::iterator p = providers.begin(); p != providers.end(); ++p )
        {
            delete_safe(*p);
        }
        providers.clear();
    }

    std::vector<std::string> getCaps()
    {
        std::vector<std::string> result;

        for (typename std::vector<RealTypeName *>::iterator p = providers.begin(); p != providers.end(); ++p)
        {
            result.push_back((*p)->name());
        }
        return result;
    }

    std::vector<std::string> provideHints()
    {
        std::vector<std::string> result;
        for (typename std::vector<RealTypeName*>::iterator p = providers.begin(); p != providers.end(); ++p)
        {
            std::vector<std::string> hints = (*p)->provideHints();
            result.insert(result.begin(), hints.begin(), hints.end());
        }
        return result;
    }

    void print(const std::string & algoClass = "Algorithm Collection")
    {
        std::cout << algoClass << " has " << providers.size() << " providers" << std::endl;
        for (typename std::vector<RealTypeName*>::iterator p = providers.begin(); p != providers.end(); ++p)
        {
            std::cout << "  " << (*p)->name() << std::endl;
            for (std::string hint: (*p)->provideHints()) {
                std::cout << "     " << hint << std::endl;
            }
        }
    }

    RealTypeName *getAlgo(const std::string &name)
    {
        for (auto p = providers.begin(); p != providers.end(); ++p)
        {
            if ((*p)->name() == name)
            {
                return (*p);
            }
        }
        SYNC_PRINT(("AlgoCollectionNaming::getAlgo(%s): no providers", name.c_str()));
        return NULL;
    }

};

#endif // ALGO_BASE_H
