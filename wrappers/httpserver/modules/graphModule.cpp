#include <iostream>
#include <sstream>

#include "core/utils/utils.h"
#include "core/stats/graphData.h"
#include "graphModule.h"

using namespace corecvs;

bool GraphModule::shouldProcessURL(const std::string& url)
{
    return HelperUtils::startsWith(url, "/graph.json");
}

bool GraphModule::shouldWrapURL(const std::string& url)
{
    return false;
}

std::shared_ptr<HttpContent> GraphModule::getContentByUrl(const std::string& url)
{
    if (HelperUtils::startsWith(url, "/graph.json"))
    {
        return std::shared_ptr<HttpContent>(new GraphContent(mGraphData));
    }
    return std::shared_ptr<HttpContent>(nullptr);
}

GraphModule::GraphModule()
{
}


std::vector<uint8_t> GraphContent::getContent()
{
    std::ostringstream result;
    if (mDao == nullptr) {
        std::string str = result.str();
        return std::vector<uint8_t>(str.begin(), str.end());
    }

    mDao->lockGraphData();

    GraphData *graph = mDao->getGraphData();

    result << "[\n";

    for (unsigned i = 0; i < graph->mData.size(); i++ )
    {
        GraphHistory *hist = &graph->mData[i];
        result << "{\n";
        result << "\"label\":\"" << hist->name << "\",\n";
        result << "\"data\": [";
        for (unsigned j = 0; j < hist->size(); j++)
        {
            GraphValue &value = hist->operator [](j);
            if (value.isValid) {
                result << value.value;
            } else {
                result << "null";
            }
            if (j != (hist->size() - 1)) {
                result << ",";
            }
        }
        result << "]\n";
        result << "}";
        if (i != graph->mData.size() - 1)
        {
            result << ",";
        }
        result << "\n";
    }
    result << "]";

    mDao->unlockGraphData();

    std::string str = result.str();
    return std::vector<uint8_t>(str.begin(), str.end());
}
