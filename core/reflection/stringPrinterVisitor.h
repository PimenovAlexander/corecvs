#ifndef STRING_PRINTER_VISITOR_H_
#define STRING_PRINTER_VISITOR_H_
/**
 * \file stringPrinterVisitor.h
 *
 * \date May 12, 2014
 **/

#include <sstream>
#include "printerVisitor.h"

namespace corecvs
{

using std::stringstream;

class StringPrinterVisitor : public PrinterVisitor
{
public:
    std::stringstream ss;

    StringPrinterVisitor() :
        PrinterVisitor(ss)
    {}

    string str()
    {
        return ss.str();
    }
};

} /* namespace corecvs */

#endif // STRING_PRINTER_VISITOR_H_
