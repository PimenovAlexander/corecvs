#ifndef DESERIALIZERVISITOR_H
#define DESERIALIZERVISITOR_H

#include "reflection.h"
#include "tinyxml2.h"

namespace corecvs {

using namespace tinyxml2;
using std::string;

class DeserializerVisitor
{
public:
    XMLElement* element;

    explicit  DeserializerVisitor(XMLElement* _element) :
        element(_element)
    {}

    string NoSpaces(const char*);
//    string NoSpaces(string);

template <typename inputType, typename reflectionType>
    void visit(inputType &field, const reflectionType* fieldDescriptor)
    {
        XMLElement* param = element->FirstChildElement( NoSpaces(fieldDescriptor->getSimpleName()).c_str() );
        field = param->Attribute("value");
    }

template <class Type>
    void visit(Type &field, Type /*defaultValue*/, const char *fieldName)
    {
        field.accept(*this);
    }
}; // DeserializerVisitor

template <>
void DeserializerVisitor::visit<int,    IntField>(int &field, const IntField *fieldDescriptor);

template <>
void DeserializerVisitor::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor);

template <>
void DeserializerVisitor::visit<float,  FloatField>(float &field, const FloatField *fieldDescriptor);

template <>
void DeserializerVisitor::visit<bool,   BoolField>(bool &field, const BoolField *fieldDescriptor);

template <>
void DeserializerVisitor::visit<string, StringField>(string &field, const StringField *fieldDescriptor);

template <>
void DeserializerVisitor::visit<char*,  PointerField>(char* &field, const PointerField *fieldDescriptor);

template <>
void DeserializerVisitor::visit<void*,  PointerField>(void* &field, const PointerField *fieldDescriptor);

template <>
void DeserializerVisitor::visit<int,    EnumField>(int &field, const EnumField *fieldDescriptor);

} //namespace corecvs

#endif // DESERIALIZERVISITOR_H
