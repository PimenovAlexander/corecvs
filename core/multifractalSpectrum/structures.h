//
// Created by gleb on 19.11.2020.
//

#ifndef CORECVS_STRUCTURE_H
#define CORECVS_STRUCTURE_H
    class Point {
    public:
        Point(int x, int y);

        int getX();

        int getY();
        ~Point() = default;

    private:
        int x;
        int y;
    };

    class Interval {
    public:
        Interval();

        Interval(double begin, double end);

        double getBegin();

        double getEnd();

    private:
        double begin{};
        double end{};
    };

#endif //CORECVS_STRUCTURE_H