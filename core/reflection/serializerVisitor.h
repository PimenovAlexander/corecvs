#ifndef MYXMLVISITOR_H
#define MYXMLVISITOR_H

#include "reflection/reflection.h"
#include "tinyxml2/tinyxml2.h"

namespace corecvs {

using namespace tinyxml2;

using std::string;

class SerializerVisitor
{
public:
    XMLNode* node;

    explicit  SerializerVisitor(XMLNode* _node) :
        node(_node)
    {}

    std::string NoSpaces(const char* str);

template <typename inputType, typename reflectionType>
    void visit(inputType &field, const reflectionType* fieldDescriptor)
    {
        XMLElement* param = node->GetDocument()->NewElement( NoSpaces(fieldDescriptor->getSimpleName()).c_str() );
        param->SetAttribute("value", field);
        node->InsertEndChild(param);
    }

///* Old style visitor */
template <class Type>
    void visit(const char *tagName, Type &field, const char *fieldName)
    {
        XMLElement* param = node->GetDocument()->NewElement( NoSpaces(tagName).c_str() );
        if (strlen(fieldName))  param->SetAttribute(NoSpaces(fieldName).c_str(), field);
        node->InsertEndChild(param);
//        field.accept(*this);
    }
template <class Type>
    void visit(const char *tagName, int fieldsNum, Type* field[], const char *fieldName[])
    {
        XMLElement* param = node->GetDocument()->NewElement( NoSpaces(tagName).c_str() );
        for (int i = 0; i < fieldsNum; i++)
            if (strlen(fieldName[i]))
                param->SetAttribute(NoSpaces(fieldName[i]).c_str(), *field[i]);
        node->InsertEndChild(param);
//        field.accept(*this);
    }


}; // SerializerVisitor

//template <>
//void myXMLvisitor::visit<int,    IntField>(int &field, const IntField *fieldDescriptor);

//template <>
//void myXMLvisitor::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor);

//template <>
//void myXMLvisitor::visit<float,  FloatField>(float &field, const FloatField *fieldDescriptor);

//template <>
//void myXMLvisitor::visit<bool,   BoolField>(bool &field, const BoolField *fieldDescriptor);

//template <>
//void myXMLvisitor::visit<string, StringField>(string &field, const StringField *fieldDescriptor);

//template <>
//void myXMLvisitor::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor);

//template <>
//void myXMLvisitor::visit<int,    EnumField>(int &field, const EnumField *fieldDescriptor);

//template <>
//void myXMLvisitor::visit<std::string, StringField>(std::string &field, const StringField *fieldDescriptor);

///* Old style visitor */
//template <>
//void myXMLvisitor::visit<int>(int &intField, int defaultValue, const char *fieldName);

//template <>
//void myXMLvisitor::visit<double>(double &doubleField, double defaultValue, const char *fieldName);

//template <>
//void myXMLvisitor::visit<float>(float &floatField, float defaultValue, const char *fieldName);

//template <>
//void myXMLvisitor::visit<bool>(bool &boolField, bool defaultValue, const char *fieldName);


} //namespace corecvs

#endif // MYXMLVISITOR_H
