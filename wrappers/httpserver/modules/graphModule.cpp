#include "core/stats/graphData.h"
#include "graphModule.h"

bool GraphModule::shouldProcessURL(QUrl url)
{
    QString path = url.path();
    if (path.startsWith("/graph.json")) {
        return true;
    }
    return false;
}

bool GraphModule::shouldWrapURL(QUrl url)
{
    return false;
}

QSharedPointer<HttpContent> GraphModule::getContentByUrl(QUrl url)
{
    QString urlPath = url.path();

    if (urlPath.startsWith("/graph.json"))
    {
        return QSharedPointer<HttpContent>(new GraphContent(mGraphData));
    }
    return QSharedPointer<HttpContent>(NULL);
}

GraphModule::GraphModule()
{
}


QByteArray GraphContent::getContent()
{
    QByteArray result;
    if (mDao == NULL) {
        return result;
    }

    mDao->lockGraphData();
    GraphData *graph = mDao->getGraphData();

    result.append("[\n");

    for (unsigned i = 0; i < graph->mData.size(); i++ )
    {
        GraphHistory *hist = &graph->mData[i];
        result.append("{\n");
        result.append(QString("\"label\":\"") + QString::fromStdString(hist->name) + "\",\n");
        result.append(QString("\"data\": ["));
        for (unsigned j = 0; j < hist->size(); j++)
        {
            GraphValue &value = hist->operator [](j);
            QString record = "null";
            if (value.isValid) {
                record = QString::number(value.value);
            }
            result.append(record);
            if (j != (hist->size() - 1)) {
                result.append(",");
            }
        }
        result.append("]\n");
        result.append("}");
        if (i != graph->mData.size() - 1)
        {
            result.append(",");
        }
        result.append("\n");
    }
    result.append("]");

    mDao->unlockGraphData();
    return result;
}
