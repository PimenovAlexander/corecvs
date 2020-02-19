#ifndef PINS_H
#define PINS_H
/**
 * \file pins.h
 * \brief TODO
 *
 * \date Dec 14, 2012
 * \author dpiatkin
 */

#include "buffers/g12Buffer.h"
#include "tinyxml2/tinyxml2.h"

namespace corecvs {

using namespace tinyxml2;

class FilterBlock;

class Pin
{
public:
   enum PinType {
        INPUT_PIN,
        OUTPUT_PIN
   };

   Pin(FilterBlock* _parent, PinType _type, const char *name = "none") :
       type(_type),
       parent(_parent),
       takeFrom(NULL)
   {
       instanceName = strdup(name);
   }

   virtual ~Pin();

   int getInstanceId() const { return instanceId; }
   virtual void setInstanceId(int) {}

   virtual void clear() {}

   void breakConnection() { takeFrom = NULL; }

//   const char* getFullName()

   virtual const char* getTypeName() const { return "none"; }

   virtual bool isEmpty() { return false; }
   virtual bool setPin(Pin*, bool = false)
   {   std::cerr << "error Abstract\n";
       return false;
   }
   virtual bool getPin(G12Buffer*&, bool = false)
   {   std::cerr << "error G12\n";
       return false;
   }
   virtual bool getPin(char*&, bool = false)
   {   std::cerr << "error Txt\n";
       return false;
   }
//   virtual bool getPin(G8Buffer*&) { stderr << "error G8"; }

   virtual void initPin(G12Buffer* /*data*/) { std::cerr << "error G12 initPin"; }

   virtual XMLNode* serialize(XMLNode* doc, bool outerPin);
   virtual void deserialize(XMLNode*);

   char* instanceName;
   PinType type;
   FilterBlock* parent;
   int instanceId;
   Pin* takeFrom;
}; // Pin

//--------------------------------------------------------------------------

class G12Pin : public Pin
{
public:
   G12Pin(FilterBlock* _parent, PinType _type, bool auto_init, const char *name = "none") :
       Pin(_parent, _type, name),
       inout(NULL)
   {
       if (auto_init)  instanceId = ++instanceCounter;
   }

   virtual void setInstanceId(int id)
   {
       instanceId = id;
       instanceCounter = CORE_MAX(instanceCounter, instanceId);
   }

   virtual ~G12Pin() {}
   virtual const char* getTypeName() const { return "G12Pin"; }
   virtual bool isEmpty() { return inout == NULL; }

   virtual bool setPin(Pin* source, bool shouldCopy = false)
   {
       if (NULL != source)
            return source->getPin(inout, shouldCopy);
       else return false;
   }

   virtual bool getPin(G12Buffer* &data, bool shouldCopy = false)
   {
       if (shouldCopy && inout != NULL)
            data = new G12Buffer(inout);
       else data = inout;
       return true;
   }

   virtual void initPin(G12Buffer* data) { inout = data; }
   G12Buffer* &getData() { return inout; }

   virtual void clear() { delete_safe(inout); }

   virtual void deserialize(XMLNode*);

private:
   G12Buffer* inout;
   static int instanceCounter;
};

//--------------------------------------------------------------------------

class TxtPin : public Pin
{
public:
   TxtPin(FilterBlock* _parent, PinType _type, bool auto_init, const char *name = "none") :
       Pin(_parent, _type, name),
       inout(NULL)
   {
       if (auto_init)  instanceId = ++instanceCounter;
   }

   virtual void setInstanceId(int id)
   {
       instanceId = id;
       instanceCounter = CORE_MAX(instanceCounter, instanceId);
   }

   virtual void clear() { delete_safe(inout); }
   virtual ~TxtPin() {}

   virtual const char* getTypeName() const { return "TxtPin"; }

   virtual bool isEmpty() { return inout == NULL; }

   virtual bool setPin(Pin* source, bool shouldCopy = false)
   {
       if (NULL != source)
            return source->getPin(inout, shouldCopy);
       else return false;
   }

   virtual bool getPin(char* &data, bool shouldCopy = false)
   {
       if (shouldCopy)
       {    data = new char[strlen(inout)+1];
            strncpy(data, inout, 10000);
       }
       else data = inout;
       return true;
   }

   virtual void initPin(char* &data) { inout = data; }
   char* &getData() { return inout; }

   virtual void deserialize(XMLNode*);

private:
   char* inout;
   static int instanceCounter;
};



//--------------------------------------------------


class OutputPin;

class InputPin : public Pin
{
public:
    InputPin(FilterBlock* _parent, const char *name = "none") :
        Pin(_parent, INPUT_PIN, name),
        takeFrom(NULL)
   {}

