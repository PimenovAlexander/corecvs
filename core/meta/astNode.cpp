#include <iostream>
#include <cmath>

#include <sstream>

#include "core/meta/astNode.h"

namespace corecvs {

using std::endl;
using std::cout;

ASTContext *ASTContext::MAIN_CONTEXT = NULL;

ASTRenderDec ASTNodeInt::identSymDefault = {" ", "\n", true};
ASTRenderDec ASTNodeInt::identSymLine    = {"", "", true};


bool ASTNodeInt::isBinary() {
    return (op > OPERATOR_BINARY && op <= OPERATOR_BINARY_LAST);
}

bool ASTNodeInt::isUnary() {
    return (op == OPERATOR_POW || op == OPERATOR_SIN || op == OPERATOR_COS);
}

void ASTNodeInt::codeGenCpp(const std::string &name, ASTRenderDec &identSym)
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

void ASTNodeInt::codeGenCpp(int ident, ASTRenderDec &identSym)
{
    std::ostream &output = identSym.output;

    for (int i = 0; i < ident; ++i) {
        output << identSym.ident;
    }

    if (op == OPREATOR_ID) {
        output << name.c_str();
        return;
    }

    if (op == OPREATOR_NUM) {
        output.precision(std::numeric_limits<double>::digits10 + 3);
        if (val >= 0) {
            output << val;
        } else {
            output << "(" << val << ")";
        }
        return;
    }
    /* Here we can check for CSE */
    if (identSym.cse != NULL && hash != 0)
    {
        auto it = identSym.cse->find(hash);
        if (it != identSym.cse->end() && (*it).second->cseCount > 0)
        {
            int cseName = (*it).second->cseName;

            output << "cse" << std::hex << cseName << std::dec;
            return;
        }
    }


    if (op > OPERATOR_BINARY && op <= OPERATOR_BINARY_LAST)
    {
        int currentPrior = getPriority(op);
        int leftPrior = getPriority(left()->op);

        if (currentPrior >= leftPrior)
            output << "(" << identSym.lbr;
        left()->codeGenCpp(ident + 1, identSym);
        if (currentPrior >= leftPrior)
            output << ")" << identSym.lbr;

        output << identSym.lbr;

        for (int i = 0; i < ident; ++i) {
            output << identSym.ident;
        }
        switch (op) {
            case OPERATOR_ADD : output << "+" ; break;
            case OPERATOR_SUB : output << "-" ; break;
            case OPERATOR_MUL : output << "*" ; break;
            case OPERATOR_DIV : output << "/" ; break;
            default           : output << " UNSUPPORTED " ; break;
        }

        int rightPrior = getPriority(right()->op);
        if (currentPrior >= rightPrior)
            output << "(" << identSym.lbr;
        //output <<  identSym.lbr;
        right()->codeGenCpp(ident + 1, identSym);
        if (currentPrior >= rightPrior)
            output << ")" << identSym.lbr;

        output << identSym.lbr;
        for (int i = 0; i < ident; ++i) {
            output << identSym.ident;
        }
        return;
    }
    if (op == OPERATOR_POW)
    {
        if (right()->op == OPREATOR_NUM && right()->val == 0.5)
        {
            output <<  identSym.lbr;
            output <<  "sqrt(";
            output <<  identSym.lbr;
            left()->codeGenCpp(ident + 1, identSym);
            output << ")";
            output <<  identSym.lbr;
        } else if (right()->op == OPREATOR_NUM && right()->val == -0.5) {
            output <<  identSym.lbr;
            output <<  "(1.0 / sqrt(";
            output <<  identSym.lbr;
            left()->codeGenCpp(ident + 1, identSym);
            output << "))";
            output <<  identSym.lbr;
        } else {
            output <<  identSym.lbr;
            output <<  "pow(";
            output <<  identSym.lbr;
            left()->codeGenCpp(ident + 1, identSym);
            output << ",";
            right()->codeGenCpp(ident + 1, identSym);
            output <<  identSym.lbr;
            output << ")";
            output << identSym.lbr;
        }
        return;
    }
    if (op == OPERATOR_FUNCTION)
    {
        /* packing input */

        output <<  identSym.lbr;        
        output << name;
        output << identSym.lbr;
        return;
    }

}

void ASTNodeInt::extractConstPool(const std::string &poolname, std::unordered_map<double, std::string> &pool)
{
    if (op == OPREATOR_NUM)
    {
        //cout << "Checking " << val << " to constpool" << endl;
        auto it = pool.find(val);
        if (it == pool.end()) {
            int id = (int)pool.size();
            char str[1000];
            snprintf2buf(str, "%s%d", poolname.c_str(), id);
            op = OPREATOR_ID;
            name = str;
            pool[val] = str;
            //cout << "Adding " << val << " to constpool" << endl;
        } else {
            op = OPREATOR_ID;
            name = (*it).second;
        }
    }

    for (ASTNodeInt *child : children)
        child->extractConstPool(poolname, pool);

}

void ASTNodeInt::cseR(std::unordered_map<uint64_t, ASTNodeInt *> &cse)
{
    auto it = cse.find(hash);
    if (it == cse.end()) {
        /* We are not intested of making cse out of consts and ids */
        if (isBinary())
        {
            left() ->cseR(cse);
            right()->cseR(cse);
            cse[hash] = this;
        }

        if (isUnary())
        {
            left()->cseR(cse);
            cse[hash] = this;
        }
    } else {
        (*it).second->cseCount++;
        if (isBinary())
        {
            left() ->cseR(cse);
            right()->cseR(cse);
        }

        if (isUnary())
        {
            left()->cseR(cse);
        }
    }
}

size_t ASTNodeInt::memoryFootprint()
{
    if (op == OPREATOR_ID)
    {
        return sizeof(ASTNodeInt);
    }

    if (op == OPREATOR_NUM)
    {
        return sizeof(ASTNodeInt);
    }

    if (isBinary())
    {

        return sizeof(ASTNodeInt) + left()->memoryFootprint() + right()->memoryFootprint();
    }

    if (isUnary())
    {
        return sizeof(ASTNodeInt) + left()->memoryFootprint();
    }

    return 0;
}

void ASTNodeInt::rehash()
{
    cseCount = 0;
    cseName = 0;

    if (op == OPREATOR_ID)
    {
        hash = std::hash<std::string>{}(name);
        height = 1;
        return;
    }

    if (op == OPREATOR_NUM)
    {
        hash = std::hash<double>{}(val);
        height = 1;
        return;
    }

    if (isBinary())
    {
        left ()->rehash();
        right()->rehash();

        hash = left()->hash + 15485867 * (right()->hash + 141650963 * std::hash<int>{}(op));

        height = 1 + std::max(left()->height, right()->height);
        return;
    }

    if (isUnary())
    {
        left()->rehash();
        hash = left()->hash + 256203161 * std::hash<int>{}(op);
        height = 1 + left()->height;
        return;
    }

    cout << "ASTNodeInt::rehash():UNSUPPORTED " << getName(op) << endl;
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

/*
void ASTNodeInt::printRec(int ident, ASTRenderDec identSym)
{


}*/

void ASTNodeInt::getVars(std::vector<std::string> &result)
{
    if (op == OPREATOR_ID) {
        if (std::find(result.begin(), result.end() ,name) == result.end()) {
            result.push_back(name);
        }
    } else {
        for (ASTNodeInt *child : children)
            child->getVars(result);
    }
}

ASTNodeInt *ASTNodeInt::derivative(const std::string &var)
{
    switch (op)
    {
        case OPREATOR_ID:
        {
            if (name == var)
                return new ASTNodeInt(1.0);
            else
                return new ASTNodeInt(0.0);
            break;
        }

        case OPREATOR_NUM:
        {
            return new ASTNodeInt(0.0);
            break;
        }

        case OPERATOR_ADD:
        {
            return new ASTNodeInt(OPERATOR_ADD, left()->derivative(var), right()->derivative(var));
            break;
        }

        case OPERATOR_SUB:
        {
            return new ASTNodeInt(OPERATOR_SUB, left()->derivative(var), right()->derivative(var));
            break;
        }

        case OPERATOR_MUL:
        {
            ASTNodeInt *leftD  = left() ->derivative(var);
            ASTNodeInt *rightD = right()->derivative(var);


            return new ASTNodeInt(OPERATOR_ADD,
                          new ASTNodeInt(OPERATOR_MUL, leftD, right() ),
                          new ASTNodeInt(OPERATOR_MUL,  left(), rightD)
                   );
            break;
        }

        case OPERATOR_DIV:
        {
            ASTNodeInt *leftD  = left() ->derivative(var);
            ASTNodeInt *rightD = right()->derivative(var);


            return new ASTNodeInt(OPERATOR_DIV,
                        new ASTNodeInt(OPERATOR_SUB,
                               new ASTNodeInt(OPERATOR_MUL, leftD, right() ),
                               new ASTNodeInt(OPERATOR_MUL,  left(), rightD)
                        ),
                        new ASTNodeInt(OPERATOR_MUL, right(),  right())
                   );
            break;
        }

        case OPERATOR_POW:
        {
            /*So far partial support */
            if (right()->op == OPREATOR_NUM) {
                ASTNodeInt *leftD  = left()->derivative(var);

                return new ASTNodeInt(OPERATOR_MUL,
                       new ASTNodeInt(right()->val),
                       new ASTNodeInt(OPERATOR_MUL,
                               new ASTNodeInt(OPERATOR_POW,
                                    left(),
                                    new ASTNodeInt(right()->val - 1.0)
                               ),
                               leftD
                           )
                       );
            }
            return new ASTNodeInt("UDEF");
            break;
        }

        case OPERATOR_FUNCTION:
        {
            ASTNodeFunctionPayload *fpay = static_cast<ASTNodeFunctionPayload *>(payload);
            if (fpay == NULL)
            {
                return new ASTNodeInt();
            }
            return fpay->derivative(var);
        }

        default:
            printf(" UNSUPPORTED OPERATOR %d", op);
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
            ASTNodeInt *nleft  = left ()->compute(bind);
            ASTNodeInt *nright = right()->compute(bind);

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
            ASTNodeInt *nleft  = left ()->compute(bind);
            ASTNodeInt *nright = right()->compute(bind);

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
            ASTNodeInt *nleft  = left ()->compute(bind);
            ASTNodeInt *nright = right()->compute(bind);

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
            ASTNodeInt *nleft  = left ()->compute(bind);
            ASTNodeInt *nright = right()->compute(bind);

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
            ASTNodeInt *nleft  = left ()->compute(bind);
            ASTNodeInt *nright = right()->compute(bind);

            if (nleft->op == OPREATOR_NUM && nright->op == OPREATOR_NUM)
            {
                return new ASTNodeInt(pow(nleft->val, nright->val));
            }
            return new ASTNodeInt(OPERATOR_POW, nleft, nright);
        }
        case OPERATOR_FUNCTION:
        {
            return this;
        }

        default   :
            printf("ASTNodeInt::compute(): UNSUPPORTED OP %d - %s\n", op, getName(op));
            return new ASTNodeInt("UDEF");
        break;
    }
}

void ASTNodeInt::deleteSubtree(ASTNodeInt *tree)
{
    tree->deleteChildren();
    delete(tree);
}

void ASTNodeInt::deleteChildren()
{
    for (ASTNodeInt *child : children)
    {
        child->deleteChildren();
        delete child;
    }
}



void ASTContext::clear() {
    for (ASTNodeInt *node : nodes)
    {
        node->markFlag = 0;
    }
}

void ASTContext::mark(ASTNodeInt *node) {
    node->markFlag = 1;

    for (ASTNodeInt *child : node->children)
        mark(child);
}


void ASTContext::sweep() {
    nodes.erase(
        remove_if(nodes.begin(), nodes.end(),
                  [](ASTNodeInt *node){
            return (node->markFlag == 0);
        }));
}

std::string ASTNodeFunctionPayload::getCCode()
{
    return std::string(
       "#error We don't know how to generate this code __FILE__ : __LINE__ : __FUNCTION__\n"
       "return;\n"
                );
}

ASTNodeFunctionPayload *ASTNodeFunctionPayload::derivative(int /*input*/)
{
    return NULL;
}

std::vector<std::string> ASTNodeFunctionWrapper::getVars()
{
    std::vector<std::string> vars;

    for (int i = 0; i < outputNumber(); i++)
    {
        ASTNodeInt *comp = components[i];
        comp->getVars(vars);
    }

    for (size_t i = 0; i < vars.size(); i++)
    {
        std::string cur = vars[i];
        vars.erase(
            remove_if(
                    vars.begin() + i + 1,
                    vars.end(),
                    [&cur](const std::string &in){ return in == cur; }
            ),
            vars.end()
        );
    }

    std::sort(vars.begin(), vars.end());
    return vars;
}

int ASTNodeFunctionWrapper::inputNumber()
{
    return (int)getVars().size();
}

int ASTNodeFunctionWrapper::outputNumber()
{
    return (int)components.size();
}

void ASTNodeFunctionWrapper::f(double in[], double out[])
{
    std::vector<std::string> vars = getVars();

    //int ins  = (int)vars.size();
    int outs = outputNumber();

    std::map<std::string, double> binds;

    for (size_t i = 0; i < vars.size(); i++)
    {
        binds.insert(std::pair<std::string, double>(vars[i], in[i]));
    }

    for (int i = 0; i < outs; i++)
    {
        ASTNodeInt *node = components[i];
        ASTNodeInt *c = node->compute(binds);

        if (c != NULL && c->op != ASTNodeInt::OPREATOR_NUM)
        {
            out[i] = c->val;
        }
    }
}

std::string ASTNodeFunctionWrapper::getCCode()
{
    /* We need to add here CSE, Constpool, etc.*/

    std::ostringstream out;
    for (int i = 0; i < outputNumber(); i++)
    {
        ASTNodeInt *node = components[i];
        ASTRenderDec params("", "", false, out);

        out << "out[" << i << "] = \n";
        node->codeGenCpp(2, params);
        out << ";\n";
    }

    return out.str();
}

ASTNodeFunctionPayload *ASTNodeFunctionWrapper::derivative(int)
{
    return NULL;
}

ASTNodeInt *ASTNodeShortcutFunction::derivative(const std::string &varname)
{
    for (size_t i = 0; i < params.size(); i++)
    {
        if (params[i] == varname) {
           /* return new ASTNodeInt(ASTNodeInt::OPERATOR_MUL,
                                  new ASTNodeInt(name),
                                  new ASTNodeInt(derivatives[i])
                                  );*/
            return new ASTNodeInt(derivatives[i]);
        }
    }
    return new ASTNodeInt(0.0);
}

} //namespace corecvs
