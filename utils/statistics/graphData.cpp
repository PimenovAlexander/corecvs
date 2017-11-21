#include "core/utils/global.h"
#include "graphData.h"

#include <stdio.h>

#include <QDebug>

void GraphData::addGraphPoint(unsigned graphId, double value, bool isValid)
{
    if (graphId >= mData.size()) {
        qDebug("GraphPlotDialog::addGraphPoint(): adding new graph");

        mData.resize(graphId + 1);
        mData[graphId].isSelected = false;
        char buf[100];
        snprintf(buf, CORE_COUNT_OF(buf), "Graph %d", graphId);
        mData[graphId].name = std::string(buf);
    }

    mData[graphId].push_front(GraphValue(value, isValid));
    while (mData[graphId].size() > limit) {
        mData[graphId].pop_back();
    }
}


void GraphData::addGraphPoint(std::string graphName, double value, bool isValid)
{
    unsigned graphId = getGraphId(graphName);
    addGraphPoint(graphId, value, isValid);
}

unsigned GraphData::getGraphId(std::string name)
{
    unsigned graphId = 0;
    //    qDebug("GraphPlotDialog::getGraphId(%s): called", name.toLatin1().constData());

    while (graphId < mData.size()) {
        if (mData[graphId].name == name) {
            return graphId;
        }
        graphId++;
    }

    //    qDebug("GraphPlotDialog::getGraphId(): adding ui.. ");

    mData.resize(graphId + 1);
    mData[graphId].isSelected = false;
    mData[graphId].name = name;
    return graphId;
}

std::string GraphData::getGraphName(unsigned id)
{
    if (id >= mData.size())
        return "";

    return mData[id].name;
}
