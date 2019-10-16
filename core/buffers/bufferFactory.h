#pragma once
/**
 * \file bufferFactory.h
 * \brief This file contains the expandable interface for loading bitmaps from files and/or streams
 *
 * \ingroup cppcorefiles
 * \date Jun 22, 2010
 */
#include <string>
#include <vector>

#include "core/utils/global.h"

#include "core/buffers/g12Buffer.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/fileformats/bufferLoader.h"
#include "core/buffers/runtimeTypeBuffer.h"
#include "core/buffers/float/dpImage.h"

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

    static void printCaps();

    /**
     * Public function to add own buffer loader, i.e. QtFileLoader
     **/
    bool registerLoader(BufferLoader<G12Buffer> * loader)
    {
        mLoadersG12.push_back(loader);
        return true;
    }

    bool registerLoaderG16(BufferLoader<G12Buffer> * loader)
    {
        mLoadersG16.push_back(loader);
        return true;
    }

    bool registerLoader(BufferLoader<RGB24Buffer> * loader)
    {
        mLoadersRGB24.push_back(loader);
        return true;
    }

    bool registerLoader(BufferLoader<RuntimeTypeBuffer> * loader)
    {
        mLoadersRuntime.push_back(loader);
        return true;
    }

    bool registerLoader(BufferLoader<DpImage> * loader)
    {
        mLoadersDoubleImage.push_back(loader);
        return true;
    }

    bool registerLoader(BufferLoader<FpImage> * loader)
    {
        mLoadersFloatImage.push_back(loader);
        return true;
    }

    bool registerSaver(BufferSaver<RGB24Buffer> * saver)
    {
        mSaversRGB24.push_back(saver);
        return true;
    }

    bool registerLoader(BufferLoader<FloatFlowBuffer> * loader)
    {
        mLoadersFloatFlow.push_back(loader);
        return true;
    }


    bool registerSaver(BufferSaver<FloatFlowBuffer> * saver)
    {
        mSaversFloatFlow.push_back(saver);
        return true;
    }

    bool registerSaver(BufferSaver<DpImage> * saver)
    {
        mSaversDoubleImage.push_back(saver);
        return true;
    }

    bool registerSaver(BufferSaver<FpImage> * saver)
    {
        mSaversFloatImage.push_back(saver);
        return true;
    }


    /** Main function to get a 12-bits buffer from the file with the given path name
     */
    virtual G12Buffer* loadG12Bitmap(string name, const string &loaderHint = "");

    /** Main function to get a 32-bits buffer from the file with the given path name
     */
    virtual RGB24Buffer* loadRGB24Bitmap(string name, const string &loaderHint = "");

    /** Main function to get a fixed 8|32 bits buffer from the file with the given path name
     */
    virtual RuntimeTypeBuffer* loadRuntimeTypeBitmap(string name, const string &loaderHint = "");

    /** Main function to get a 16-bits buffer from the file with the given path name
     */
    virtual G12Buffer* loadG16Bitmap(string name, const string &loaderHint = "");

    /** Main function to get a float flow
     */
    virtual FloatFlowBuffer* loadFloatFlow(string name, const string &loaderHint = "");

    /** Main function to get double image
     */
    virtual DpImage* loadDpImage(string name, const string &loaderHint = "");

    /** Main function to get double image
     */
    virtual FpImage *loadFpImage(string name, const string &loaderHint = "");

    /** Main function to save a 32-bits buffer to the file with the given path name
     */
    virtual bool saveRGB24Bitmap(RGB24Buffer& buffer, const string &name, const string &saverHint = "");

    /** Main function to save  float flow
     */
    virtual bool saveFloatFlow(FloatFlowBuffer& buffer, const string &name, const string &saverHint = "");

    /** Main function to save double image
     */
    virtual bool saveDpImage(DpImage& buffer, const string &name, const string &saverHint = "");

    /** Main function to save double image
     */
    virtual bool saveFpImage(FpImage& buffer, const string &name, const string &saverHint = "");


    /** Two useful methods to read/save a 32-bits buffer from/to the file with the given path name
    */
    RGB24Buffer readRgb(const string &name)
    {
        RGB24Buffer img, *buf = loadRGB24Bitmap(name);  //TODO: load to the ready buffer should be implemented!
        if (buf)
        {
            img = *buf;
            delete buf;
        }
        return img;
    }

    bool saveRGB24Bitmap(RGB24Buffer* buffer, const string &name)
    {
        return saveRGB24Bitmap(*buffer, name);
    }

    bool saveRGB24Bitmap(DpImage* buffer, const string &name, const RGB24Buffer::DoubleDrawStyle &style)
    {
        RGB24Buffer image24(buffer->getSize(), false);
        image24.drawDoubleBuffer(*buffer, style);
        return saveRGB24Bitmap(image24, name);
    }

    bool saveRGB24Bitmap(FpImage* buffer, const string &name, const RGB24Buffer::DoubleDrawStyle &style)
    {
        RGB24Buffer image24(buffer->getSize(), false);
        image24.drawFloatBuffer(*buffer, style);
        return saveRGB24Bitmap(image24, name);
    }

    bool saveRGB24Bitmap(G8Buffer* buffer8, const string &name)
    {
        RGB24Buffer image24(buffer8);
        return saveRGB24Bitmap(image24, name);  //TODO: change factory ifc to support 8-bits saving
    }

    bool saveRGB24Bitmap(G12Buffer* buffer12, const string &name)
    {
        RGB24Buffer image24(buffer12);
        return saveRGB24Bitmap(image24, name);  //TODO: change factory ifc to support 8-bits saving
    }

    bool saveRGB24Bitmap(G16Buffer* buffer16, const string &name)
    {
        RGB24Buffer image24(buffer16);
        return saveRGB24Bitmap(image24, name);  //TODO: change factory ifc to support 8-bits saving
    }

    /**
     **/
    virtual std::vector<std::string> extentionsRGB24();
    virtual std::vector<std::string> extentionsG12();
    virtual std::vector<std::string> extentionsRuntimeType();
    virtual std::vector<std::string> extentionsFloatFlowType();


    BufferLoader<RGB24Buffer> *getLoaderRGB24ByName(const std::string &name);


private:
    BufferFactory();
    virtual ~BufferFactory();

public:
    /**
     * List of loaders - the supported formats
     */
    vector<BufferLoader<G12Buffer>   *>       mLoadersG12;
    vector<BufferLoader<G12Buffer>   *>       mLoadersG16;
    vector<BufferLoader<RGB24Buffer> *>       mLoadersRGB24;
    vector<BufferLoader<RuntimeTypeBuffer> *> mLoadersRuntime;
    vector<BufferLoader<FloatFlowBuffer> *>   mLoadersFloatFlow;
    vector<BufferLoader<DpImage> *>           mLoadersDoubleImage;
    vector<BufferLoader<FpImage> *>           mLoadersFloatImage;

    //vector<BufferSaver<RuntimeTypeBuffer> *> mSaversRuntime;
    vector<BufferSaver<RGB24Buffer> *>     mSaversRGB24;
    vector<BufferSaver<FloatFlowBuffer> *> mSaversFloatFlow;
    vector<BufferSaver<DpImage> *>         mSaversDoubleImage;
    vector<BufferSaver<FpImage> *>         mSaversFloatImage;

};

} //namespace corecvs
