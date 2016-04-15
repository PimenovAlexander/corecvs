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
    bool isSaver () { return true;}
    bool isLoader() { return false;}

public:
    bool autoInit = false;
    string filename;
    PropertyList *output;

    PropertyListWriterVisitor(PropertyList *_output) :
        output(_output)
    {}

    PropertyListWriterVisitor(const std::string &filename) :
        autoInit(true),
        filename(filename),
        output(new PropertyList())
    {
        //output->load(filename);
    }

    ~PropertyListWriterVisitor()
    {
        if (autoInit) {
            output->save(filename);
            delete_safe(output);
        }
    }

template<class Type>
    void visit(Type &field, Type defaultValue, const char *fieldName);

template <typename inputType>
    void visit(inputType &field, const char *fieldName);


template <typename inputType, typename reflectionType>
    void visit(inputType &field, const reflectionType *fieldDescriptor);

/* Generic array support*/
    template <typename innerType>
    void visit(std::vector<innerType> &field, const char* arrayName)
    {
        for (size_t i = 0; i < field.size(); i++)
        {
            std::ostringstream ss;
            ss << arrayName << "[" <<  i << "]";
            visit<innerType>(field[i], ss.str().c_str());
        }
    }



};

template<class Type>
void PropertyListWriterVisitor::visit(Type &field, Type /*defaultValue*/, const char *fieldName)
{
    pushChild(fieldName);
        field.accept(*this);
    popChild();
}

template <typename inputType>
    void PropertyListWriterVisitor::visit(inputType &field, const char *fieldName)
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
    void PropertyListWriterVisitor::visit<int,    EnumField>(int &field, const EnumField *fieldDescriptor);

template <>
    void PropertyListWriterVisitor::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor);

template <>
    void PropertyListWriterVisitor::visit<bool,   BoolField>(bool &field, const BoolField *fieldDescriptor);

template <>
    void PropertyListWriterVisitor::visit<std::string,   StringField>(std::string &field, const StringField *fieldDescriptor);


/* Typed arrays */
template <>
    void PropertyListWriterVisitor::visit<std::vector<double>, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor);



/**
 *  Visitor that de-serializes the type hierarchy from the PropertyList -
 *  the internal human readable format for CVS core
 **/
class PropertyListReaderVisitor : public BasePathVisitor
{
public:
    bool isSaver () { return false;}
    bool isLoader() { return true; }

public:
    bool autoInit = false;
    PropertyList *input;
    bool ignoreUnknown;

    PropertyListReaderVisitor(PropertyList *_input, bool _ignoreUnknown = false) :
        input(_input),
        ignoreUnknown(_ignoreUnknown)
    {}

    PropertyListReaderVisitor(const std::string &filename) :
        autoInit(true),
        input(new PropertyList()),
        ignoreUnknown(false)
    {
        input->load(filename);
    }

    ~PropertyListReaderVisitor()
    {
        if (autoInit) {
            delete_safe(input);
        }
    }

template<class Type>
    void visit(Type &field, Type defaultValue, const char *fieldName);

template <typename inputType>
    void visit(inputType &field, const char *fieldName);

template <typename inputType, typename reflectionType>
    void visit(inputType &field, const reflectionType *fieldDescriptor);




/* Generic array support*/
    template <typename innerType>
    void visit(std::vector<innerType> &field, const char* arrayName)
    {
        for (size_t i = 0; i < field.size(); i++)
        {
            std::ostringstream ss;
            ss << arrayName << "[" <<  i << "]";
            visit<innerType>(field[i], ss.str().c_str());
        }
    }


};

template<class Type>
void PropertyListReaderVisitor::visit(Type &field, Type /*defaultValue*/, const char *fieldName)
{
    pushChild(fieldName);
        field.accept(*this);
    popChild();
}

template <typename inputType>
    void PropertyListReaderVisitor::visit(inputType &field, const char *fieldName)
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
    void PropertyListReaderVisitor::visit<int,    EnumField>(int &field, const EnumField *fieldDescriptor);

template <>
    void PropertyListReaderVisitor::visit<int,    IntField>(int &field, const IntField *fieldDescriptor);

template <>
    void PropertyListReaderVisitor::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor);

template <>
    void PropertyListReaderVisitor::visit<bool,   BoolField>(bool &field, const BoolField *fieldDescriptor);

template <>
    void PropertyListReaderVisitor::visit<std::string,   StringField>(std::string &field, const StringField *fieldDescriptor);


/* Typed arrays */
template <>
    void PropertyListReaderVisitor::visit<std::vector<double>, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor);




} //namespace corecvs
#endif /* PROPERTYLISTVISITOR_H_ */

