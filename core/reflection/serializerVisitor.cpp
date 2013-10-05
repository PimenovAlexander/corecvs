#include "serializerVisitor.h"
#include <algorithm>

namespace corecvs {

string SerializerVisitor::NoSpaces(const char* str)
{
    string toReturn(str);
//    toReturn.erase(remove_if(toReturn.begin(), toReturn.end(), isspace), toReturn.end());
    toReturn.erase(remove(toReturn.begin(), toReturn.end(), ' '), toReturn.end());
    return toReturn;
}

//template <>
//void SerializerVisitor::visit<int,    IntField>(int &field, const IntField *fieldDescriptor)
//{
//    XMLElement* param = node->GetDocument()->NewElement( fieldDescriptor->getSimpleName() );
//    param->SetAttribute("value", field);
//    node->InsertEndChild(param);
//}

//template <>
//void SerializerVisitor::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor)
//{
//    XMLElement* param = node->GetDocument()->NewElement( fieldDescriptor->getSimpleName() );
//    param->SetAttribute("value", field);
//    node->InsertEndChild(param);
//}

//template <>
//void SerializerVisitor::visit<float,  FloatField>(float &field, const FloatField *fieldDescriptor)
//{
//    XMLElement* param = node->GetDocument()->NewElement( fieldDescriptor->getSimpleName() );
//    param->SetAttribute("value", field);
//    node->InsertEndChild(param);
//}

//template <>
//void SerializerVisitor::visit<bool,   BoolField>(bool &field, const BoolField *fieldDescriptor)
//{
//    XMLElement* param = node->GetDocument()->NewElement( fieldDescriptor->getSimpleName() );
//    param->SetAttribute("value", field);
//    node->InsertEndChild(param);
//}

//template <>
//void SerializerVisitor::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor)
//{
//    XMLElement* param = node->GetDocument()->NewElement( fieldDescriptor->getSimpleName() );
//    param->SetAttribute("value", field);
//    node->InsertEndChild(param);
//}

//template <>
//void SerializerVisitor::visit<int, EnumField>(int &field, const EnumField *fieldDescriptor)
//{
//    XMLElement* param = node->GetDocument()->NewElement( fieldDescriptor->getSimpleName() );
//    param->SetAttribute("value", field);
//    node->InsertEndChild(param);
//}

} //namespace corecvs
