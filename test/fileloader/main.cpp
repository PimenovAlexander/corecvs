#include <iostream>
#include "bmpLoader.h"
#include "rgb24Buffer.h"
#include "libjpegFileReader.h"

using namespace std;
using namespace corecvs;

int main()
{
    LibjpegFileReader reader;
    RGB24Buffer *out = reader.load("first.jpg");
    BMPLoader().save("out.bmp", out);
    delete_safe(out);
    //cout << "out" << endl;
    return 0;
}

