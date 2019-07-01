#include "core/stereointerface/dummyFlowProcessor.h"

/**
 *   Factory
 **/

namespace corecvs {

DummyFlowImplFactory::DummyFlowImplFactory()
{

}

Processor6D *DummyFlowImplFactory::getProcessor()
{
    return new DummyFlowProcessor;
}

std::map<std::string, DynamicObject> DummyFlowProcessor::getParameters()
{
    std::map<std::string, DynamicObject> result;
    RgbColorParameters color;
    result.emplace("dummy", DynamicObject(&color));
    return result;
}

bool DummyFlowProcessor::setParameters(std::string /*name*/, const DynamicObject &/*param*/)
{
    return true;
}

} // namespace corecvs
