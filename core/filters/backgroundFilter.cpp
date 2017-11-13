/**
 * \file backgroundFilter.cpp
 * \brief Add Comment Here
 *
 * \date Oct 11, 2012
 */

#include "core/filters/backgroundFilter.h"
#include "core/reflection/serializerVisitor.h"
#include "core/reflection/deserializerVisitor.h"

namespace corecvs
{
int BackgroundFilter::instanceCounter = 0;

int BackgroundFilter::operator()()
{
    // cout << "frame ID: " << frameId << endl;
    G12Buffer* &input  = static_cast<G12Pin*>(inputPins[0])->getData();
    G12Buffer* &result = static_cast<G12Pin*>(outputPins[0])->getData();

    if (input == NULL)  return 0;
    if (mPreviousInput == NULL)  mPreviousInput = new G12Buffer(input);

    // Here we start filtering action
    result = G12Buffer::difference(mPreviousInput, input);
    for (int i = 0; i < result->h; i++)
        for (int j = 0; j < result->w; j++)
            if (result->element(i,j) < mBackgroundFilterParameters.mThreshold)
                result->element(i,j) = 0;

    // maybe do not delete? just create proper copy
    delete_safe(mPreviousInput);
    mPreviousInput = new G12Buffer(input);
    return 0;
}

XMLNode* BackgroundFilter::serialize(XMLNode* node)
{
    XMLNode* mBlock = FilterBlock::serialize(node);
    XMLElement* params = node->GetDocument()->NewElement("BackgroundFilterParameters");
    XMLNode* paramsNode = mBlock->InsertEndChild(params);

    SerializerVisitor visitor(paramsNode);
    mBackgroundFilterParameters.accept(visitor);
    return mBlock;
}

void BackgroundFilter::deserialize(XMLNode* node, bool force)
{
    FilterBlock::deserialize(node, force);
    XMLElement* p = node->LastChild()->ToElement();

    DeserializerVisitor visitor(p);
    mBackgroundFilterParameters.accept(visitor);
}

BackgroundFilter::~BackgroundFilter()
{
    // TODO Auto-generated destructor stub
    delete_safe(mPreviousInput);
}

} /* namespace corecvs */
