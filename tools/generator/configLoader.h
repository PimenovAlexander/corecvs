#ifndef CONFIGLOADER_H_
#define CONFIGLOADER_H_

#include <QtCore/QString>
#include <QtXml/QDomDocument>
#include <QtCore/QHash>
#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QFileInfo>
#include "reflectionGenerator.h"

using namespace corecvs;

class ConfigLoader
{
public:
    explicit ConfigLoader();
    ~ConfigLoader();

    QMap<QString, Reflection *> *load(QString const &fileName);

    QList<QPair<QString, QString> > *paramsMapperFields();
    const QString mapperPostfix()
    {
        return mMapperPostfix;
    }

    QHash<QString, EnumReflection *> *enumReflections()
    {
        return &mEnums;
    }

private:
    void loadIncludes(QDomDocument const &config, const QFileInfo &currentFile);
    void loadEnums(QDomDocument const &config, const QFileInfo &currentFile);
    void loadClasses(QDomDocument const &config, QFileInfo const &currentFile);
    void loadParamsMapper(QDomDocument const &config, QFileInfo const &currentFile);

    QString getFromHierarcy(QDomElement const &classElement, QString const &name);
    ReflectionNaming getNamingFromXML(QDomElement const &classElement);

    const char *toCString(QString const &str);

    QHash<QString, EnumReflection *> mEnums;
    QMap<QString, Reflection *> mReflections;

    QString mMapperPostfix;
    QList<QPair<QString, QString> > mParamsMapper;
};

#endif // CONFIGLOADER_H_
