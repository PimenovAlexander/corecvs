#import "layerBuilder.h"

namespace corecvs {
    double max(const std::vector<std::vector<double>>& vec){
        std::vector<double> tmp;
        for (auto i : vec){
            double val = std::max_element(i.begin(), i.end())[0];
            tmp.push_back(val);
        }
        return std::max_element(tmp.begin(), tmp.end())[0];
    }

    double min(const std::vector<std::vector<double>>& vec){
        std::vector<double> tmp;
        for (auto i : vec){
            double val = std::min_element(i.begin(), i.end())[0];
            tmp.push_back(val);
        }
        return std::min_element(tmp.begin(), tmp.end())[0];
    }

    Layer::Layer(std::vector<Point> points, Interval singularityBounds) {
        this->points = points;
        this->singularityBounds = singularityBounds;
    }

    const vector<Point> &Layer::getPoints() const {
        return points;
    }

    Interval Layer::getSingularityBounds() {
        return singularityBounds;
    }

    LayersBuilder::LayersBuilder(RGB24Buffer *image, ImageChannel::ImageChannel channel, std::vector<int> windows) {
            this->converterImage = image->getChannelG16(channel);
            this->height = image->h;
            this->width = image->w;
            this->densities.resize(width - 2 * this->maxWindowSize,
                                   std::vector<double>(height - 2 * this->maxWindowSize, 0));
            this->windows = std::move(windows);
        }


    /// <summary>
    /// Определение границ сингулярности изображения
    /// </summary>
    /// <param name="image">Анализируемое изображение</param>
    /// <param name="channel">Тип конвертера</param>
    /// <returns>Интервал сингулярностей изображения</returns>
    Interval LayersBuilder::getSingularityBounds() {
        calculateDensity();
        return Interval(min(densities), max(densities));
    }

    /// <summary>
    /// Определение множеств уровня исходного изображения
    /// </summary>
    /// <param name="singularityBounds">Интервал сингулярности</param>
    /// <param name="singularityStep">Шаг сингулярности</param>
    /// <returns>Список слоёв, каждый из которых - список точек</returns>
    std::vector<Layer> LayersBuilder::splitByLayers(Interval singularityBounds, double singularityStep) {
        std::vector<Layer> layers = std::vector<Layer>();

        for (double sin = singularityBounds.getBegin(); sin <= singularityBounds.getEnd(); sin += singularityStep) {
            Interval layerSingularity = Interval(sin, sin + singularityStep);

            std::vector<Point> points = std::vector<Point>();

            for (int i = 0; i < this->densities.size(); i++) {
                for (int j = 0; j < this->height; j++) {
                    if (this->densities[i][j] >= sin && this->densities[i][j] < sin + singularityStep) {
                        points.emplace_back(i, j);
                    }
                }
            }

            layers.emplace_back(points, layerSingularity);
        }

        return layers;
    }

    /// <summary>
    /// Вычисление функции плотности для всех точек изображения
    /// </summary>
    void LayersBuilder::calculateDensity() {
        for (int i = maxWindowSize; i < width - maxWindowSize; i++) {
            for (int j = maxWindowSize; j < height - maxWindowSize; j++) {
                auto* point = new Point(i, j);
                double density = calculateDensityInPoint(point);
                this->maxDensity = std::max(this->maxDensity, density);
                this->minDensity = std::min(this->minDensity, density);
                this->densities[point->getX() - maxWindowSize][point->getY() - maxWindowSize] = density;
            }
        }
    }

    /// <summary>
    /// Вычисление функции плотности в окрестности данной точки
    /// </summary>
    /// <param name="point">Координаты точки</param>
    /// <returns>Значение функции плотности в окрестности данной точки</returns>
    double LayersBuilder::calculateDensityInPoint(Point *point) {
        std::vector<std::tuple<double, double>> points;

        for (auto windowSize : windows) {

            double intens = calculateIntensity(point, windowSize);

            double x = log2(2 * windowSize + 1);
            double y = log2(intens + 1);

            points.emplace_back(x, y);
        }

        return leastSquares(points);
    }

    /// <summary>
    /// Вычисление суммарной интенсивности пикселей в прямоугольнике
    /// </summary>
    /// <param name="point">Цетральная точка области</param>
    /// <param name="windowSize">Размер окна</param>
    /// <returns>Cуммарная интенсивность пикселей в области</returns>
    double LayersBuilder::calculateIntensity(Point *point, int windowSize) {
        int intensity = 0;

        for (int i = point->getX() - windowSize; i < point->getX() + windowSize; i++) {
            for (int j = point->getY() - windowSize; j < point->getY() + windowSize; j++) {
                intensity += this->converterImage.element(j, i);
            }
        }

        return intensity;
    }

}



