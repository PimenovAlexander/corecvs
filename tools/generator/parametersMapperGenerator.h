#ifndef PARAMETERSMAPPERGENERATOR_H_
#define PARAMETERSMAPPERGENERATOR_H_

#include <QtCore/QSet>
#include <QtCore/QHash>
#include <QtCore/QMap>
#include <QtCore/QList>

#include "baseGenerator.h"

class ParametersMapperGenerator : public BaseGenerator
{
public:
    explicit ParametersMapperGenerator(QMap<QString, Reflection *> *reflections
                                     , QList<QPair<QString, QString> > *mapperFields
                                     , const QString &postfix);

    void generateMapper();

private:
    typedef QPair<QString, QString> StringPair;
    QString mapperPostfix;

    void enterFieldContext(int i);
    void enterMapperFieldContext(int i);

    bool getMapperClass(QString const &className, QList<CompositeField const *> &fields);

    void generateSource();
    void generateHeader();

    //QHash<QString, Reflection *> *mReflections;
    QMap<QString, Reflection *> *mReflections;

    QList<StringPair> *mMapperFields;

    /// Classes defined inside parametersMapper tag and list of their composite fields
    QHash<QString, QList<CompositeField const *> > mCompositeClasses;
    /// Classes that are fields of classes from mCompositeClasses
    QList<CompositeField const *> mFieldClasses;

    QString baseName;
    QString parametersName;
    QString widgetName;
    QString fieldName;

    QString classFileName;
    QString classWidgetFileName;

    QString signalName;
    QString pointerName;
    QString fieldType;
};

#endif // PARAMETERSMAPPERGENERATOR_H_
