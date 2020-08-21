#include "jsonContent.h"


JSONContent::JSONContent()
{
}

JSONContent::JSONContent(QVariantMap vars) :
    mVariants(vars)
{
}

JSONContent::~JSONContent()
{
}

QByteArray JSONContent::getContent()
{
    QByteArray result;

    /*result.append("{\n");
    QHashIterator<QString, QVariant> iter(mVariants);
    bool isFirst = true;
    while (iter.hasNext()) {
        iter.next();
        QString  key     = QString("\"") + iter.key() + QString("\"");
        QVariant variant = iter.value();

        if (variant.type() == QVariant::String )
        {
            result.append(QString("   ") + (isFirst ? " " : ",") + key + " : \"");
            result.append(variant.toString().toUtf8());
            result.append("\"\n");
            isFirst = false;
            continue;
        }

        if (variant.type() == QVariant::Bool )
        {
            result.append(QString("   ") + (isFirst ? " " : ",") + key + " : " + (variant.toBool() ? "true" : "false") + "\n" );
            isFirst = false;
            continue;
        }

        if (variant.type() == QVariant::Int )
        {
            result.append(QString("   ") + (isFirst ? " " : ",") + key + " : " + QString::number(variant.toInt()) + "\n" );
            isFirst = false;
            continue;
        }

        if (variant.type() == QVariant::LongLong )
        {
            result.append(QString("   ") + (isFirst ? " " : ",") + key + " : " + QString::number(variant.toLongLong()) + "\n" );
            isFirst = false;
            continue;
        }

        if (variant.type() == QVariant::Double )
        {
            result.append(QString("   ") + (isFirst ? " " : ",") + key + " : " + QString::number(variant.toDouble()) + "\n" );
            isFirst = false;
            continue;
        }

        if (variant.type() == QVariant::QVariantList )
        {

        }

    }
    result.append("}\n\n");*/

    // qDebug() << mVariants;
    QJsonObject obj = QJsonObject::fromVariantMap(mVariants);
    //qDebug() << obj.length();
    //qDebug() << QJsonDocument(QVariant(mVariants).toJsonObject()).toJson();

    result = QJsonDocument(obj).toJson(QJsonDocument::Indented);
    return result;
}

void JSONContent::setResult(bool res)
{
    mVariants["result"] = res;
}

void JSONContent::setValue(const QVariant &value)
{
    mVariants["value"] = value;
}
