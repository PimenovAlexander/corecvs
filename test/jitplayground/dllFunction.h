#ifndef DLLFUNCTION_H
#define DLLFUNCTION_H

#include "core/function/function.h"

struct SparseEntry {
   int i;
   int j;
   double val;
};

class DllFunction : public corecvs::FunctionArgs
{
public:
    typedef int  (*TestFunctor)(int);
    typedef int  (*DimentionFunctor)();
    typedef void (*FFunctor)(const double *, double *);
    typedef void (*JacobianPartFunctor)(const double *, SparseEntry *, int *);

    void *dllHandle = NULL;

    DimentionFunctor inputF;
    DimentionFunctor outputF;
    FFunctor f;
    vector<JacobianPartFunctor> jacobianParts;
    vector<int> jacobianPartStart;
    vector<int> jacobianPartSize;
    int nonTrivial = 0;

    DllFunction(const std::string &dllName);


    // FunctionArgs interface
public:
    virtual void operator ()(const double in[], double out[]) override
    {
        f(in, out);
    }

    corecvs::SparseMatrix getNativeJacobian(const double* in, double delta = 1e-7);

};


#endif // DLLFUNCTION_H
