#ifndef GRAPHINTERFACE_H
#define GRAPHINTERFACE_H

#include <vector>
#include "core/filters/blocks/filterBlock.h"

using namespace corecvs;

class GraphInterface
{
public:
    GraphInterface();
    FilterGraph* mGraph;

//signals:
//    void paramsChanged();
//public slots:
//    void addBlock(FilterBlock *block)
//    {
//        mGraph->addBlock(block);
//    }

    void addConnection(FilterBlock *block)
    {

    }

//    void parametersChanged()
//    {
//        mBlock->setParameters(widget->createParametersVirtual());
//        emit paramsChanged();
//    }
private:
    vector<FilterBlock*> mBlocks;
//    vector<FilterParametersControlWidgetBase*> mWidgets;
};

#endif // GRAPHINTERFACE_H
