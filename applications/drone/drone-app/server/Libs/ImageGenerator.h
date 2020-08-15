//
// Created by dio on 7/26/20.
//

#ifndef LIBEVENTAPP_IMAGEGENERATOR_H
#define LIBEVENTAPP_IMAGEGENERATOR_H

#include <iostream>
#include <fstream>

int width = 255;
int height = 255;
int max_colour = 255;

int curImg = 0;

const char *generateImage(int id = 0)
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
        case 0: return "Pages/img/jojo0.jpg";
        case 1: return "Pages/img/jojo1.jpg";
        case 2: return "Pages/img/jojo2.jpg";
        default: return "Pages/img/jojo3.jpg";
    }
}

#endif //LIBEVENTAPP_IMAGEGENERATOR_H
