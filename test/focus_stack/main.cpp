#include <iostream>
#include "core/buffers/bufferFactory.h"
#include "core/fileformats/bmpLoader.h"
#include "imageStack.h"
#include "laplacianStacking.h"


using namespace std;
using namespace corecvs;

int main(int argc, char *argv[])
{
    if (argc != 2) {
        SYNC_PRINT(("Wrong amount of arguments!\n"));
        return 1;
    }

    ImageStack * imageStack = ImageStack::loadStack("/home/adminlinux/Desktop/testStack", 10);
    if (imageStack == nullptr)
    {
        SYNC_PRINT(("Can't load images!\n"));
    }
    imageStack->focus_stack(LaplacianStacking());
    imageStack->saveMegredImage("/home/adminlinux/Desktop");
}

