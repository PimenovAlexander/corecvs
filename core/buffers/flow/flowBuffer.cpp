/**
 * \file flowBuffer.cpp
 * \brief Add Comment Here
 *
 *
 * \ingroup cppcorefiles
 * \date Feb 27, 2010
 * \author alexander
 */
#include <fstream>

#include "global.h"

#include "flowBuffer.h"
#include "sseWrapper.h"
#include "tbbWrapper.h"
#include "floatFlowBuffer.h"

namespace corecvs {

using std::ifstream;
using std::ofstream;
using std::ios_base;

std::vector<FloatFlowVector> *FlowBuffer::toVectorForm()
{
    std::vector<FloatFlowVector> *result = new std::vector<FloatFlowVector>();
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (!isElementKnown(i, j))
                continue;

            FlowElement *flowElement = &element(i,j);
            Vector2dd start(j,i);
            Vector2dd shift(flowElement->x(), flowElement->y());
            result->push_back(FloatFlowVector(start, start + shift));
        }
    }
    return result;
}

FlowBuffer *FlowBuffer::invert()
{
    FlowBuffer *result = new FlowBuffer(this->h, this->w);
    for (int i = 0; i < result->h; i++)
    {
        for (int j = 0; j < result->w; j++)
        {
            if (!this->isElementKnown(i,j))
                continue;
            FlowElement flowElem = this->element(i,j);
            int x = flowElem.x() + j;
            int y = flowElem.y() + i;
            if (!result->isValidCoord(y,x))
                continue;
            result->element(y, x) = FlowElement(-flowElem.x(), -flowElem.y());
        }
    }
    return result;
}

Vector2dd FlowBuffer::getMeanDifference()
{
    Vector2dd mean (0.0);
    int       num = 0;

    for (int i = 0; i < this->h; i++)
        for (int j = 0; j < this->w; j++)
        {
            if (!this->isElementKnown(i,j))
                continue;
            mean += Vector2dd(this->element(i,j));
            num++;
        }
    return (num != 0) ? mean / double(num) : mean;
}

//---------------------------------------------------------------------------
FlowBuffer* FlowBuffer::load(const string& path)
{
    bool_t binaryMode = path.rfind(".txt") == string::npos;

    ifstream file(path.c_str(), binaryMode ? ios_base::binary : ios_base::in);
    if (!file)
        return NULL;

    FlowBuffer* toReturn = new FlowBuffer;
    if (toReturn == NULL)
        return NULL;

    if (!toReturn->AbstractBuffer::load(file, binaryMode)) {
        delete_safe(toReturn);
        return NULL;
    }

    if (!binaryMode)                                        // binary file has been already loaded
    {
        for (int i = 0; i < toReturn->getH(); i++) {
            for (int j = 0; j < toReturn->getW(); j++)
                toReturn->setElementUnknown(i, j);          // at first initialize FB by the init value
        }
        char buf[256];
        read(file, buf, binaryMode);                        // skip footer

        int offsetXY = 0;
        while (!file.eof())
        {
            int x0, y0, x1, y1;
            file >> y0 >> x0 >> y1 >> x1;

            if (!toReturn->isValidCoord(y0, x0) ||
                !toReturn->isValidCoord(y1, x1))
            {
                delete_safe(toReturn);
                break;
            }

            if (y0 < 5 && !offsetXY) {                      // old file format: all coords have an offset by 5 pixels
                offsetXY = 5;
            }
            toReturn->element(y0 + offsetXY, x0 + offsetXY) = FlowElement(x1 - x0, y1 - y0);
        }
    }

    return toReturn;
}

bool FlowBuffer::dump(const string& path) const
{
    bool_t binaryMode = path.rfind(".txt") == string::npos;

    ofstream file(path.c_str(), ios_base::trunc | (binaryMode ? ios_base::binary : ios_base::out));
    if (!file)
        return false;

    bool_t res = AbstractBuffer::dump(file, binaryMode);

    if (res && !binaryMode)
    {
        file << "#y0\tx0\ty1\tx1\n";                        // write footer
        for (int i = 0; i < h; i++)
        {
            for (int j = 0; j < w; j++)
            {  
                if (isElementKnown(i, j))
                {
                    FlowElement el = element(i, j);
                    int x0 = j; // - 5;
                    int y0 = i; // - 5;
                    int x1 = x0 + el.x();
                    int y1 = y0 + el.y();
                    file << y0 << TAB << x0 << TAB << y1 << TAB << x1 << endl;
                }
            }
        }
    }

    return !file.bad();
}

} //namespace corecvs
