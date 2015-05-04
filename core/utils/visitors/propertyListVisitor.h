#ifndef PROPERTYLISTVISITOR_H_
#define PROPERTYLISTVISITOR_H_
/**
 * \file propertyListVisitor.h
 * \brief Add Comment Here
 *
 * \date Nov 27, 2011
 * \author alexander
 */


#include <deque>
#include <string>
#include <vector>

#include "global.h"

#include "propertyList.h"
#include "basePathVisitor.h"
#include "reflection.h"

namespace corecvs {

/**
 *  Visitor that serializes the type hierarchy into the PropertyList -
 *  the internal human readable format for CVS core
 **/
class PropertyListWriterVisitor : public BasePathVisitor
{
public:
    PropertyListWriterVisitor(PropertyList *_output) :
        output(_output)
    {}

template<class Type>
    void visit(Type &field, Type defaultValue, const char *fieldName);

template <typename inputType, typename reflectionType>
    void visit(inputType &field, const reflectionType *fieldDescriptor);

    PropertyList *output;
};

template<class Type>
void PropertyListWriterVisitor::visit(Type &field, Type /*defaultValue*/, const char *fieldName)
{
    pushChild(fieldName);
        field.accept(*this);
    popChild();
}

template <typename inputType, typename reflectionType>
    void PropertyListWriterVisitor::visit(inputType &field, const reflectionType *fieldDescriptor)
{
    pushChild(fieldDescriptor->name.name);
        field.accept(*this);
    popChild();
}


template<>
    void PropertyListWriterVisitor::visit<int>(int &intField, int defaultValue, const char *fieldName);

template<>
    void PropertyListWriterVisitor::visit<double>(double &doubleField, double defaultValue, const char *fieldName);

template<>
    void PropertyListWriterVisitor::visit<bool>(bool &boolField, bool boolValue, const char *fieldName);

template<>
    void PropertyListWriterVisitor::visit<std::string>(std::string &stringField, std::string stringValue, const char *fieldName);


/* New Style visitor*/
template <>
    void PropertyListWriterVisitor::visit<int,    IntField>(int &field, const IntField *fieldDescriptor);

template <>
    void PropertyListWriterVisitor::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor);

template <>
    void PropertyListWriterVisitor::visit<bool,   BoolField>(bool &field, const BoolField *fieldDescriptor);



/**
 *  Visitor that de-serializes the type hierarchy from the PropertyList -
 *  the internal human readable format for CVS core
 **/
class PropertyListReaderVisitor : public BasePathVisitor
{
public:
    PropertyList *input;
    bool ignoreUnknown;

    PropertyListReaderVisitor(PropertyList *_input, bool _ignoreUnknown = false) :
        input(_input),
        ignoreUnknown(_ignoreUnknown)
    {}

template<class Type>
    void visit(Type &field, Type defaultValue, const char *fieldName);

template <typename inputType, typename reflectionType>
    void visit(inputType &field, const reflectionType *fieldDescriptor);


};

template<class Type>
void PropertyListReaderVisitor::visit(Type &field, Type /*defaultValue*/, const char *fieldName)
{
    pushChild(fieldName);
        field.accept(*this);
    popChild();
}

template <typename inputType, typename reflectionType>
    void PropertyListReaderVisitor::visit(inputType &field, const reflectionType *fieldDescriptor)
{
    pushChild(fieldDescriptor->name.name);
        field.accept(*this);
    popChild();
}

template<>
    void PropertyListReaderVisitor::visit<int>(int &intField, int defaultValue, const char *fieldName);

template<>
    void PropertyListReaderVisitor::visit<double>(double &doubleField, double defaultValue, const char *fieldName);

template<>
    void PropertyListReaderVisitor::visit<bool>(bool &boolField, bool boolValue, const char *fieldName);

template<>
    void PropertyListReaderVisitor::visit<std::string>(std::string &stringField, std::string stringValue, const char *fieldName);


/* New Style visitor*/
template <>
    void PropertyListReaderVisitor::visit<int,    IntField>(int &field, const IntField *fieldDescriptor);

template <>
    void PropertyListReaderVisitor::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor);

template <>
    void PropertyListReaderVisitor::visit<bool,   BoolField>(bool &field, const BoolField *fieldDescriptor);




} //namespace corecvs
#endif /* PROPERTYLISTVISITOR_H_ */

