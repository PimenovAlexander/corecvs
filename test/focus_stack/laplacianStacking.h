#ifndef LAPLACIANSTACKING_H
#define LAPLACIANSTACKING_H

#include "FSAlgorithm.h"
using namespace std;

class LaplacianStacking : public FSAlgorithm
{
public:
    LaplacianStacking();
    void doStacking(vector<RGB24Buffer*> & imageStack, RGB24Buffer * result) override;
    ~LaplacianStacking() override;
};

#endif // LAPLACIANSTACKING_H
