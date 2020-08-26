#include <sstream>
#include "jsonContent.h"


JSONContent::JSONContent() :
    result (false)
{
}

JSONContent::~JSONContent()
{
}

std::vector<uint8_t> JSONContent::getContent()
{
    std::ostringstream data;

    data << "{\n"
         << "   \"result\":" << (result ? "true" : "false") << ",\n"
         << "   \"value\":" << value << "\n"
         << "}\n" ;

    std::string str = data.str();
    return std::vector<uint8_t>(str.begin(), str.end());
}

void JSONContent::setResult(bool res)
{
    result = res;
}


void JSONContent::setValue(std::string val)
{
    value = val;
}

