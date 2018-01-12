#ifndef ADVANCED_BINARY_READER_H
#define ADVANCED_BINARY_READER_H


#include <stdint.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include "core/reflection/reflection.h"
#include "core/utils/log.h"

#include "core/reflection/advanced/advancedBinaryWriter.h"

namespace corecvs {

using std::string;
using std::istream;
using std::cout;


/**
   \attention NB Please note!!

   This visitor is not storing data in "key->value" form.
   It only stores values, so it assumes fields would be requested in the same order they have been written

   There would be no checks or warnings so far. You would just experience fast yet painful death
**/
class AdvancedBinaryReader : AdvancedBinaryVisitorBase
{
public:
    bool isSaver () { return false;}
    bool isLoader() { return true;}


public:
    std::istream *stream  = NULL;
    bool fileOwned = false;

    typedef std::map<std::string, size_t> NameDict;

    std::vector<size_t> sizes;
    std::vector<NameDict> dicts;





    explicit  AdvancedBinaryReader(std::istream *istream):
        stream(istream)
    {
        prologue();
    }

    explicit  AdvancedBinaryReader(const string &filepath)
        : stream(new std::ifstream(filepath.c_str(), std::ifstream::in | std::ios::binary)),
          fileOwned(true)
    {
        if (stream == NULL || !stream->good())
        {
            L_ERROR_P("Couldn't open for reading file <%s>", filepath.c_str());
            delete_safe(stream);
            fileOwned = false;
            return;
        }
        SYNC_PRINT(("reading from <%s>\n", filepath.c_str()));
        prologue();
    }

    ~AdvancedBinaryReader()
    {
       if (fileOwned) {
           epilogue();
           delete_safe(stream);
           fileOwned = false;
       }
    }

    /*file prologue*/
    void prologue()
    {
        const char *greeting = MAGIC_STRING1;
        char greeting_read[10];
        if (stream == NULL) return;

        stream->read(greeting_read, strlen(greeting));
        greeting_read[strlen(greeting)] = 0;

        cout << "Read greating:" << greeting_read << std::endl;
        if (strcmp(greeting, greeting_read) != 0)
        {
            cout << "Failed to recognize the format <"  <<  greeting << "> <" << greeting_read << ">" << std::endl;
        }

        uint32_t size = 0;
        stream->read((char *)&size, sizeof(size));
        cout << "Block size: " << size << std::endl;
        sizes.push_back(size);
        dicts.push_back(readDictionary());

        if (!stream->good())
        {
            cout << "Stream is not good" << std::endl;
        }
    }

    void epilogue()
    {
        const char *bye = MAGIC_STRING2;
        char bye_read[10];
        if (stream == NULL) return;
        stream->read(bye_read, strlen(bye));
        bye_read[strlen(bye)] = 0;
        cout << "Read bye:" << bye_read << std::endl;
    }

    std::map<std::string, size_t> readDictionary()
    {
        std::map<std::string, size_t> result;

        if (stream == NULL) return result;

        size_t pos = stream->tellg();
        std::string name;

        int count = 0;
        size_t limit = sizes.back() + stream->tellg();

        while (true)
        {
            uint32_t len = 0;
            name = readString(stream);

            size_t posF =  stream->tellg();
            stream->read((char *)&len, sizeof(len));
            if (len == 0) {
                break;
            }
            result.insert(std::pair<std::string, size_t>(name, posF));

            cout << "<" << name << ">" << " " << "len:" << len << std::endl;
            stream->seekg(posF + len + sizeof(len));

            count++;
            if (stream->bad()) {
                cout << "Unexpected stream crash" << std::endl;
                break;
            }
            if ((size_t)stream->tellg() >= limit) {
                cout << "Object end found " << stream->tellg() << " " << limit << std::endl;
                break;
            }
        }

        stream->seekg(pos);
        return result;
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
        field.accept(*this);       
    }


    /**
     * Complex objects support with old style calls
     **/
template <class Type>
    void visit(Type &field, const char * fieldName)
    {
        if (stream == NULL) return;

        auto it = dicts.back().find(fieldName);
        if (it == dicts.back().end())
        {
            cout << "Name not found:" << fieldName << std::endl;
            return;
        }

        stream->seekg(it->second);

        uint32_t len = 0;
        stream->read((char *)&len, sizeof(len));
        sizes.push_back(len);
        dicts.push_back(readDictionary());
       // std::string name = readString(stream);
        field.accept(*this);

        sizes.pop_back();
        dicts.pop_back();
    }

template <typename Type>
    void visit(Type &field, Type /*defaultValue*/, const char * fieldName)
    {
        visit<Type>(field, fieldName);
    }

    bool repositionToField(const char *name);

template <typename Type>
    bool loadField(Type &field, const std::string &name);

};


template <>
void AdvancedBinaryReader::visit<int,    IntField>(int &field, const IntField *fieldDescriptor);

template <>
void AdvancedBinaryReader::visit<uint64_t, UInt64Field>(uint64_t &field, const UInt64Field *fieldDescriptor);

template <>
void AdvancedBinaryReader::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor);

template <>
void AdvancedBinaryReader::visit<float,  FloatField>(float &field, const FloatField *fieldDescriptor);

template <>
void AdvancedBinaryReader::visit<bool,   BoolField>(bool &field, const BoolField *fieldDescriptor);

template <>
void AdvancedBinaryReader::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor);

template <>
void AdvancedBinaryReader::visit<int,    EnumField>(int &field, const EnumField *fieldDescriptor);

template <>
void AdvancedBinaryReader::visit<std::string, StringField>(std::string &field, const StringField *fieldDescriptor);

template <>
void AdvancedBinaryReader::visit<std::wstring, WStringField>(std::wstring &field, const WStringField *fieldDescriptor);


/* Arrays */

template <>
void AdvancedBinaryReader::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor);

/**
 * Old Style
 *
 * this methods can be made universal, but are separated to make it a bit more controllable
 **/
template <>
void AdvancedBinaryReader::visit<uint64_t>(uint64_t &intField, uint64_t defaultValue, const char *fieldName);

template <>
void AdvancedBinaryReader::visit<bool>(bool &boolField, bool defaultValue, const char *fieldName);

template <>
void AdvancedBinaryReader::visit<int>(int &boolField, int defaultValue, const char *fieldName);

template <>
void AdvancedBinaryReader::visit<double>(double &boolField, double defaultValue, const char *fieldName);


template <>
void AdvancedBinaryReader::visit<std::string>(std::string &stringField, std::string defaultValue, const char *fieldName);

template <>
void AdvancedBinaryReader::visit<std::wstring>(std::wstring &stringField, std::wstring defaultValue, const char *fieldName);


} //namespace corecvs

#endif // ADVANCED_BINARY_READER_H
