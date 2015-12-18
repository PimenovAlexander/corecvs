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

class Context
{
public:
    std::vector<ASTNodeInt *> nodes;

};

struct ASTRenderDec {
    const char *ident;
    const char *lbr;
};

class ASTNodeInt
{
public:

    ASTNodeInt() {}

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
       /* owner(_owner),*/
        op   (_op),
        left (_left),
        right(_right)
    {
       /* if (owner != NULL)
            _owner->nodes.push_back(this);*/
    }

    ASTNodeInt(/*Context *_owner,*/ double _value) :
       /* owner(_owner),*/
        op   (OPREATOR_NUM),
        val  (_value),
        left (NULL),
        right(NULL)
    {
    }

    ASTNodeInt(/*Context *_owner,*/ const char *_name) :
       /* owner(_owner),*/
        op   (OPREATOR_ID),
        val  (0),
        name (_name),
        left (NULL),
        right(NULL)
    {
    }

    ~ASTNodeInt()
    {
     /*   if (owner == NULL)
            return;

        auto &vec = owner->nodes;
        vec.erase(std::remove(vec.begin(), vec.end(), this), vec.end());*/
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
