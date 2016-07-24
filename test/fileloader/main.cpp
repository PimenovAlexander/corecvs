#include <iostream>
#include "bmpLoader.h"
#include "rgb24Buffer.h"
#include "libjpegFileReader.h"
#include "libpngFileReader.h"

using namespace std;
using namespace corecvs;

int main()
{
    PNGLoader reader;
    RGB24Buffer *out = reader.loadRGB("first.png");
    BMPLoader().save("first.bmp", out);
    delete_safe(out);
    RGB24Buffer *out1 = BMPLoader().loadRGB("first.bmp");
    PNGLoader().save("out.png", out1);
    delete_safe(out1);
    cout << "out" << endl;
    return 0;
}

