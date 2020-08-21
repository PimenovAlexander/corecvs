#include <QUrlQuery>

#include "statisticsListModule.h"
#include "statisticsContent.h"

bool StatisticsListModule::shouldProcessURL(QUrl url)
{
    QString path = url.path();
    if (path.startsWith("/stat")) {
        return true;
    }
    return false;
}

bool StatisticsListModule::shouldWrapURL(QUrl /*url*/)
{
    return true;
}

QSharedPointer<HttpContent> StatisticsListModule::getContentByUrl(QUrl url)
{
    QString urlPath = url.path();
    QList<QPair<QString, QString> > query = QUrlQuery(url).queryItems();

    if (urlPath.startsWith("/stat"))
    {
        return QSharedPointer<HttpContent>(new UnitedStatisticsContent(mStatisticsDAO->getCollector()));
    }

    return QSharedPointer<HttpContent>();

}

StatisticsListModule::StatisticsListModule()
{
}
