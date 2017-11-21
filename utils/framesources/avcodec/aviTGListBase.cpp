/**
 * \brief Capture video stream from avi file using avcodec library
 */

#include "core/utils/global.h"
#include "aviTGListBase.h"

#include <QDir>
#include <QFileInfo>
#include <QRegExp>
#include <QDebug>



AviTGListBase::AviTGListBase(const QString &params) :
    AviListBase(params),
    frameCount(0)
{
    parseFileName(params);

}

void AviTGListBase::parseFileName(const QString &fileName)
{
    QFileInfo fileInfo(fileName);

        // video-2015-01-29_12:00-1422522133.mp4
        // 1    2     3      4      5      6      7        8
    QRegExp aviStringPattern(QString("^(.*)-(\\d*)-(\\d*)-(\\d*)_(\\d*)(?::|-)(\\d*)-(\\d*)\\.([^\\.]*)$"));

    static const int GenericNameGroup  = 1;
    static const int YearGroup         = 2;
    static const int MonthGroup        = 3;
    static const int DayGroup          = 4;
    static const int HourGroup         = 5;
    static const int MinutesGroup      = 6;
    static const int TimestampGroup    = 7;
    static const int ExtentionGroup    = 8;

    int result = aviStringPattern.indexIn(fileInfo.fileName());
    if (result == -1)
    {
        printf("AVI is a generic AVI:%s\n", fileName.toLatin1().constData());
        mVirualAVIStart = PreciseTimer::currentTime().usec();
    } else
    {
        /*printf (
        "Parsed data:\n"
        "  | - Name = %s\n"
        "  | - Date = %s %s %s\n"
        "  | - Time = %s %s \n"
        "  | - Timestamp = %s   \n"
        "  \\ - Ext = %s\n",
                aviStringPattern.cap(GenericNameGroup) .toLatin1().constData(),

                aviStringPattern.cap(YearGroup) .toLatin1().constData(),
                aviStringPattern.cap(MonthGroup)  .toLatin1().constData(),
                aviStringPattern.cap(DayGroup).toLatin1().constData(),

                aviStringPattern.cap(HourGroup)   .toLatin1().constData(),
                aviStringPattern.cap(MinutesGroup)  .toLatin1().constData(),

                aviStringPattern.cap(TimestampGroup).toLatin1().constData(),
                aviStringPattern.cap(ExtentionGroup).toLatin1().constData()
        );*/

        mVirualAVIStart = aviStringPattern.cap(TimestampGroup).toLongLong() * 1000 * 1000;
        //SYNC_PRINT(("Start timestamp will be %" PRIu64 "\n", mVirualAVIStart / 1000));
        //SYNC_PRINT(("Now it is               %" PRIu64 "\n", PreciseTimer::currentTime().usec() / 1000));

        //QDateTime dateTime;
        //dateTime.setMSecsSinceEpoch(mVirualAVIStart);
        //SYNC_PRINT(("This is apporximately: %s\n", dateTime.toString().toLatin1().constData()));
        //dateTime.setMSecsSinceEpoch(PreciseTimer::currentTime().usec() / 1000);
        //SYNC_PRINT(("Now is apporximately: %s\n", dateTime.toString().toLatin1().constData()));

        path   = fileInfo.absoluteDir().absolutePath();
        prefix = aviStringPattern.cap(GenericNameGroup);

        timemark = QDateTime(
                    QDate(
                            aviStringPattern.cap(YearGroup).toInt(),
                            aviStringPattern.cap(MonthGroup).toInt(),
                            aviStringPattern.cap(DayGroup).toInt()
                        ),
                    QTime(
                            aviStringPattern.cap(HourGroup).toInt(),
                            aviStringPattern.cap(MinutesGroup).toInt()
                        )
                    );
    }

}



AviTGListBase::~AviTGListBase()
{

}

int AviTGListBase::getRawFrameFromAVI()
{
    frameCount++;
    if (frameCount < 6600) {
        return AviListBase::getRawFrameFromAVI();
    } else {
        return  -1;
    }
}

string AviTGListBase::getNextFile()
{
    frameCount = 0;
    SYNC_PRINT(("AviTGListBase::getNextFile(): called: We have finished a file.\n"));
    if (prefix.isEmpty()) {
        return "";
    }

//    qDebug() << "AviTGListBase::getNextFile()" << timemark;
    timemark = timemark.addSecs(10 * 60);
//    qDebug() << "AviTGListBase::getNextFile()" << timemark;

    QString nameFilter1(prefix + "-" + timemark.toString( "yyyy-MM-dd_HH:mm" ) + "*.mp4" );
    QString nameFilter2(prefix + "-" + timemark.toString( "yyyy-MM-dd_HH-mm" ) + "*.mp4" );

//    qDebug() << "File dir is " << path;
//    qDebug() << "Next file pattern is " << nameFilter;
    QStringList filters;
    filters << nameFilter1 << nameFilter2;

    QDir dir(path);
    QStringList fileNames = dir.entryList(filters);
    qDebug() << "files found " << fileNames;



    if (!fileNames.isEmpty())
    {
        QString fileName = path + QDir::separator() + fileNames[0];
        parseFileName(fileName);
        return fileName.toStdString();
    }
    else
        return "";
}
