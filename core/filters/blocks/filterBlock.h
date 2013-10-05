#pragma once

/**
 * \file filterBlock.h
 *
 * \date Nov 9, 2012
 **/
#include <vector>
#include <algorithm>
#include <string.h>
#include <sstream>

#include "global.h"
#include "reflection.h"
#include "pins.h"
#include <typeinfo>

namespace corecvs
{

class FilterGraph;

using std::vector;

typedef int PinId;

class FilterBlock
{
public:
    enum SortOfBlock
    {   INPUT_FILTER,
        OUTPUT_FILTER,
        PROCESSING_FILTER,
        COMPOUND_FILTER
    };

    FilterBlock(/*FilterGraph* _parent,*/ int id = -1);

    virtual void setInstanceName(char* name)
    {
        if (instanceName)  free(instanceName);
        instanceName = name;
    }
    virtual char* getInstanceName() { return instanceName; }

    class NamePredicate
    {
        const char *mName;
    public:
        NamePredicate(const char *name) : mName(name) {}

        bool operator() (Pin *pin)
        {
            return strcmp(pin->instanceName, mName) == 0;
        }
    };

    class IdPredicate
    {
        const int mId;
    public:
        IdPredicate(const int id) : mId(id) {}

        bool operator() (Pin *pin)
        {
            return pin->getInstanceId() == mId;
        }
    };

    virtual int getInstanceId() const { return -1; }
    virtual const char* getTypeName() const { return "none"; }

    virtual string getFullName()
    {
        std::ostringstream oss;
        oss << getTypeName() << " " << getInstanceId();
        return oss.str();
    }

    virtual int operator()(void)
    {
        printf("FilterBlock::Executing block %s\n", instanceName);
        return 0;
    }

    virtual bool setParameters(const void *newParameters)
    {
        printf("Setting parameters %p\n", newParameters);
        return false;
    }

    virtual void* getParameters()
    {
        printf("Getting parameters\n");
        return NULL;
    }

    bool isOuterPin(Pin* pin)
    {
        return (sort == INPUT_FILTER  && pin->type == Pin::INPUT_PIN) ||
               (sort == OUTPUT_FILTER && pin->type == Pin::OUTPUT_PIN);
    }

    Pin* PinByName(vector<Pin*> &vec, const char *name)
    {
        vector<Pin*>::iterator pin = std::find_if(vec.begin(), vec.end(), NamePredicate(name));
        if (pin == vec.end())  return NULL;

        return *pin;
    }

    Pin* inPinByName(const char *name) { return PinByName(inputPins,  name); }
    Pin* outPinByName(const char *name){ return PinByName(outputPins, name); }

    Pin* PinById(vector<Pin*> &vec, int id)
    {
        vector<Pin*>::iterator pin = std::find_if(vec.begin(), vec.end(), IdPredicate(id));
        if (pin == vec.end())  return NULL;

        return *pin;
    }

    Pin *inPinById(int id) { return PinById(inputPins,  id); }
    Pin *outPinById(int id){ return PinById(outputPins, id); }

    virtual XMLNode* serialize(XMLNode*);
    virtual void deserialize(XMLNode*, bool force = true);

    virtual void clear();
    virtual ~FilterBlock();

private:
    void deserializePins(vector<Pin*> &vec, XMLNode* node, bool force);
// ------------- members --------------
public:
    int x;
    int y;

    FilterGraph* parent;
    int instanceId;
    int inLinks;
    SortOfBlock sort;
    vector<Pin*> inputPins;
    vector<Pin*> outputPins;
protected:
    char* instanceName;
}; // FilterBlock

//------------------------------------------------------------------------------------

//template<class GeneratedStub>
//class GeneratedFilterBlock : public GeneratedStub, public FilterBlock
//{
//public:
//    GeneratedFilterBlock(const char *_name = "none") : FilterBlock(_name)
//    { }

//    virtual const char * getTypeName() {
//        Reflection *reflection = GeneratedStub::getReflection();
//        ReflectionNaming &naming = reflection->name;
//        return naming.name;
//    }

//    virtual int operator()(void)
//    {
//        printf("FilterBlock::Executing block <%s>:%s \n", getTypeName(), instanceName);
//        return 0;
//    }

//    virtual void *getPin(PinId id)
//    {
//        printf("FilterBlock::Getting pin %s:%d\n", instanceName, id);
//        vector<const BaseField *>& fields = GeneratedStub::fields();
//        for (unsigned i = 0; i < fields.size(); i++)
//        {
//            const char *pinName = fields[i]->getSimpleName();
//            if (strncmp(pinName, "out", 3) != 0) {
//                continue;
//            }
//            if (id == 0) {
//                return *((void **)GeneratedStub::getPtrById(i));
//            }
//            id--;
//        }
//        return NULL;
//    } // getPin

//    virtual void setPin(PinId id, void *data)
//    {
//        printf("FilterBlock::Setting pin %s:%d\n", instanceName, id);
//        vector<const BaseField *>& fields = GeneratedStub::fields();
//        for (unsigned i = 0; i < fields.size(); i++)
//        {
//            const char *pinName = fields[i]->getSimpleName();
//            if (strncmp(pinName, "in", 2) != 0) {
//                continue;
//            }
//            if (id == 0)
//            {
//                *((void **)GeneratedStub::getPtrById(i)) = data;
//            }
//            id--;
//        }
//    } // setPin

//    virtual PinId getMaxPinInId()  {
//        PinId id = 0;
//        vector<const BaseField *>& fields = GeneratedStub::fields();
//        for (unsigned i = 0; i < fields.size(); i++)
//        {
//            const char *pinName = fields[i]->getSimpleName();
//            if (strncmp(pinName, "in", 2) != 0) {
//                continue;
//            }
//            id++;
//        }
//        return id;
//    }

//    virtual PinId getMaxPinOutId() {
//        PinId id = 0;
//        vector<const BaseField *>& fields = GeneratedStub::fields();
//        for (unsigned i = 0; i < fields.size(); i++)
//        {
//            const char *pinName = fields[i]->getSimpleName();
//            if (strncmp(pinName, "out", 3) != 0) {
//                continue;
//            }
//            id++;
//        }
//        return id;
//    }

//}; // GeneratedFilterBlock



//class FilterConnection
//{
//public:
//    FilterBlock *start;
//    FilterBlock *end;
//    PinId        startPin;
//    PinId        endPin;


//    FilterConnection(
//        FilterBlock *_start    = NULL,
//        PinId        _startPin = -1,
//        FilterBlock *_end      = NULL,
//        PinId        _endPin   = -1
//    ) :
//        start(_start),
//        end  (_end),
//        startPin(_startPin),
//        endPin  (_endPin)
//    {}

//    virtual ~FilterConnection();
//};

} /* namespace corecvs */
/* EOF */
