#ifndef BINARY_READER_H
#define BINARY_READER_H


#include <stdint.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include "core/reflection/reflection.h"
#include "core/utils/log.h"

#include "core/reflection/binaryWriter.h"

namespace corecvs {

using std::string;
using std::istream;
using std::cout;


/**
   \attention NB Please note!!

   This visitor is not storing data in "key->value" form.
   It only stores values, so it assumes fields would be requested in the same order they have been writen

   There would be no checks or warnings so far. You would just experience fast yet painful death
**/
class BinaryReader : BinaryVisitorBase
{
public:
    bool isSaver () { return false;}
    bool isLoader() { return true;}


public:
    std::istream *stream  = NULL;
    bool fileOwned = false;


    explicit  BinaryReader(std::istream *istream):
        stream(istream)
    {
        prologue();
    }

    explicit  BinaryReader(const string &filepath)
        : stream(new std::ifstream(filepath.c_str(), std::ifstream::in | std::ios::binary)),
          fileOwned(true)
    {
        if (stream == NULL || !stream->good() )
        {
            L_ERROR_P("Couldn't open for reading file <%s>", filepath.c_str());
            delete_safe(stream);
            fileOwned = false;
            return;
        }
        SYNC_PRINT(("reading from <%s>\n", filepath.c_str()));
        prologue();
    }

    ~BinaryReader()
    {
       if (fileOwned) {
           epilogue();
           delete_safe(stream);
           fileOwned = false;
       }
    }
    /*file prologue*/
    void prologue() {
        const char *greeting=MAGIC_STRING1;
        char greeting_read[10];
        if (stream == NULL) return;

        stream->read(greeting_read, strlen(greeting));
        greeting_read[strlen(greeting)] = 0;

        cout << "Read greating:" << greeting_read << std::endl;
        if (!strcmp(greeting, greeting_read) || !stream->good())
        {
            cout << "Failed to recongnise the format" << std::endl;
        }

    }

    void epilogue() {
        const char *bye=MAGIC_STRING2;
        char bye_read[10];
        if (stream == NULL) return;
        stream->read(bye_read, strlen(bye));
        cout << "Read buy:" << bye_read << std::endl;
    }


    /* */
    template <typename innerType>
    void visit(std::vector<std::vector<innerType>> &fields, const char *arrayName)
    {

    }

    /**
     * Generic Array support with new style reflection
     **/
    template <typename inputType, typename reflectionType>
    void visit(std::vector<inputType> &fields, const reflectionType * /*fieldDescriptor*/)
    {
        for (int i = 0; i < fields.size(); i++)
        {
            fields[i].accept(*this);
        }        
    }

    /**
     * Generic Array support with old style
     **/
    template <typename innerType>
    void visit(std::vector<innerType> &field, const char* arrayName)
    {

    }

    /**
     * Complex objects support with new style reflection
     **/
template <typename inputType, typename reflectionType>
    void visit(inputType &field, const reflectionType * /*fieldDescriptor*/)
    {       
        field.accept(*this);       
    }


    /**
     * Complex objects support with old style calls
     **/
template <class Type>
    void visit(Type &field, const char * /*fieldName*/)
    {
        field.accept(*this);
    }

template <typename Type>
    void visit(Type &field, Type /*defaultValue*/, const char * fieldName)
    {
        visit<Type>(field, fieldName);
    }

};


template <>
void BinaryReader::visit<int,    IntField>(int &field, const IntField *fieldDescriptor);

template <>
void BinaryReader::visit<uint64_t, UInt64Field>(uint64_t &field, const UInt64Field *fieldDescriptor);

template <>
void BinaryReader::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor);

template <>
void BinaryReader::visit<float,  FloatField>(float &field, const FloatField *fieldDescriptor);

template <>
void BinaryReader::visit<bool,   BoolField>(bool &field, const BoolField *fieldDescriptor);

template <>
void BinaryReader::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor);

template <>
void BinaryReader::visit<int,    EnumField>(int &field, const EnumField *fieldDescriptor);

template <>
void BinaryReader::visit<std::string, StringField>(std::string &field, const StringField *fieldDescriptor);

template <>
void BinaryReader::visit<std::wstring, WStringField>(std::wstring &field, const WStringField *fieldDescriptor);


/* Arrays */

template <>
void BinaryReader::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor);

/**
 * Old Style
 *
 * this methods can be made universal, but are separated to make it a bit more controllable
 **/
template <>
void BinaryReader::visit<uint64_t>(uint64_t &intField, uint64_t defaultValue, const char *fieldName);

template <>
void BinaryReader::visit<bool>(bool &boolField, bool defaultValue, const char *fieldName);

template <>
void BinaryReader::visit<int>(int &boolField, int defaultValue, const char *fieldName);

template <>
void BinaryReader::visit<double>(double &boolField, double defaultValue, const char *fieldName);


template <>
void BinaryReader::visit<std::string>(std::string &stringField, std::string defaultValue, const char *fieldName);

template <>
void BinaryReader::visit<std::wstring>(std::wstring &stringField, std::wstring defaultValue, const char *fieldName);


} //namespace corecvs

#endif // BINARY_READER_H
