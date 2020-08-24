#include "reflectionContent.h"


using namespace corecvs;

ReflectionContent::ReflectionContent(LockableObject *reflection) :
    mReflection(reflection),
    mSimple(true)
{
}

std::vector<uint8_t> ReflectionContent::getContent()
{
    std::ostringstream data;
    if (mReflection == NULL) {
        data << "Oops";
    } else {
        mReflection->lock();

        std::string prefix = "";
        std::ostringstream result;

        if (!mSimple) {
            result << "Object: ";
            char buffer[100];
            snprintf2buf(buffer, "0x%08X", mReflection->rawObject);
            result << buffer;
        }

        result << "<form class=\"QtStyle\" name=\"form1\" method=\"get\">\n";
        result << "<table border=\"1\">\n";


        for (int i = 0; i < mReflection->reflection->fieldNumber(); i++)
        {
            result << "<tr>\n";
            const BaseField *field = mReflection->reflection->fields[i];

            if (!mSimple) {
                result << "<td>" << field->id << "</td>";
            }
            result << "<td>" << field->name.name << "</td>";

            if (!mSimple) {
                result << "<td>" << field->type   << "</td>";
                result << "<td>" << field->offset << "</td>";
            }
            result << "<td>";

            switch (field->type) {
                case BaseField::TYPE_BOOL:
                {
                    bool *value = mReflection->getField<bool>(i);

                    result  << "<input type=\"checkbox\" " << (*value ? "checked=\"checked\"" : "");
                    result  << " onchange=\"postEdit(this)\" ";
                    result  << "name=\"" << prefix << field->name.name << "\" >";
                    result  << field->name.name;
                    result  << "</input>";
                    break;
                }
                case BaseField::TYPE_INT:
                {
                    int *value = mReflection->getField<int>(i);
                    result << "<input "
                                 //"id=\"" + QString::number(mId++) + "\"" //for what id?
                                 "onchange=\"postEdit(this)\" "
                                 "type=\"text\" "
                                 "size=\"5\" "
                                 "value=\"" << *value << "\" "
                                 "name=\"" << prefix << field->name.name << "\" "
                                 ">"
                              "</input>\n";
                    break;
                }
                case BaseField::TYPE_DOUBLE:
                {
                    double *value = mReflection->getField<double>(i);
                    result << "<input type=\"text\"  size=\"5\" value=\"" << *value
                           << "\" onchange=\"postEdit(this)\" "
                           << " name=\"" << prefix << field->name.name << "\" >"  "</input>\n";
                    break;
                }
                case BaseField::TYPE_STRING:
                {
                    std::string *value = mReflection->getField<std::string>(i);
                    result << "<input type=\"text\"  size=\"5\" name=\"" << prefix << field->name.name
                           << "\" onchange=\"postEdit(this)\" value=\""
                           << *value << "\" >"  "</input>\n";
                    break;
                }

            case BaseField::TYPE_ENUM:
            {
                int *value = mReflection->getField<int>(i);
                const EnumField *enumField = static_cast<const EnumField *>(field);
                const EnumReflection *enumReflection = enumField->enumReflection;

                result  << "<select name=\""
                        << prefix << field->name.name << "\" "
                        << "onchange=\"postEdit(this)\" "
                        << "autocomplete=\"off\">\n";
                for (unsigned option = 0; option < enumReflection->options.size(); option++)
                {
                    result << "    <option value=\"" << option << "\" "
                           << (*value == option ? "selected" : "") << ">" << enumReflection->options[option]->name.name << "</option>\n";
                }
                result  << "</select>\n";
                break;
            }

                default:
                break;
            }

            //result += QString::number(field->id) + " " + field->name.name + " " + field->type + "<br/>\n";
            result << "</td></tr>\n";
        }

        result << "</table>\n";
        result << "</form>\n";
        data << result.str();
        mReflection->unlock();
    }
    std::string str = data.str();
    return std::vector<uint8_t>(str.begin(), str.end());
}


