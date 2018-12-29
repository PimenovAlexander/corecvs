#include "core/buffers/bufferFactory.h"
#include "debuggableBlock.h"


namespace corecvs {

DebuggableBlock::DebuggableBlock()
{

}

void DebuggableBlock::dumpAllDebugs(const std::string &prefix, const std::string &extenstion )
{
    std::vector<std::string> buffers = debugBuffers();

    BufferFactory *factory = BufferFactory::getInstance();
    for (std::string &bufferName : buffers)
    {
        RGB24Buffer *debug = getDebugBuffer(bufferName);
        factory->saveRGB24Bitmap(debug, prefix + bufferName + extenstion);
        delete_safe(debug);
    }
}

DebuggableBlock::~DebuggableBlock()
{

}


}
