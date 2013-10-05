#pragma once

/**
 * \file LogicKernels.h
 *
 * \date Sep 24, 2013
 **/

namespace corecvs
{

class LogicKernels
{
};

template <typename Algebra>
class LogicAnd
{
public:
    static const int inputNumber = 2;
    static const int outputNumber = 1;

    inline static int getCenterX(){ return 0; };
    inline static int getCenterY(){ return 0; };
    inline static int getSizeX(){ return 1; };
    inline static int getSizeY(){ return 1; };

    typedef typename Algebra::InputType Type;

template <typename OtherAlgebra>
    LogicAnd(const LogicAnd<OtherAlgebra> &){}

    LogicAnd(){};

    void process(Algebra &algebra) const
    {
        Type a = algebra.getInput(0,0,0);
        Type b = algebra.getInput(1,0,0);
        Type result = a & b;
        algebra.putOutput(0,0,result);
        printf("%d & %d = %d\n", a, b, result);
    }
};

template <typename Algebra>
class LogicOr
{
public:
    static const int inputNumber = 2;
    static const int outputNumber = 1;

    inline static int getCenterX(){ return 0; };
    inline static int getCenterY(){ return 0; };
    inline static int getSizeX(){ return 1; };
    inline static int getSizeY(){ return 1; };

    typedef typename Algebra::InputType Type;

template <typename OtherAlgebra>
    LogicOr(const LogicOr<OtherAlgebra> &){}

    LogicOr(){};

    void process(Algebra &algebra) const
    {
        Type a = algebra.getInput(0,0,0);
        Type b = algebra.getInput(1,0,0);
        Type result = a | b;
        algebra.putOutput(0,0,result);
    }
};

template <typename Algebra>
class LogicXor
{
public:
    static const int inputNumber = 2;
    static const int outputNumber = 1;

    inline static int getCenterX(){ return 0; };
    inline static int getCenterY(){ return 0; };
    inline static int getSizeX(){ return 1; };
    inline static int getSizeY(){ return 1; };

    typedef typename Algebra::InputType Type;

template <typename OtherAlgebra>
    LogicXor(const LogicXor<OtherAlgebra> &){}

    LogicXor(){};

    void process(Algebra &algebra) const
    {
        Type a = algebra.getInput(0,0,0);
        Type b = algebra.getInput(1,0,0);
        Type result = a ^ b;
        algebra.putOutput(0,0,result);
    }
};

template <typename Algebra>
class LogicSubt
{
public:
    static const int inputNumber = 2;
    static const int outputNumber = 1;

    inline static int getCenterX(){ return 0; };
    inline static int getCenterY(){ return 0; };
    inline static int getSizeX(){ return 1; };
    inline static int getSizeY(){ return 1; };

    typedef typename Algebra::InputType Type;

template <typename OtherAlgebra>
    LogicSubt(const LogicSubt<OtherAlgebra> &){}

    LogicSubt(){};

    void process(Algebra &algebra) const
    {
        Type a = algebra.getInput(0,0,0);
        Type b = algebra.getInput(1,0,0);
        Type result = Algebra::andNot(a,b);
        algebra.putOutput(0,0,result);
    }
};


#if 0
template <typename Algebra>
class LogicNot
{
public:
    static const int inputNumber = 1;
    static const int outputNumber = 1;

    inline static int getCenterX(){ return 0; };
    inline static int getCenterY(){ return 0; };
    inline static int getSizeX(){ return 1; };
    inline static int getSizeY(){ return 1; };

    typedef typename Algebra::InputType Type;

template <typename OtherAlgebra>
    LogicNot(const LogicXor<OtherAlgebra> &){}

    LogicNot(){};

    void process(Algebra &algebra) const
    {
        Type a = algebra.getInput(0,0,0);
        Type b = algebra.getInput(1,0,0);
        Type result = ~a;
        algebra.putOutput(0,0,result);
    }
};
#endif

} /* namespace corecvs */
/* EOF */