#if 0
bool ReflectionContent::changeValue(const QUrl &url, QVariant &realValue)
{
    typedef QPair<QString, QString> Items;

    bool ok = false;

    for(int count = 1; count < QUrlQuery(url).queryItems().size(); count++)
    {
        qDebug() << "Count: " << count;
        Items item = QUrlQuery(url).queryItems()[count];
        QString name  = item.first;
        QString newValue = item.second;

        qDebug() << "Setting value :" << name << " to " << newValue;

        int id = mReflection->reflection->idByName(name.toLatin1().constData());
        if (id == -1) {
            continue;
        }

        const BaseField *field = mReflection->reflection->fields[id];

        if (field == NULL) {
            continue;
        }

        if (field->type == BaseField::TYPE_INT)
        {
            mReflection->lock();
            int *value = mReflection->getField<int>(id);

            int remoteValue = newValue.toInt(&ok);
            const IntField* ref = static_cast<const IntField*>(mReflection->getFieldReflection(id));
            if (ref->hasAdditionalValues) {
                if (remoteValue < ref->min) remoteValue = ref->min;
                if (remoteValue > ref->max) remoteValue = ref->max;
            }
            *value = remoteValue;

            realValue = *value;
            mReflection->unlock();
            break;
        }
        if (field->type == BaseField::TYPE_ENUM)
        {
            mReflection->lock();
            int *value = mReflection->getField<int>(id);

            int remoteValue = newValue.toInt(&ok);
            const EnumField* ref = static_cast<const EnumField*>(mReflection->getFieldReflection(id));
            if (remoteValue < 0) remoteValue = 0;
            if (remoteValue >= ref->enumReflection->optionsNumber()) remoteValue = ref->enumReflection->optionsNumber() - 1;
            *value = remoteValue;

            realValue = *value;
            mReflection->unlock();
            break;
        }
        if (field->type == BaseField::TYPE_DOUBLE)
        {
            mReflection->lock();
            double *value = mReflection->getField<double>(id);

            double remoteValue = newValue.toDouble(&ok);
            const DoubleField* ref = static_cast<const DoubleField*>(mReflection->getFieldReflection(id));
            if (ref->hasAdditionalValues) {
                if (remoteValue < ref->min) remoteValue = ref->min;
                if (remoteValue > ref->max) remoteValue = ref->max;
            }
            *value = remoteValue;

            realValue = *value;
            mReflection->unlock();
            break;
        }
        if (field->type == BaseField::TYPE_BOOL)
        {
            mReflection->lock();
            bool *value = mReflection->getField<bool>(id);
            *value = (newValue == "on");
            realValue = *value;
            mReflection->unlock();
            break;
        }
        if (field->type == BaseField::TYPE_STRING)
        {
            mReflection->lock();
            std::string *value = mReflection->getField<std::string>(id);
            qDebug() << "Input string arrived <" << newValue << ">";

            QString text = QUrl::fromPercentEncoding(newValue.toLatin1());
            *value = text.toStdString();
            realValue = QString::fromStdString(*value).toHtmlEscaped();
            ok = true;
            mReflection->unlock();
            break;
        }
    }

    return ok;
}
#endif

std::string ReflectionContent::getContentType()
{
    return "text/html";
}

ReflectionListContent::ReflectionListContent(std::vector<std::string> names) :
    mNames(names)
{

}

std::vector<uint8_t> ReflectionListContent::getContent()
{
    std::ostringstream data;
    data << "<h1>Control Structures</h1>\n";
    data << "<ol>\n";
    for (auto name : mNames)
    {
        data << "  <li><a href=\"reflection?name=";
        data <<  name;
        data << "\">";
        data <<  name;
        data << "</a></li>\n";
    }

    data << "</ol>\n";

    std::string str = data.str();
    return std::vector<uint8_t>(str.begin(), str.end());
}

std::string ReflectionListContent::getContentType()
{
    return "text/html";
}
