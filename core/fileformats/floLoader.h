#ifndef FLOLOADER_H
#define FLOLOADER_H

#include "core/buffers/flow/flowBuffer.h"
#include "core/buffers/flow/floatFlowBuffer.h"

#include "bufferLoader.h"

namespace corecvs {

/**
 * Loads data in .flo format as described in Middlebury
 *
 * http://vision.middlebury.edu/flow/code/flow-code/README.txt
 **/
class FLOLoader : public BufferLoader<FloatFlowBuffer>
{
public:
    static string extention1;
    static float MAGIC_NUMBER;
    static float FLO_INFINITY;

    virtual bool acceptsFile(const string & name) override;

    virtual FloatFlowBuffer *load(const string & name) override;

    virtual std::string name() override { return "FLO Loader"; }
    virtual std::vector<std::string> extentions() override {
        return std::vector<std::string>({ extention1 });
    }
};

class FLOSaver : public BufferSaver<FloatFlowBuffer>
{
    virtual bool acceptsFile(const std::string & name);

    virtual bool save(const FloatFlowBuffer &buffer, const string& name, int quality = 95) override;

    virtual std::string name() override { return "FLO Saver"; }
    virtual std::vector<std::string> extentions() override {
        return std::vector<std::string>({FLOLoader::extention1});
    }

    virtual ~FLOSaver() {}
};


class ListFlowSaver : public BufferSaver<FloatFlowBuffer>
{
    static string extention1;

    virtual bool acceptsFile(const std::string & name);

    virtual bool save(const FloatFlowBuffer &buffer, const string& name, int quality = 95) override;

    virtual std::string name() override { return "List Flow Saver"; }
    virtual std::vector<std::string> extentions() override {
        return std::vector<std::string>({ListFlowSaver::extention1});
    }

    virtual ~ListFlowSaver() {}
};

} //namespace

#endif // FLOLOADER_H
