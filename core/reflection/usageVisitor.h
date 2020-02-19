#ifndef USAGE_VISITOR_H_
#define USAGE_VISITOR_H_

#include <stdint.h>
#include <iostream>
#include <sstream>

#include "reflection/reflection.h"
#include "reflection/dynamicObject.h"

namespace corecvs {

class UsagePrinter
{
public:
    static const std::string SEPARATOR;
    UsagePrinter() {}

    /**
     * \attention Discouraged
     **/
    static void printUsage(const Reflection *reflection, const std::string &addNamespace);

    static void printUsage(DynamicObjectWrapper *wrapper, const std::string &addNamespace);
};

} //namespace corecvs
#endif // USAGE_VISITOR_H_
