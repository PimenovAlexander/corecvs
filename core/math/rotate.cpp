#include <cstdio>
#include <iostream>
#include <string>
#include <cmath>
#include "core/buffers/rgb24/rgb24Buffer.h"

namespace corecvs {


    double pi = 3.14159265358979323846;

    inline double sinc(double x) {
        x = (x * pi);
        return sin(x) / x;
    }

    inline double LanczosFilter(double x, double lanczos_size) {
        if (std::abs(x) < 0.0001) {
            return 1.0;
        }
        if (std::abs(x) < lanczos_size) {
            return (sinc(x) * sinc(x / lanczos_size));
        }

        return 0.0;
    }

    inline double maxOf4(double d1, double d2, double d3, double d4){
        return std::max(d1, std::max(d2, std::max(d3, d4)));
    }

    inline double minOf4(double d1, double d2, double d3, double d4){
        return std::min(d1, std::min(d2, std::min(d3, d4)));
    }

    RGB24Buffer* rotate_with_lanczoc_filter(double angle, RGB24Buffer rgb24buffer, double lanczos_size = 3.0) {

        float sin_angle = sin(pi / 180 * angle);
        float cos_angle = cos(pi / 180 * angle);

        double x_center = rgb24buffer.getW() / 2;
        double y_center = rgb24buffer.getH() / 2;

        double x;
        double y;

        x = -x_center;
        y = -y_center;

        double x1 = x * cos_angle - y * sin_angle;
        double y1 = x * sin_angle + y * cos_angle;

        x = -x_center;
        y = y_center;
        double x2 = x * cos_angle - y * sin_angle;
        double y2 = x * sin_angle + y * cos_angle;

        x = x_center;
        y = y_center;

        double x3 = x * cos_angle - y * sin_angle;
        double y3 = x * sin_angle + y * cos_angle;

        x = x_center;
        y = -y_center;

        double x4 = x * cos_angle - y * sin_angle;
        double y4 = x * sin_angle + y * cos_angle;

        double x_new_top = maxOf4(x1, x2, x3, x4);
        double y_new_top = maxOf4(y1, y2, y3, y4);

        double x_new_bottom = minOf4(x1, x2, x3, x4);
        double y_new_bottom = minOf4(y1, y2, y3, y4);

        int new_width = x_new_top - x_new_bottom;
        int new_height = y_new_top - y_new_bottom;

         RGB24Buffer *rotated_image = new RGB24Buffer(new_height, new_width);


        for (int i = 0; i < new_width; i++) {
            x = i + x_new_bottom;

            for (int j = 0; j < new_height; j++) {

                y = j + y_new_bottom;

                double x_within = x * cos_angle + y * sin_angle + x_center;
                double y_within = -x * sin_angle + y * cos_angle + y_center;

                int x_src = x_within;
                int y_src = y_within;

                double weight = 0.0;
                int red = 0;
                int green = 0;
                int blue = 0;
                for (int i_sample = x_src - lanczos_size + 1; i_sample <= x_src + lanczos_size; ++i_sample) {
                    for (int j_sample = y_src - lanczos_size + 1; j_sample <= y_src + lanczos_size; ++j_sample) {

                        double lanc_val = LanczosFilter(x_within - i_sample, lanczos_size) * LanczosFilter(y_within - j_sample, lanczos_size);
                        weight += lanc_val;
                        if (i_sample >= 0 && i_sample < rgb24buffer.getW() && j_sample >= 0 && j_sample < rgb24buffer.getH()) {

                            red += rgb24buffer.element(j_sample, i_sample).r() * lanc_val,
                            green += rgb24buffer.element(j_sample, i_sample).g() * lanc_val,
                            blue += rgb24buffer.element(j_sample, i_sample).b() * lanc_val;

                        }
                    }
                }

                red /= weight;
                green /= weight;
                blue /= weight;

                red = (red > 255) ? 255 : red;
                red = (red < 0) ? 0 : red;

                green = (green > 255) ? 255 : green;
                green = (green < 0) ? 0 : green;

                blue = (blue > 255) ? 255 : blue;
                blue = (blue < 0) ? 0 : blue;

                rotated_image->drawPixel(i, j, RGBColor(red, green, blue));
            }

        }

        return rotated_image;
    }
}
/*
int main(){
    rotate_with_lanczoc_filter(180, "image.bmp");
    int wait;
    std::cin >> wait;
    return 0;
}*/



