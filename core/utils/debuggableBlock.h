#ifndef DEBUGGABLE_BLOCK_H
#define DEBUGGABLE_BLOCK_H

#include <vector>
#include <string>

#include "core/buffers/rgb24/rgb24Buffer.h"

namespace corecvs {


/**
 * Generic interface to get information about algorithm internals
 **/
class DebuggableBlock
{
public:
    DebuggableBlock();


    virtual std::vector<std::string> debugBuffers() const = 0;

    /* This method returns a copy of the internal debug buffer. You would need to delete it */
    virtual RGB24Buffer *getDebugBuffer(const std::string& name) const = 0;

    virtual void dumpAllDebugs(const std::string &prefix = "", const std::string &extenstion = ".bmp");

    virtual ~DebuggableBlock();
};



class DebuggableBlockUnited : public DebuggableBlock
{
public:

    virtual std::vector<std::string> debugBuffers() const override
    {
        return std::vector<std::string>();
    }

    virtual RGB24Buffer *getDebugBuffer(const std::string& /*name*/) const override
    {
        return NULL;
    }

    virtual ~DebuggableBlockUnited();
};

} // namespace corecvs

#endif // DEBUGGABLE_BLOCK_H
