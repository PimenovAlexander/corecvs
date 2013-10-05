#include "deserializerVisitor.h"
#include <algorithm>

namespace corecvs {

string DeserializerVisitor::NoSpaces(const char* str)
{
    string toReturn(str);
//    toReturn.erase(remove_if(toReturn.begin(), toReturn.end(), isspace), toReturn.end());
    toReturn.erase(remove(toReturn.begin(), toReturn.end(), ' '), toReturn.end());
    return toReturn;
}

//string DeserializerVisitor::NoSpaces(string str)
//{
//    string toReturn(str);
//    toReturn.erase(remove(toReturn.begin(), toReturn.end(), ' '), toReturn.end());
//    return toReturn;
//}

//--------------------------------------------------------------------------------------------

template <>
void DeserializerVisitor::visit<int,    IntField>(int &field, const IntField *fieldDescriptor)
{
    XMLElement* param = element->FirstChildElement( NoSpaces(fieldDescriptor->getSimpleName()).c_str() );
    field = param->IntAttribute("value");
}

template <>
void DeserializerVisitor::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor)
{
    XMLElement* param = element->FirstChildElement( NoSpaces(fieldDescriptor->getSimpleName()).c_str() );
    field = param->DoubleAttribute("value");
}

template <>
void DeserializerVisitor::visit<float,  FloatField>(float &field, const FloatField *fieldDescriptor)
{
    XMLElement* param = element->FirstChildElement( NoSpaces(fieldDescriptor->getSimpleName()).c_str() );
    field = param->FloatAttribute("value");
}

template <>
void DeserializerVisitor::visit<bool,   BoolField>(bool &field, const BoolField *fieldDescriptor)
{
    XMLElement* param = element->FirstChildElement( NoSpaces(fieldDescriptor->getSimpleName()).c_str() );
    field = param->BoolAttribute("value");
}

template <>
void DeserializerVisitor::visit<string, StringField>(string &field, const StringField *fieldDescriptor)
{
    XMLElement* param = element->FirstChildElement( NoSpaces(fieldDescriptor->getSimpleName()).c_str() );
    field = string(param->Attribute("value"));
}

template <>
void DeserializerVisitor::visit<char*,  PointerField>(char* &field, const PointerField *fieldDescriptor)
{
    XMLElement* param = element->FirstChildElement( NoSpaces(fieldDescriptor->getSimpleName()).c_str() );
    const char* str = param->Attribute("value");
    field = new char[strlen(str)+1];
    strcpy(field, str);
}

template <>
void DeserializerVisitor::visit<int,    EnumField>(int &field, const EnumField *fieldDescriptor)
{
    XMLElement* param = element->FirstChildElement( NoSpaces(fieldDescriptor->getSimpleName()).c_str() );
    field = param->IntAttribute("value");
}

} //namespace corecvs
