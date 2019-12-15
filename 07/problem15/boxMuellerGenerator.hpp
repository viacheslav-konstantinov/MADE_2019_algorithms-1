#include <random>
#include <time.h>
#include <vector>
#include <cmath>

//Генератор стандартных нормально распределённых случайных величин на базе
//преобразования Бокса-Мюллера

std::vector<std::pair<double, double>> boxMuellerGenerator(size_t numberOfPoints = 10)
{
    std::vector<std::pair<double, double>> result;

    size_t counter = 0;
    int base = 1000000;
    double x, y, s;

    std::srand(time(NULL));

    while(counter < numberOfPoints)
    {
        x = (base - 2 * (std::rand() % base)) / double(base);
        y = (base - 2 * (std::rand() % base)) / double(base);

        s = x*x + y*y;
        if(s <= 1 && s != 0)
        {
            x = x * std::sqrt(-2 * std::log(s) / s);
            y = y * std::sqrt(-2 * std::log(s) / s);
            ++counter;
            result.push_back(std::make_pair(x, y));
        }
    }

    return result;
}

