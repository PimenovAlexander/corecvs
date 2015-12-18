#include "astNode.h"

namespace corecvs {

void ASTNodeInt::codeGenCpp(const std::string &name, ASTRenderDec identSym)
{
    printf("double %s() {\n", name.c_str());
    printf("return (%s", identSym.lbr);
    codeGenCpp(1, identSym);
    printf("%s);\n", identSym.lbr);
    printf("}\n");
}

void ASTNodeInt::codeGenCpp(int ident, ASTRenderDec identSym)
{
    for (int i = 0; i < ident; ++i) {
        printf("%s", identSym.ident);
    }

    if (op == OPREATOR_ID) {
        printf("%s", name.c_str());
        return;
    }

    if (op == OPREATOR_NUM) {
        printf("%lf", val);
        return;
    }

    if (op >= OPERATOR_BINARY && op < OPERATOR_LAST)
    {
        printf ("(%s", identSym.lbr);
        left->codeGenCpp(ident + 1, identSym);
        printf ("%s", identSym.lbr);

        for (int i = 0; i < ident; ++i) {
            printf("%s", identSym.ident);
        }
        switch (op) {
            case OPERATOR_ADD : printf("+"); break;
            case OPERATOR_SUB : printf("-"); break;
            case OPERATOR_MUL : printf("*"); break;
            case OPERATOR_DIV : printf("/"); break;
            default           : printf(" UNSUPPORTED "); break;
        }
        printf ("%s", identSym.lbr);
        right->codeGenCpp(ident + 1, identSym);
        printf ("%s", identSym.lbr);
        for (int i = 0; i < ident; ++i) {
            printf("%s", identSym.ident);
        }
        printf (")%s", identSym.lbr);
        return;
    }

}



} //namespace corecvs
