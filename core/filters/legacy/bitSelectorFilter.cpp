/**
 * \file bitSelectorFilter.cpp
 * \brief Add Comment Here
 *
 * \date May 1, 2012
 * \author alexander
 */

#include "core/filters/legacy/bitSelectorFilter.h"
#include "core/buffers/commonMappers.h"
#include "core/reflection/serializerVisitor.h"
#include "core/reflection/deserializerVisitor.h"

namespace corecvs
{

int BitSelectorFilter::instanceCounter = 0;

BitSelectorFilter::~BitSelectorFilter()
{
    // TODO Auto-generated destructor stub
}

int BitSelectorFilter::operator()()
{
//    printf("BitSelectorFilter::operator() called\n");
    G12Buffer* &input  = static_cast<G12Pin*>(inputPins[0])->getData();
    G12Buffer* &result = static_cast<G12Pin*>(outputPins[0])->getData();

    if (input == NULL)
        return 0;

    uint16_t mask = 0x0;
    if (mBitSelectorParameters.bit0 ()) mask |= 0x0001;
    if (mBitSelectorParameters.bit1 ()) mask |= 0x0002;
    if (mBitSelectorParameters.bit2 ()) mask |= 0x0004;
    if (mBitSelectorParameters.bit3 ()) mask |= 0x0008;

    if (mBitSelectorParameters.bit4 ()) mask |= 0x0010;
    if (mBitSelectorParameters.bit5 ()) mask |= 0x0020;
    if (mBitSelectorParameters.bit6 ()) mask |= 0x0040;
    if (mBitSelectorParameters.bit7 ()) mask |= 0x0080;

    if (mBitSelectorParameters.bit8 ()) mask |= 0x0100;
    if (mBitSelectorParameters.bit9 ()) mask |= 0x0200;
    if (mBitSelectorParameters.bit10()) mask |= 0x0400;
    if (mBitSelectorParameters.bit11()) mask |= 0x0800;

    if (mBitSelectorParameters.bit12()) mask |= 0x1000;
    if (mBitSelectorParameters.bit13()) mask |= 0x2000;
    if (mBitSelectorParameters.bit14()) mask |= 0x4000;
    if (mBitSelectorParameters.bit15()) mask |= 0x8000;

    int shift = mBitSelectorParameters.shift();
    result = new G12Buffer(input);
    ShiftMaskMapper smm(mask, shift);
    result->mapOperationElementwize<ShiftMaskMapper>(smm);
    return 0;
}

XMLNode* BitSelectorFilter::serialize(XMLNode* node)
{
    XMLNode* mBlock = FilterBlock::serialize(node);
    XMLElement* params = node->GetDocument()->NewElement("BitSelectorParameters");
    XMLNode* paramsNode = mBlock->InsertEndChild(params);

    SerializerVisitor visitor(paramsNode);
    mBitSelectorParameters.accept(visitor);
    return mBlock;
}

void BitSelectorFilter::deserialize(XMLNode* node, bool force)
{
    FilterBlock::deserialize(node, force);
    XMLElement* p = node->LastChild()->ToElement();

    DeserializerVisitor visitor(p);
    mBitSelectorParameters.accept(visitor);
}

} /* namespace corecvs */
