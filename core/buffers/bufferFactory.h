#pragma once
/**
 * \file bufferFactory.h
 * \brief This file contains the expendable interface for loading bitmaps from files and/or streams
 *
 * \ingroup cppcorefiles
 * \date Jun 22, 2010
 * \author alexander
 */
#include <string>
#include <vector>

#include "global.h"

#include "g12Buffer.h"
#include "bufferLoader.h"
#include "countedPtr.h"

namespace corecvs {

using std::string;

/** Singleton for buffer loading
 */
class BufferFactory
{
public:
    /** Public function to get the only one instance of this object.
     *  It will be automatically destroyed on shut down.
     */
    static BufferFactory* getInstance();

    /** Public function to add own buffer loader, i.e. QtFileLoader
     */
    bool registerLoader(BufferLoader<G12Buffer> * loader)
    {
        mLoaders.push_back(loader);
        return true;
    }

    /** Main function to get a 12-bits buffer from the file with the given path name
     */
    virtual G12Buffer* loadG12Bitmap(string name);

    /** Main function to get a 16-bits buffer from the file with the given path name
     */
    virtual G12Buffer* loadG16Bitmap(string name);


private:
    friend class CountedPtr<BufferFactory>;                 // this class will create/destroy the factory indeed
    BufferFactory() {}
    virtual ~BufferFactory();

    static CountedPtr<BufferFactory> sThis;

    /**
     * List of loaders - the supported formats
     */
    vector<BufferLoader<G12Buffer> *> mLoaders;
};

} //namespace corecvs
