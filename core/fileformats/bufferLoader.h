#pragma once
/**
 * \file bufferLoader.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Jun 22, 2010
 * \author alexander
 */
#include <vector>
#include <string>

namespace corecvs {

using std::string;

/**
 * Common interface for loaders for the particular format
 **/
template <typename BufferType>
class BufferLoader
{
public:
    BufferLoader() {}

    virtual bool acceptsFile(string name) = 0;
    virtual BufferType *load(string name) = 0;

    virtual std::string              name()       { return "noname"; }
    virtual std::vector<std::string> extentions() { return std::vector<std::string>(); }
    virtual ~BufferLoader() {}
};

template <typename BufferType>
class BufferSaver
{
public:
    BufferSaver() {}

    virtual bool acceptsFile(string name) = 0;
    virtual bool save(BufferType &buffer, string name) = 0;

    virtual std::string              name()       { return "noname"; }
    virtual std::vector<std::string> extentions() { return std::vector<std::string>(); }
    virtual ~BufferSaver() {}
};


} //namespace corecvs
