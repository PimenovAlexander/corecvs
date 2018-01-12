
#if defined (__GNUC__)
#include <dlfcn.h>
#endif
#include <iostream>

#include "dllFunction.h"
#include "core/tbbwrapper/tbbWrapper.h"

using namespace std;
using namespace corecvs;

DllFunction::DllFunction(const std::string &dllName) :
    FunctionArgs(0,0) /*OOPS... bad design*/
{
    SYNC_PRINT(("Loading DLL...\n"));

    dllHandle = dlopen (dllName.c_str(), RTLD_LAZY);
    if (!dllHandle) {
        fprintf (stderr, "%s\n", dlerror());
        exit(1);
    }
    /*char *error =*/ dlerror();    /* Clear any existing error */

    const char *testName = "test";
    void *testF = dlsym(dllHandle, testName);

    f = (FFunctor)dlsym(dllHandle, "function");
    inputF  = (DimentionFunctor)dlsym(dllHandle, "input");
    outputF = (DimentionFunctor)dlsym(dllHandle, "output");

    if (testF != NULL) {
        TestFunctor testCall = (TestFunctor)testF;
        cout << "Dll test call result:" << testCall(41) << endl;
    }

    nonTrivial = 0;
    int jpi = 0;
    while (true) {
        char buffer[100];
        snprintf2buf(buffer, "j%d", jpi);
        cout << "Try to load symbol <" << buffer << ">" << endl;


        JacobianPartFunctor jp = (JacobianPartFunctor)dlsym(dllHandle, buffer);
        if (jp == NULL)
            break;
        jacobianParts.push_back(jp);
        int partSize = 0;
        jp(NULL, NULL, &partSize);
        jacobianPartStart.push_back(nonTrivial);
        jacobianPartSize .push_back(partSize);
        nonTrivial += partSize;
        jpi++;
    }

    cout << "Loaded " << jacobianParts.size() << " jacobian parts" << endl;

    inputs  = inputF();
    outputs = outputF();
}

corecvs::SparseMatrix DllFunction::getNativeJacobian(const double *in, double /*delta*/)
{
    vector<SparseEntry> scratch;
    scratch.resize(nonTrivial);

    parallelable_for(0, (int)jacobianParts.size(),
        [&](const corecvs::BlockedRange<int> &r)
        {
            for (int k = r.begin(); k < r.end(); k++)
            {
                int partStart = jacobianPartStart[k];
                jacobianParts[k](in, &scratch[partStart], NULL);
            }
        }
    );

    /* There is a double copy and we also can call blocks in parallel */
    std::map<std::pair<int, int>, double> data;
    for (SparseEntry &entry : scratch) {
        data[std::pair<int, int>(entry.i, entry.j)] = entry.val;
        /*
        if (entry.i > inputs || entry.j > outputs)
        {
            SYNC_PRINT(("Out of boundary"));
        }*/
    }

    return corecvs::SparseMatrix(inputs, outputs, data);
}
