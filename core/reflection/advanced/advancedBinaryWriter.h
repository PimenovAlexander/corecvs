#ifndef ADVANCED_BINARY_WRITER_H
#define ADVANCED_BINARY_WRITER_H


#include <stdint.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include "core/reflection/reflection.h"
#include "core/utils/log.h"

namespace corecvs {

using std::string;
using std::ostream;
using std::cout;

class AdvancedBinaryVisitorBase {
public:
    static const char* MAGIC_STRING1;
    static const char* MAGIC_STRING2;


    std::string readString(std::istream *stream)
    {
        uint32_t length = 0;
        stream->read((char *)&length, sizeof(length));
        char* data = new char[length + 1];
        stream->read((char *)data, length);
        data[length] = 0;                
        return data;
    }

    void writeString(std::ostream *stream, const std::string &string)
    {
        uint32_t length = (uint32_t)string.length();
        stream->write((char *)&length, sizeof(length));
        stream->write((char *)string.c_str(), length);
    }

};

/**
   \attention NB Please note!!

   This visitor is not storing data in "key->value" form.
   It only stores values, so it assumes fields would be requested in the same order they have been writen

   There would be no checks or warnings so far. You would just experience fast yet painful death
**/
class AdvancedBinaryWriter : AdvancedBinaryVisitorBase
{
public:
    bool isSaver () { return true;}
    bool isLoader() { return false;}


public:
    std::ostream *stream  = NULL;
    bool fileOwned = false;


    explicit  AdvancedBinaryWriter(std::ostream *ostream):
        stream(ostream)
    {
        prologue();
    }

    explicit  AdvancedBinaryWriter(const string &filepath)
        : stream(new std::ofstream(filepath.c_str(), std::ofstream::out | std::ios::binary)),
          fileOwned(true)
    {
        if (!(*stream))
        {
            L_ERROR_P("Couldn't open for writting the file <%s>", filepath.c_str());
            delete_safe(stream);
            fileOwned = false;
            return;
        }
        L_INFO_P("saving to <%s>", filepath.c_str());
        prologue();
    }

    ~AdvancedBinaryWriter()
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
        if (stream == NULL) return;
        stream->write(greeting, strlen(greeting));

        uint32_t flen = 0;
        /*Write a placeholder */
        stream->write((char *)&flen, sizeof(flen));

    }

    void epilogue() {
        const char *bye      = MAGIC_STRING2;
        const char *greeting = MAGIC_STRING1;

        if (stream == NULL) return;
        size_t pos1 = strlen(greeting);

        size_t pos2 = stream->tellp();
        stream->seekp(pos1);
        uint32_t flen = (uint32_t)(pos2 - pos1 - sizeof(flen));
        stream->write((char *)&flen, sizeof(flen));

        stream->seekp(pos2);
        stream->write(bye, strlen(bye));

    }



    /* Field opening */
    size_t fieldOpening(const std::string &fieldName)
    {
        writeString(stream, fieldName);

        uint32_t flen = 0;
        size_t pos1 = stream->tellp();
        /*Write a placeholder */
        stream->write((char *)&flen, sizeof(flen));
        return pos1;
    }


    /* Field closing */
    void fieldClosing(size_t pos1)
    {
        size_t pos2 = stream->tellp();
        uint32_t flen = (uint32_t)(pos2 - pos1 - sizeof(flen));
        stream->seekp(pos1);
        stream->write((char *)&flen, sizeof(flen));
        stream->seekp(pos2);
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
    void visit(inputType &field, const reflectionType * fieldDescriptor)
    {
        visit(field, fieldDescriptor->name.name);
    }


    /**
     * Complex objects support with old style calls
     **/
template <class Type>
    void visit(Type &field, const char * fieldName)
    {
        if (stream == NULL) return;
        size_t pos = fieldOpening(fieldName);
        field.accept(*this);
        fieldClosing(pos);
    }

template <typename Type>
    void visit(Type &field, Type /*defaultValue*/, const char * fieldName)
    {
        visit<Type>(field, fieldName);
    }

};


template <>
void AdvancedBinaryWriter::visit<int,    IntField>(int &field, const IntField *fieldDescriptor);

template <>
void AdvancedBinaryWriter::visit<uint64_t, UInt64Field>(uint64_t &field, const UInt64Field *fieldDescriptor);

template <>
void AdvancedBinaryWriter::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor);

template <>
void AdvancedBinaryWriter::visit<float,  FloatField>(float &field, const FloatField *fieldDescriptor);

template <>
void AdvancedBinaryWriter::visit<bool,   BoolField>(bool &field, const BoolField *fieldDescriptor);

template <>
void AdvancedBinaryWriter::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor);

template <>
void AdvancedBinaryWriter::visit<int,    EnumField>(int &field, const EnumField *fieldDescriptor);

template <>
void AdvancedBinaryWriter::visit<std::string, StringField>(std::string &field, const StringField *fieldDescriptor);

template <>
void AdvancedBinaryWriter::visit<std::wstring, WStringField>(std::wstring &field, const WStringField *fieldDescriptor);


/* Arrays */

template <>
void AdvancedBinaryWriter::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor);

/**
 * Old Style
 *
 * this methods can be made universal, but are separated to make it a bit more controllable
 **/
template <>
void AdvancedBinaryWriter::visit<uint64_t>(uint64_t &intField, uint64_t defaultValue, const char *fieldName);

template <>
void AdvancedBinaryWriter::visit<bool>(bool &boolField, bool defaultValue, const char *fieldName);

template <>
void AdvancedBinaryWriter::visit<int>(int &boolField, int defaultValue, const char *fieldName);

template <>
void AdvancedBinaryWriter::visit<double>(double &boolField, double defaultValue, const char *fieldName);


template <>
void AdvancedBinaryWriter::visit<std::string>(std::string &stringField, std::string defaultValue, const char *fieldName);

template <>
void AdvancedBinaryWriter::visit<std::wstring>(std::wstring &stringField, std::wstring defaultValue, const char *fieldName);


} //namespace corecvs

#endif // BINARY_WRITER_H
