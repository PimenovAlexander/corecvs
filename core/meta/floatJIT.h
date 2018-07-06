#ifndef FLOATJIT_H
#define FLOATJIT_H

#include "core/meta/astNode.h"

namespace corecvs {

class FloatJIT
{
public:
    FloatJIT();

    static void toAsm (ASTNodeInt *node);

};


} //namespace corecvs

#endif // FLOATJIT_H
