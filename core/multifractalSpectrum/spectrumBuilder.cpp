#include "spectrumBuilder.h"

#include <utility>
//
// Created by gleb on 22.11.2020.
//

/// default constructor
corecvs::SpectrumBuilder::SpectrumBuilder(vector<int> windows, const string& path) {
    this->windows = std::move(windows);
    this->layersDirectoryPath = path;
}

/// <summary>
/// Вычисление мультифрактального спектра: создание уровней и измерение их размерности
/// </summary>
/// <param name="image">Анализируемое изображение</param>
/// <param name="layers">Множества уровня</param>
/// <param name="singularityBounds">Интервал сингулярности</param>
/// <param name="singularityStep">Шаг сингулярности</param>
/// <returns>Мультифракальный спектр изображения</returns>
std::map<double, double> corecvs::SpectrumBuilder::calculateSpectrum(corecvs::RGB24Buffer *image, const vector<Layer>& layers,
                                                                     Interval singularityBounds,
                                                                     double singularityStep){
    double currentLayerSingularity = singularityBounds.getBegin();
    auto spectrum = std::map<double, double>();

    for (const auto& layer : layers)
    {
        double measure = createAndMeasureLayer(image, layer);
        spectrum[currentLayerSingularity] = measure;

        currentLayerSingularity += singularityStep;
    }

    return spectrum;
}

/// <summary>
/// Создание изображения, соответствующего данному уровню, и его измерение
/// </summary>
/// <param name="image">Анализируемое изображение</param>
/// <param name="layer">Множество уровня</param>
/// <returns>Изображение слоя и его фрактальная размерность</returns>
double corecvs::SpectrumBuilder::createAndMeasureLayer(corecvs::RGB24Buffer* image, const corecvs::Layer& layer)
{
    int newWidth = image->w - this->maxWindowSize * 2;
    int newHeight = image->h - maxWindowSize * 2;
    auto layerImage = new RGB24Buffer(newWidth, newHeight);

    for (int i = 0; i < layerImage->w; i++)
    {
        for (int j = 0; j < layerImage->h; j++)
        {
            layerImage->drawPixel(i, j, RGBColor(255, 255, 255, 255));
        }
    }

    for (auto point : layer.getPoints())
    {
        layerImage->drawPixel(point.getX(), point.getY(), RGBColor(0, 0, 0, 255));
    }

    saveLayerImage(layer, layerImage);
    double result = calculateMeasure(layerImage);
    delete_safe(layerImage);
    return result;
}

/// <summary>
/// Вычисление фрактальной размерности изображения
/// </summary>
/// <param name="image">Анализируемое изображение</param>
/// <returns>Фрактальная размерность изображения</returns>
double corecvs::SpectrumBuilder::calculateMeasure(RGB24Buffer *image) {
    std::vector<std::tuple<double, double>> points;
    for (auto windowSize : windows)
    {
        double intens = calculateBlackWindows(image, windowSize);

        double x = log2(windowSize);
        double y = log2(intens + 1);

        points.emplace_back(x, y);
    }

    return -leastSquares(points);
}

/// <summary>
/// Подсчёт числа квадратиков, имеющих внутри себя хотя бы один чёрный пиксель
/// </summary>
/// <param name="image">Исследуемая область изображения</param>
/// <param name="window">Размер окна</param>
/// <returns>Число квадратиков, имеющих внутри себя хотя бы один чёрный пиксель </returns>
double corecvs::SpectrumBuilder::calculateBlackWindows(corecvs::RGB24Buffer *image, int window) {
    double blackWindows = 0;

    for (int i = 0; i < image->w - window; i += window)
    {
        for (int j = 0; j < image->h - window; j += window)
        {
            if (hasBlackPixel(image, i, j, window))
            {
                blackWindows++;
            }
        }
    }

    return blackWindows;
}

/// <summary>
/// Проверка, содержит ли прямоугольник хотя бы один чёрный пиксель
/// </summary>
/// <param name="image">прямоугольник, который изучаем</param>
/// <param name="start_x">левая верхняя координата по оси x</param>
/// <param name="start_y">левая верхняя координата по оси y</param>
/// <param name="window">размер окна</param>
/// <returns>Результат проверки</returns>
bool corecvs::SpectrumBuilder::hasBlackPixel(corecvs::RGB24Buffer *image, int start_x, int start_y, int window) {
    for (int i = start_x; i < start_x + window; i++)
    {
        for (int j = start_y; j < start_y + window; j++)
        {
            auto color = image->element(i, j);
            if (color.b() == 0 && color.r() == 0 && color.g() == 0)
            {
                return true;
            }
        }
    }

    return false;
}

void corecvs::SpectrumBuilder::saveLayerImage(corecvs::Layer layer, corecvs::RGB24Buffer *layerImage) {
    auto minSingularity = std::to_string(round(layer.getSingularityBounds().getBegin() * 100) / 100);
    auto maxSingularity = std::to_string(round(layer.getSingularityBounds().getEnd() * 100) / 100);

    std::vector<string> name{"layer: ", minSingularity, " ", maxSingularity, ".bmp"};
    string layerName;
    for (const auto& n : name){ layerName += n;}
    string pathToImage = std::filesystem::path(layersDirectoryPath) / layerName;

    //path is cmake-build-debug/test-core/layer*
    BMPLoader().save(pathToImage, layerImage);
}
