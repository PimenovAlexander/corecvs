#ifndef ASTNODE_H
#define ASTNODE_H

/**
 * \file astNode.h
 * \brief Abstract Syntax Tree Node
 *
 * \ingroup cppcorefiles
 */
#include "global.h"

#include <memory>
#include <vector>
#include <string>

namespace corecvs {

class ASTNodeInt;

class ASTContext
{
public:
    static ASTContext *MAIN_CONTEXT;

    std::vector<ASTNodeInt *> nodes;

    ~ASTContext()
    {
        SYNC_PRINT(("ASTContext::~ASTContext(): there is %d garbage remaining\n", (int)nodes.size()));
        for (auto it = nodes.begin(); it != nodes.end(); ++it)
        {
            delete_safe(*it);
        }
    }

};

struct ASTRenderDec {
    const char *ident;
    const char *lbr;
};

class ASTNodeInt
{
public:

    void _init()
    {
        if (ASTContext::MAIN_CONTEXT == NULL)
            return;

        ASTContext::MAIN_CONTEXT->nodes.push_back(this);
    }

    ASTNodeInt() {
        _init();
    }

    enum Operator {
        OPREATOR_ID,
        OPREATOR_NUM,

        OPERATOR_BINARY = OPREATOR_NUM,
        OPERATOR_ADD,
        OPERATOR_SUB,
        OPERATOR_MUL,
        OPERATOR_DIV,
        OPERATOR_LAST
    };


    ASTNodeInt(/* Context *_owner,*/ Operator _op, ASTNodeInt *_left = NULL, ASTNodeInt *_right = NULL) :
        op   (_op),
        left (_left),
        right(_right)
    {
        _init();
    }

    ASTNodeInt(double _value) :
        op   (OPREATOR_NUM),
        val  (_value),
        left (NULL),
        right(NULL)
    {
        _init();
    }

    ASTNodeInt(const char *_name) :
        op   (OPREATOR_ID),
        val  (0),
        name (_name),
        left (NULL),
        right(NULL)
    {
        _init();
    }

    ~ASTNodeInt()
    {
        if (ASTContext::MAIN_CONTEXT == NULL)
            return;
#if 0
        auto &vec = owner->nodes;
        vec.erase(std::remove(vec.begin(), vec.end(), this), vec.end());
#endif
    }

    Operator op;

    /*Context *owner;*/

    double val;
    std::string name;

    ASTNodeInt *left;
    ASTNodeInt *right;


   /*friend ASTNode operator +(const ASTNode &left, const ASTNode &right);
    friend ASTNode operator -(const ASTNode &left, const ASTNode &right);
    friend ASTNode operator /(const ASTNode &left, const ASTNode &right);*/


    void codeGenCpp (const std::string &name, ASTRenderDec identSym = {" ", "\n"});
    void codeGenCpp (int ident , ASTRenderDec identSym = {" ", "\n"});

};


class ASTNode {
public:
    ASTNodeInt *p;

    ASTNode() :
        p(new ASTNodeInt("UDEF"))
    {}

    ASTNode(int _value) :
        p(new ASTNodeInt((double)_value))
    {
        SYNC_PRINT(("ASTNode(%i): with id %p\n", _value, p));
    }

    ASTNode(double _value) :
        p(new ASTNodeInt(_value))
    {
        SYNC_PRINT(("ASTNode(%lf): with id %p\n", _value, p));
    }

    ASTNode(const char *_value) :
        p(new ASTNodeInt(_value))
    {
        SYNC_PRINT(("ASTNode(\"%s\"): with id %p\n", _value, p));
    }

    ASTNode(ASTNodeInt::Operator _op, const ASTNode &_left, const ASTNode &_right) :
        p(new ASTNodeInt(_op, _left.p, _right.p))
    {
       /* if (owner != NULL)
            _owner->nodes.push_back(this);*/
    }


//    friend ASTNode operator *(ASTNode left, ASTNode right);


};


inline ASTNode operator *(const ASTNode &left, const ASTNode &right)
{

    SYNC_PRINT(("Creating wrapper for (%p * %p)\n", left.p, right.p));
    return ASTNode(ASTNodeInt::OPERATOR_MUL, left, right);
}

inline ASTNode operator +(const ASTNode &left, const ASTNode &right)
{
    return ASTNode(ASTNodeInt::OPERATOR_ADD, left, right);
}

inline ASTNode operator +=(ASTNode &left, const  ASTNode &right)
{
    left.p = new ASTNodeInt(ASTNodeInt::OPERATOR_ADD, left.p, right.p);
    return left;
}


inline ASTNode operator -(const ASTNode &left, const ASTNode &right)
{
    return ASTNode(ASTNodeInt::OPERATOR_SUB, left, right);
}


inline ASTNode operator /(const ASTNode &left, const ASTNode &right)
{
    return ASTNode(ASTNodeInt::OPERATOR_DIV, left, right);
}


/*ASTNode operator +(const ASTNode &left, const ASTNode &right)
{
    return ASTNode(ASTNode::OPERATOR_ADD, &left, &right);
}

ASTNode operator -(const ASTNode &left, const ASTNode &right)
{
    return ASTNode(ASTNode::OPERATOR_SUB, &left, &right);
}

ASTNode operator /(const ASTNode &left, const ASTNode &right)
{
    return ASTNode(ASTNode::OPERATOR_DIV, &left, &right);
}*/

} //namespace corecvs

#endif // ASTNODE_H