   virtual ~InputPin();

   void breakConnection() { takeFrom = NULL; }

   virtual bool setPin(OutputPin*, bool = false)
   {   std::cerr << "error Abstract\n";
       return false;
   }

   virtual XMLNode* serialize(XMLNode* doc, bool outerPin);
   virtual void deserialize(XMLNode*);

   OutputPin* takeFrom;
}; // InputPin

//-----------------------------------------------------------

class OutputPin : public Pin
{
public:
   OutputPin(FilterBlock* _parent, const char *name = "none") :
       Pin(_parent, OUTPUT_PIN, name)
   {}

   virtual ~OutputPin();

   void breakConnection()
   {
       for (unsigned int j = 0; j < connectedTo.size(); j++)
          connectedTo[j]->takeFrom = NULL;
       connectedTo.clear();
   }

   virtual bool getPin(G12Buffer*&, bool = false)
   {   std::cerr << "error G12\n";
       return false;
   }
   virtual bool getPin(char*&, bool = false)
   {   std::cerr << "error Txt\n";
       return false;
   }
//   virtual bool getPin(G8Buffer*&, bool = false) { stderr << "error G8"; }

   virtual XMLNode* serialize(XMLNode* doc, bool outerPin);
   virtual void deserialize(XMLNode*);

   vector<InputPin*> connectedTo;
}; // OutputPin

//----------------------------------------------------------

template<typename TypeOfPin>
class ExtraG12Pin : public TypeOfPin
{
public:
   ExtraG12Pin(FilterBlock* _parent, Pin::PinType _type, bool auto_init, const char *name = "none") :
       TypeOfPin(_parent, _type, name),
       inout(NULL)
   {
       if (auto_init)  TypeOfPin::instanceId = ++instanceCounter;
   }

   virtual void setInstanceId(int id)
   {
       TypeOfPin::instanceId = id;
       instanceCounter = max(instanceCounter, TypeOfPin::instanceId);
   }

   virtual ~ExtraG12Pin() {}

   virtual const char* getTypeName() const { return "G12Pin"; }

   virtual bool isEmpty() { return inout == NULL; }

   virtual bool setPin(OutputPin* source, bool shouldCopy = false)
   {
       if (NULL != source)
            return source->getPin(inout, shouldCopy);
       else return false;
   }

   virtual bool getPin(G12Buffer* &data, bool shouldCopy = false)
   {
       if (shouldCopy)
            data = new G12Buffer(inout);
       else data = inout;
       return true;
   }

   virtual void initPin(G12Buffer* data) { inout = data; }
   G12Buffer* &getData() { return inout; }

   virtual void clear() { delete_safe(inout); }

   virtual XMLNode* serialize(XMLNode* doc, bool outerPin);
   virtual void deserialize(XMLNode*);

private:
   G12Buffer* inout;
   static int instanceCounter;
}; // ExtraG12Pin

//--------------------------------------------------------

template<typename TypeOfPin>
class ExtraTxtPin : public TypeOfPin
{
public:
   ExtraTxtPin(FilterBlock* _parent, Pin::PinType _type, bool auto_init, const char *name = "none") :
       TypeOfPin(_parent, _type, name),
       inout(NULL)
   {
       if (auto_init)  TypeOfPin::instanceId = ++instanceCounter;
   }

   virtual void setInstanceId(int id)
   {
       TypeOfPin::instanceId = id;
       instanceCounter = max(instanceCounter, TypeOfPin::instanceId);
   }

   virtual void clear() { delete_safe(inout); }
   virtual ~ExtraTxtPin() {}

   virtual const char* getTypeName() const { return "G12Pin"; }

   virtual bool isEmpty() { return inout == NULL; }

   virtual bool setPin(Pin* source, bool shouldCopy = false)
   {
       if (NULL != source)
            return source->getPin(inout, shouldCopy);
       else return false;
   }

   virtual bool getPin(char* &data, bool shouldCopy = false)
   {
       if (shouldCopy)
       {    data = new char[strlen(inout)+1];
            strncpy(data, inout, 10000);
       }
       else data = inout;
       return true;
   }

   virtual void initPin(char* &data) { inout = data; }
   char* &getData() { return inout; }

   virtual XMLNode* serialize(XMLNode* doc, bool outerPin);
   virtual void deserialize(XMLNode*);

private:
   char* inout;
   static int instanceCounter;
}; // ExtraTxtPin

} //namespace corecvs
#endif // PINS_H
