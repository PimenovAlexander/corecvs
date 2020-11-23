#include "leastSquares.h"

/// <summary>
/// Уточнение результата вычислений с помощью МНК
/// </summary>
/// <param name="points">множество точек и значений в них</param>
/// <returns>Уточнённое значение</returns>
    double leastSquares(const std::vector<std::tuple<double, double>>& points) {
        int n = points.size();
        double xsum = 0, ysum = 0, xysum = 0, xsqrsum = 0, x = 0, y = 0;

        for (auto point : points) {
            std::tie(x, y) = point;
            xsum += x;
            ysum += y;
            xsqrsum += x * x;
            xysum += x * y;
        }

        return 1.0 * (n * xysum - xsum * ysum) / (n * xsqrsum - xsum * xsum);
    }