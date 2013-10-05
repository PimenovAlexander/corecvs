#pragma once
/**
 * \file bufferLoader.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Jun 22, 2010
 * \author alexander
 */
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

//  virtual BufferType *load(string name, fstream input) = 0;

    virtual ~BufferLoader() {}

};

} //namespace corecvs
