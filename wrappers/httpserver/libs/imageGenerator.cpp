#include <iostream>
#include <fstream>

#include "imageGenerator.h"

int width = 255;
int height = 255;
int max_colour = 255;
int curImg = 0;

const char *generateImage(int id)
{
    curImg = (curImg + 1) % 4;
#ifdef generate_ppm
    std::ofstream file;
    file.open("picture.ppm");
    file << "P3 " << width << " " << height << " " << max_colour << " ";
    for(int y = id; y < id+height; y++)
    {
        for(int x = id; x < id+width; x++)
        {
            file << x % max_colour << " " << y % max_colour << " " << x * y % max_colour << " ";
        }
    }
    file.close();
#endif
    switch (curImg) {
        case 0: return "pages/img/someOtherImages/jojo0.jpg";
        case 1: return "pages/img/jojo1.jpg";
        case 2: return "pages/img/jojo2.jpg";
        default: return "pages/img/jojo3.jpg";
    }
}

