#include <iostream>

#include "astNode.h"

namespace corecvs {

using std::endl;
using std::cout;

ASTContext *ASTContext::MAIN_CONTEXT = NULL;

void ASTNodeInt::codeGenCpp(const std::string &name, ASTRenderDec identSym)
{
    if (!identSym.genParameters) {
        printf("double %s() {\n", name.c_str());
    } else {
        std::vector<std::string> params;
        getVars(params);

        printf("double %s(", name.c_str());

        for (size_t i = 0; i < params.size(); i++)
        {
           printf("%sdouble %s", (i == 0) ? "" : ", ", params[i].c_str());
        }
        printf(") {\n");

    }
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

    if (op > OPERATOR_BINARY && op <= OPERATOR_BINARY_LAST)
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
    if (op == OPERATOR_POW)
    {
        printf ("%s", identSym.lbr);
        printf("pow(");
        printf ("%s", identSym.lbr);
        left->codeGenCpp(ident + 1, identSym);
        printf(",");
        right->codeGenCpp(ident + 1, identSym);
        printf ("%s", identSym.lbr);
        printf(")");
        printf ("%s", identSym.lbr);

    }

}

void ASTNodeInt::print()
{
    switch (op) {
        case OPREATOR_ID: {
            cout << name;
            break;
        }

        case OPREATOR_NUM: {
            cout << "value(" << val << ")";
            break;
        }

        case OPERATOR_ADD : {
            cout << "+";
            break;
        }
        case OPERATOR_SUB : {
            cout << "-";
            break;
        }
        case OPERATOR_MUL : {
            cout << "*";
            break;
        }
        case OPERATOR_DIV : {
            cout << "/";
            break;
        }

        case OPERATOR_POW : {
            cout << "pow";
            break;
        }
        case OPERATOR_SIN: {
            cout << "sin";
            break;
        }
        case OPERATOR_COS: {
            cout << "cos";
            break;
        }

        default   : {
            cout << " UNSUPPORTED ";
            break;
        }
    }
}

void ASTNodeInt::getVars(std::vector<std::string> &result)
{
    if (op == OPREATOR_ID) {
        if (std::find(result.begin(), result.end() ,name) == result.end()) {
            result.push_back(name);
        }
    } else {
        //cout << "ZZ ";
        if (left != NULL)
            left ->getVars(result);
        if (right != NULL)
            right->getVars(result);
    }
}

ASTNodeInt *ASTNodeInt::derivative(const std::string &var)
{
    switch (op) {
        case OPREATOR_ID: {
            if (name == var)
                return new ASTNodeInt(1.0);
            else
                return new ASTNodeInt(0.0);
            break;
        }

        case OPREATOR_NUM: {
            return new ASTNodeInt(0.0);
            break;
        }

        case OPERATOR_ADD : {
            return new ASTNodeInt(OPERATOR_ADD, left->derivative(var), right->derivative(var));
            break;
        }
        case OPERATOR_SUB : {
            return new ASTNodeInt(OPERATOR_SUB, left->derivative(var), right->derivative(var));
            break;
        }
        case OPERATOR_MUL : {
            ASTNodeInt *leftD  = left->derivative(var);
            ASTNodeInt *rightD = right->derivative(var);


            return new ASTNodeInt(OPERATOR_ADD,
                          new ASTNodeInt(OPERATOR_MUL, leftD, right ),
                          new ASTNodeInt(OPERATOR_MUL,  left, rightD)
                   );
            break;
        }
        case OPERATOR_DIV : {
            ASTNodeInt *leftD  = left->derivative(var);
            ASTNodeInt *rightD = right->derivative(var);


            return new ASTNodeInt(OPERATOR_DIV,
                        new ASTNodeInt(OPERATOR_SUB,
                               new ASTNodeInt(OPERATOR_MUL, leftD, right ),
                               new ASTNodeInt(OPERATOR_MUL,  left, rightD)
                        ),
                        new ASTNodeInt(OPERATOR_MUL, right,  right)
                   );
            break;
        }
        case OPERATOR_POW : {
            /*So far partial support */
            if (right->op == OPREATOR_NUM) {
                ASTNodeInt *leftD  = left->derivative(var);

                return new ASTNodeInt(OPERATOR_MUL,
                       new ASTNodeInt(right->val),
                       new ASTNodeInt(OPERATOR_MUL,
                               new ASTNodeInt(OPERATOR_POW,
                                    left,
                                    new ASTNodeInt(right->val - 1.0)
                               ),
                               leftD
                           )
                       );
            }
            return new ASTNodeInt("UDEF");
            break;
        }
        default   :
            printf(" UNSUPPORTED OPERATOR %d");
            return new ASTNodeInt("UDEF");
        break;
    }


}

ASTNodeInt *ASTNodeInt::compute(const std::map<std::string, double> &bind)
{
    // print();
    // cout << endl;
    switch (op) {
        case OPREATOR_ID: {
            auto it = bind.find(name);
            if (it != bind.end())
            {
                return new ASTNodeInt(it->second);
            } else {
                return this;
            }
            break;
        }

        case OPREATOR_NUM: {
            return this;
            break;
        }

        case OPERATOR_ADD : {
            ASTNodeInt *nleft  = left->compute(bind);
            ASTNodeInt *nright = right->compute(bind);

            if (nleft->op == OPREATOR_NUM && nright->op == OPREATOR_NUM)
            {
                return new ASTNodeInt(nleft->val + nright->val);
            }
            if (nleft->op == OPREATOR_NUM && nleft->val == 0.0)
            {
                return nright;
            }
            if (nright->op == OPREATOR_NUM && nright->val == 0.0)
            {
                return nleft;
            }
            return new ASTNodeInt(OPERATOR_ADD, nleft, nright);
            break;
        }
        case OPERATOR_SUB : {
            ASTNodeInt *nleft  = left->compute(bind);
            ASTNodeInt *nright = right->compute(bind);

            if (nleft->op == OPREATOR_NUM && nright->op == OPREATOR_NUM)
            {
                return new ASTNodeInt(nleft->val - nright->val);
            }
            if (nright->op == OPREATOR_NUM && nright->val == 0.0)
            {
                return nleft;
            }
            return new ASTNodeInt(OPERATOR_SUB, nleft, nright);
            break;
        }
        case OPERATOR_MUL : {
            ASTNodeInt *nleft  = left->compute(bind);
            ASTNodeInt *nright = right->compute(bind);

            if (nleft->op == OPREATOR_NUM && nright->op == OPREATOR_NUM)
            {
                return new ASTNodeInt(nleft->val * nright->val);
            }
            if (nleft->op == OPREATOR_NUM && nleft->val == 0.0)
            {
                return new ASTNodeInt(0.0);
            }
            if (nright->op == OPREATOR_NUM && nright->val == 0.0)
            {
                return new ASTNodeInt(0.0);
            }
            if (nleft->op == OPREATOR_NUM && nleft->val == 1.0)
            {
                return nright;
            }
            if (nright->op == OPREATOR_NUM && nright->val == 1.0)
            {
                return nleft;
            }
            return new ASTNodeInt(OPERATOR_MUL, nleft, nright);
            break;
        }
        case OPERATOR_DIV : {
            ASTNodeInt *nleft  = left->compute(bind);
            ASTNodeInt *nright = right->compute(bind);

            if (nleft->op == OPREATOR_NUM && nright->op == OPREATOR_NUM)
            {
                return new ASTNodeInt(nleft->val / nright->val);
            }
            if (nright->op == OPREATOR_NUM && nright->val == 1.0)
            {
                return nleft;
            }
            if (nleft->op == OPREATOR_NUM && nleft->val == 0.0)
            {
                return new ASTNodeInt(0.0);
            }
            return new ASTNodeInt(OPERATOR_DIV, nleft, nright);
            break;
        }
        case OPERATOR_POW : {
            ASTNodeInt *nleft  = left->compute(bind);
            ASTNodeInt *nright = right->compute(bind);

            if (nleft->op == OPREATOR_NUM && nright->op == OPREATOR_NUM)
            {
                return new ASTNodeInt(pow(nleft->val, nright->val));
            }
            return new ASTNodeInt(OPERATOR_POW, nleft, nright);
        }
        default   :
            printf("ASTNodeInt::compute(): UNSUPPORTED\n");
            return new ASTNodeInt("UDEF");
        break;
    }
}



} //namespace corecvs
