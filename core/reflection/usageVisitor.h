#ifndef USAGE_VISITOR_H_
#define USAGE_VISITOR_H_

#include <stdint.h>
#include <iostream>
#include <sstream>

#include "core/reflection/reflection.h"

namespace corecvs {

class UsagePrinter
{
public:
    UsagePrinter() {}

    static void printUsage(const Reflection *feflection);
};

} //namespace corecvs
#endif // USAGE_VISITOR_H_
